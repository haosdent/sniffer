#include <pcap.h>
#include <signal.h>
#include <sched.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>
#include <errno.h>
#include <sys/poll.h>
#include <time.h>
#include <netinet/in_systm.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/ip6.h>
#include <net/ethernet.h>     /* the L2 protocols */

#include "config.h"
#include "pfring.h"
#include "list.h"
#include "tools.h"

#define DEFAULT_DEVICE "eth0"
#define OUTPUT "result_pfring.txt"

struct list_head* record_list;

void break_handler(int s)
{
	struct record *r;
	FILE *fp = fopen(OUTPUT, "w");

	if (fp == NULL) {
		printf("Error when open result.txt.");
		exit(1);
	};

	list_for_each_entry(r, record_list, node) {
		printf("Src host: %u:%u, Dst host: %u:%u, Packet Count: %u\n", r->src_ip, r->src_port, r->dst_ip, r->dst_port, r->count);
		fprintf(fp, "Src host: %u:%u, Dst host: %u:%u, Packet Count: %u\n", r->src_ip, r->src_port, r->dst_ip, r->dst_port, r->count);
	}

	fclose(fp);
	exit(1);
}

int main(int argc, char* argv[])
{
	printf("Use Ctrl + C to kill this program. The results would be written to result.txt.\n");

	struct sigaction sig_handler;
	sigemptyset(&sig_handler.sa_mask);
	sig_handler.sa_handler = break_handler;
	sig_handler.sa_flags = 0;

	sigaction(SIGINT, &sig_handler, NULL);
	sigaction(SIGTERM, &sig_handler, NULL);
	sigaction(SIGQUIT, &sig_handler, NULL);

	char *device = DEFAULT_DEVICE;
	u_int flags = PF_RING_LONG_HEADER | PF_RING_STRIP_HW_TIMESTAMP;
	pfring *pd;
	u_char *p;
	struct pfring_pkthdr hdr;
	LIST_HEAD(head);
	record_list = &head;

	memset(&hdr, 0, sizeof(hdr));

	if ((pd = pfring_open(device, 1520, flags)) == NULL) {
		printf("pfring_open error");
		exit(-1);
	} else
		pfring_set_application_name(pd, "sniffer_pfring");

	pfring_enable_ring(pd);

	for (; ; ) {
		if (pfring_recv(pd, &p, 0, &hdr, 1) > 0) {
			uint32_t src_ip = hdr.extended_hdr.parsed_pkt.ipv4_src;
			uint32_t dst_ip = hdr.extended_hdr.parsed_pkt.ipv4_dst;
			uint16_t src_port = hdr.extended_hdr.parsed_pkt.l4_src_port;
			uint16_t dst_port = hdr.extended_hdr.parsed_pkt.l4_dst_port;

			struct record *r;
			int flag = 0;
			list_for_each_entry(r, &head, node) {
				if (r->src_ip == src_ip && r->dst_ip == dst_ip
				    && r->src_port == src_port && r->dst_port == dst_port) {
					r->count++;
					flag = 1;
					break;
				};
			}

			if (0 == flag) {
				r = (struct record *)malloc(sizeof(*r));
				r->src_ip = src_ip;
				r->dst_ip = dst_ip;
				r->src_port = src_port;
				r->dst_port = dst_port;
				r->count = 1;
				list_add(&(r->node), &head);
			};
		}
	}

	pfring_close(pd);
	return(0);
}
