#include <linux/if_ether.h>
#include <sys/socket.h>
#include <netpacket/packet.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

#include "list.h"

struct list_head* record_list;

struct record {
	uint32_t src_ip;
	uint16_t src_port;
	uint32_t dst_ip;
	uint16_t dst_port;
	uint32_t count;
	struct list_head node;
};

uint32_t convert_ip(unsigned char *start)
{
	uint32_t result = 0;
	int i;
	for (i = 0; i < 4; i++) {
		result *= 256;
		result += *start;
		start++;
	};

	return result;
}

void break_handler(int s)
{
	struct record *r;
	FILE *fp = fopen("result.txt", "w");

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

int main(int argc, char *argv[])
{
	printf("Use Ctrl + C to kill this program. The results would be written to result.txt.\n");

	struct sigaction sig_handler;
	sigemptyset(&sig_handler.sa_mask);
	sig_handler.sa_handler = break_handler;
	sig_handler.sa_flags = 0;

	sigaction(SIGINT, &sig_handler, NULL);
	sigaction(SIGTERM, &sig_handler, NULL);
	sigaction(SIGQUIT, &sig_handler, NULL);

	unsigned char buf[2048] = {0};
	unsigned char *iphead;
	int n;
	LIST_HEAD(head);
	record_list = &head;

	int sd = socket(PF_PACKET, SOCK_RAW, htons(ETH_P_ALL));

	for (; ; ) {
		n = recvfrom(sd, buf, sizeof(buf), 0, NULL, NULL);

		if (n < 42) {
			printf("Incomplete packet");
			close(sd);
			exit(0);
		}

		iphead = buf + 14;
		uint32_t src_ip = convert_ip(&iphead[12]);
		uint32_t dst_ip = convert_ip(&iphead[16]);
		uint16_t src_port = (iphead[20] << 8) + iphead[21];
		uint16_t dst_port = (iphead[22] << 8) + iphead[23];

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
	};

	close(sd);
	return 0;
}
