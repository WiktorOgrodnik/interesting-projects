/**
 * Wiktor Ogrodnik
 * 323129
 */

#define _GNU_SOURCE

#ifndef ICMP_H
#define ICMP_H

#include <netinet/ip_icmp.h>

void icmp_send_packet(int socket, struct sockaddr_in* ip_addr, u_int8_t type, u_int8_t code, u_int16_t id, u_int16_t sequence);
struct icmphdr* icmp_receive_packet(int socket, u_int8_t* const buffer, struct sockaddr_in* const sender, socklen_t* const sender_len);

#endif