/**
 * Wiktor Ogrodnik
 * 323129
 */

#include "icmp.h"

#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/time.h>

#define PACKETS_IN_ROW 3
#define AWAIT_TIME 1

u_int16_t my_pid = 0;

enum select_status {
	SELECT_ERROR,
	SELECT_TIMEOUT,
	SELECT_SUCCESS
};

void print_ip(struct sockaddr_in* ip) {
	printf("%s ", inet_ntoa(ip->sin_addr));
}

int correct_icmp_packet(const struct icmphdr* icmp, int cnt) {

	if (ntohs(icmp->un.echo.id) == my_pid && 
		ntohs(icmp->un.echo.sequence) >= PACKETS_IN_ROW * cnt &&
		ntohs(icmp->un.echo.sequence) < PACKETS_IN_ROW * (cnt + 1)) {
			return ntohs(icmp->un.echo.sequence) % PACKETS_IN_ROW;
		}

	return -1;
}

void update_time(struct timeval** time, struct timeval* timeleft) {

	*time = (struct timeval*)malloc(sizeof(struct timeval));

	if (*time == NULL) {
		fprintf(stderr, "Can not allocate timeval!\n");
		exit(EXIT_FAILURE);
	}

	(*time)->tv_sec = AWAIT_TIME;
	(*time)->tv_usec = 0;
	timersub(*time, timeleft, *time);
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

int main(int argc, char** argv) {

	my_pid = getpid();
	
	bool always_print_times = false;
	bool input_flag = false;
	int opt;
	char ip_string[100];

	struct sockaddr_in ip_addr_s;
	bzero(&ip_addr_s, sizeof(ip_addr_s));
	ip_addr_s.sin_family = AF_INET;

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

	int res = inet_pton(AF_INET, ip_string, &ip_addr_s.sin_addr);

	if (res == 0) {
		fprintf(stderr, "%s: Name or service not known\n", ip_string);
		return EXIT_FAILURE;
	} else if (res == -1) {
		fprintf(stderr, "inet_pton() error: %s\n", strerror(errno));
        return EXIT_FAILURE;
	}

	bool route_found = false;

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
			icmp_send_packet(sockfd, &ip_addr_s, ICMP_ECHO, 0, my_pid, (i - 1) * PACKETS_IN_ROW + j);
		}

		struct timeval tv;
		tv.tv_sec = AWAIT_TIME;
		tv.tv_usec = 0;
		int received = 0;
		
		struct timeval* times[PACKETS_IN_ROW];

		for (int i = 0; i < PACKETS_IN_ROW; i++) times[i] = NULL;

		printf("%d. ", i);

		do {
			if (await_res(sockfd, &tv) == SELECT_TIMEOUT) {
				break;
			}

			struct icmphdr* icmp_header = icmp_receive_packet(sockfd, buffer, &sender, &sender_len);

			int packet_number = -1;
			if ((packet_number = correct_icmp_packet(icmp_header, i - 1)) != -1) {
				if (times[packet_number] == NULL) {
					print_ip(&sender);
					update_time(&times[packet_number], &tv);
					received++;
				}

				if (icmp_header->type == ICMP_ECHOREPLY) route_found = true;
			}
		} while (received < PACKETS_IN_ROW);

		if (received == 0) {
			printf("*\n");
		} else if (received == PACKETS_IN_ROW || (always_print_times && received > 0)) {
			u_int64_t total_microseconds = 0;
			for (int i = 0; i < PACKETS_IN_ROW; i++) {
				if (times[i] != NULL) {
					total_microseconds += times[i]->tv_usec;
				}
			}
			u_int64_t avarage = (total_microseconds / 1000) / received;

			printf("%ldms\n", avarage);
		} else {
			printf("???\n");
		}

		for (int i = 0; i < PACKETS_IN_ROW; i++)
			if (times[i] != NULL) free(times[i]);
	}

    return EXIT_SUCCESS;
}