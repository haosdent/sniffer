#include <linux/if_ether.h>
#include <sys/socket.h>
#include <netpacket/packet.h>
#include <arpa/inet.h>

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









