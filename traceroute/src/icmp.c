/**
 * Wiktor Ogrodnik
 * 323129
 */

#include "icmp.h"
#include <assert.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

static u_int16_t compute_icmp_checksum(const void* buff, int length) {
	u_int32_t sum;
	const u_int16_t* ptr = buff;
	assert(length % 2 == 0);
	for (sum = 0; length > 0; length -= 2)
		sum += *ptr++;
	sum = (sum >> 16) + (sum & 0xffff);
	return (u_int16_t)(~(sum + (sum >> 16)));
}

static struct icmphdr* icmp_get_header(u_int8_t* buffer) {
	struct ip* ip_header = (struct ip*)buffer;
	size_t ip_header_len = sizeof(unsigned int) * ip_header->ip_hl;
	u_int8_t* icmp_packet = buffer + ip_header_len;
	return (struct icmphdr*)icmp_packet;
}

void icmp_send_packet(int socket, struct sockaddr_in* ip_addr, u_int8_t type, u_int8_t code, u_int16_t id, u_int16_t sequence) {

	struct icmp icmpheader;
	icmpheader.icmp_type = type;
	icmpheader.icmp_code = code;
	icmpheader.icmp_hun.ih_idseq.icd_id = htons(id);
	icmpheader.icmp_hun.ih_idseq.icd_seq = htons(sequence);
	icmpheader.icmp_cksum = 0;
	icmpheader.icmp_cksum = compute_icmp_checksum((u_int16_t*)&icmpheader, sizeof(struct icmp));

	int res = sendto(socket, &icmpheader, sizeof(icmpheader), MSG_DONTWAIT, (struct sockaddr*)ip_addr, sizeof(*ip_addr));

	if (res == -1) {
		fprintf(stderr, "sendto() error: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
	}
}

struct icmphdr* icmp_receive_packet(int socket, u_int8_t* const buffer, struct sockaddr_in* const sender, socklen_t* const sender_len) {
	
	int res = recvfrom(socket, buffer, IP_MAXPACKET, MSG_DONTWAIT, (struct sockaddr*)sender, sender_len);

	if (res == -1) {
		fprintf(stderr, "recvfrom() error: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
	}

	struct icmphdr* icmp_header = icmp_get_header(buffer);

	if (icmp_header->type == ICMP_TIME_EXCEEDED) {
		u_int8_t* new_buffer = (u_int8_t*)icmp_header + sizeof(struct icmphdr);
		icmp_header = icmp_get_header(new_buffer);
	}

	return icmp_header;
}