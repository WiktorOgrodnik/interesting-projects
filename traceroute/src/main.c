/**
 * Wiktor Ogrodnik
 * 323129
 */

#define _GNU_SOURCE

#include <features.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/time.h>
#include <ctype.h>

#include "icmp.h"

#define PACKETS_IN_ROW 3
#define AWAIT_TIME 1

enum select_status {
	SELECT_ERROR,
	SELECT_TIMEOUT,
	SELECT_SUCCESS
};

void print_ip(u_int32_t data) {
	char ip[20];

	u_int8_t block1 = data % 256;
	u_int8_t block2 = (data >> 8) % 256; 
	u_int8_t block3 = (data >> 16) % 256;
	u_int8_t block4 = (data >> 24) % 256; 
	sprintf(ip, "%d.%d.%d.%d", block1, block2, block3, block4);

	printf("%s ", ip);
}

bool correct_icmp_packet(const struct icmphdr* icmp, int cnt) {
	return icmp->un.echo.id == getpid() && 
		icmp->un.echo.sequence >= PACKETS_IN_ROW * cnt &&
		icmp->un.echo.sequence < PACKETS_IN_ROW * (cnt + 1);
}

void update_time(struct timeval* time, struct timeval* timeleft) {
	time->tv_sec = AWAIT_TIME;
	time->tv_usec = 0;
	timersub(time, timeleft, time);
}

enum select_status await_res(int socket, struct timeval* tv) {
	fd_set ready_socket;
	FD_ZERO(&ready_socket);
	FD_SET(socket, &ready_socket);

	int rc = select(socket + 1, &ready_socket, NULL, NULL, tv);

	if (rc == -1) {
		fprintf(stderr, "select() failed");
		exit(EXIT_FAILURE);
	} else if (rc == 0) {
		return SELECT_TIMEOUT;
	} else {
		return SELECT_SUCCESS;
	}
}

bool validate_ip(const char* const ip_string, u_int16_t ipblocks[]) {

	u_int16_t digits = 0, dots = 0;
	size_t len = strlen(ip_string);
	char last_sign = '\0';

	if (ip_string[0] == '.')
		return false;

	for (size_t i = 0; i < len; i++) {
		if (!isdigit(ip_string[i]) && ip_string[i] != '.')
			return false;

		if (isdigit(ip_string[i]))
			digits++;

		if (ip_string[i] == '.')
			dots++;

		if (i > 0 && ip_string[i - 1] == '.' && ip_string[i] == '.')
			return false;

		last_sign = ip_string[i];
	}

	if (last_sign == '.' || digits < 4 || dots != 3)
		return false;

	for (int i = 0; i < 4; i++) {
		if (ipblocks[i] > 255) {
			return false;
		}
	}

	return true;
}

int main(int argc, char** argv) {
	
	bool always_print_times = false;
	bool input_flag = false;
	u_int16_t ipblocks[4];
	int opt;
	char ip_string[100];

	while ((opt = getopt(argc, argv, "de:h")) != -1) {
		if (opt == 'd') always_print_times = true;
		else if (opt == 'e') {
			input_flag = true;
			memcpy(ip_string, optarg, strlen(optarg));
		} else if (opt == 'h') {
			printf("Usage: tracerout [options] [target] ...\n");
            printf("Options:\n");
            printf("-e STRING           Load ip address.\n");
            printf("-d                  Print delays for every row instead of ???.\n");
            printf("-h                  Help menu.\n");
			return EXIT_SUCCESS;
		}
	}

	if (!input_flag && argc > 1 && strlen(argv[1]) > 2) {
		input_flag = true;
		memcpy(ip_string, argv[1], strlen(argv[1]));
	}

	if (!input_flag) {
		printf("Type ip address: ");
		int e = scanf("%99s", ip_string);
		if (!e) {
			fprintf(stderr, "Error while parsing data!\n");
			return EXIT_FAILURE;
		}
	}

	sscanf(ip_string, "%hu.%hu.%hu.%hu", ipblocks, ipblocks + 1, ipblocks + 2, ipblocks + 3);

	if (!validate_ip(ip_string, ipblocks)) {
		fprintf(stderr, "%s: Name or service not known\n", ip_string);
		return EXIT_FAILURE;
	}

	bool route_found = false;
	u_int32_t ip_addr = ipblocks[0] | (ipblocks[1] << 8) | (ipblocks[2] << 16) | (ipblocks[3] << 24);

    int sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    if (sockfd < 0) {
        fprintf(stderr, "The problem with socket appeard: %s\n", strerror(errno));
        return EXIT_FAILURE;
    }

    for (int i = 1; i <= 30 && !route_found; i++) {

		struct sockaddr_in sender;	
		socklen_t sender_len = sizeof(sender);
		u_int8_t buffer[IP_MAXPACKET];

		setsockopt(sockfd, IPPROTO_IP, IP_TTL, &i, sizeof(int));

		for (int j = 0; j < PACKETS_IN_ROW; j++) {
			icmp_send_packet(sockfd, ip_string, ICMP_ECHO, 0, getpid(), (i - 1) * PACKETS_IN_ROW + j);
		}

		struct timeval tv;
		tv.tv_sec = AWAIT_TIME;
		tv.tv_usec = 0;
		int received = 0;
		
		struct timeval times[PACKETS_IN_ROW];

		printf("%d. ", i);

		do {
			if (await_res(sockfd, &tv) == SELECT_TIMEOUT) {
				break;
			}

			struct icmphdr* icmp_header = icmp_receive_packet(sockfd, buffer, &sender, &sender_len);

			if (icmp_header->type == ICMP_TIME_EXCEEDED) {
				u_int8_t* new_buffer = (u_int8_t*)icmp_header + sizeof(struct icmphdr);
				icmp_header = icmp_get_header(new_buffer);
			}

			if (correct_icmp_packet(icmp_header, i - 1)) {
				print_ip(sender.sin_addr.s_addr);
				update_time(&times[received++], &tv);

				if (ip_addr == sender.sin_addr.s_addr) route_found = true;
			}
		} while (received < PACKETS_IN_ROW);

		if (received == 0) {
			printf("*\n");
		} else if (received == PACKETS_IN_ROW || (always_print_times && received > 0)) {
			u_int64_t total_microseconds = 0;
			for (int i = 0; i < received; i++) {
				total_microseconds += times[i].tv_usec;
			}
			u_int64_t avarage = (total_microseconds / 1000) / received;

			printf("%ldms\n", avarage);
		} else {
			printf("???\n");
		}
	}

    return EXIT_SUCCESS;
}