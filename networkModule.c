#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <pthread.h>
#include <string.h>
#include <limits.h>
#include <errno.h>
#include <time.h>

#include "networkModule.h"
#include "timeModule.h"

#define PORT                    12345
#define UDP_MAX_SIZE            1500
#define BUFFER_SIZE             UDP_MAX_SIZE * 2

static int loop = 0;
static pthread_t networkThreadId;

/* Helper functions */
// static void NM_sendMessage(struct sockaddr_in sa, int fd, char *buf, size_t msg_size)
// {
// 	size_t sa_len = sizeof(sa);
// 	char udp_buf[UDP_MAX_SIZE] = {0};
// 	char *curr_pos = buf;
// 	char *start_pos = buf;

// 	while ((start_pos-buf) < msg_size) {
// 		(void)memset(udp_buf, '\0', sizeof(udp_buf));
// 		curr_pos += UDP_MAX_SIZE-1;

// 		// check if current buffer window is before the end
// 		// of the message (more data than udp buffer size)
// 		// segment messages by new line characters
// 		if ((curr_pos-buf) < msg_size) {
// 			// only the "get array" cmd should 
// 			while (*curr_pos != '\n')
// 				--curr_pos;
// 			(void)strncpy(udp_buf, start_pos, curr_pos-start_pos+1);
// 		} else {
// 			(void)strcpy(udp_buf, start_pos);
// 		}
// 		(void)sendto(fd, udp_buf, strlen(udp_buf), 0,
// 			(struct sockaddr *)&sa, sa_len);

// 		start_pos = curr_pos + 1;
// 	}
// }

static void *networkThread(void *arg)
{
	// Network variables
	int fd;
	struct sockaddr_in sa;
	char buf[BUFFER_SIZE] = {0};
	int bytes_recv;
	unsigned int sa_len;
	size_t msg_size;
	int numAlarms = 0;
	int alarmsFromNetwork[50];
	struct tm alarm;

	// Packet variables to store information for alarm 
	int day;
	int month;
	int year;
	int hour;
	int min;
	
	// Initialize socket
	fd = socket(PF_INET, SOCK_DGRAM, 0);
	if (fd < 0) {
		printf("Error: unable to get file descriptor from socket()\n");
		(void)fflush(stdout);
		return NULL;
	}

	memset(&sa, 0, sizeof(sa));
	sa.sin_family = AF_INET;
	sa.sin_port = htons(PORT);
	sa.sin_addr.s_addr = htonl(INADDR_ANY);

	if (bind(fd, (struct sockaddr *)&sa, sizeof(sa)) < 0) {
		printf("Error: unable to bind to port\n");
		(void)fflush(stdout);
		return NULL;
	}

	char *cmd;
	char *current;
	cmd = buf;
	current = cmd;

	while (loop) {
		sa_len = sizeof(sa);
		bytes_recv = recvfrom(fd, buf, BUFFER_SIZE-1, 0,
			(struct sockaddr *)&sa, &sa_len);

		if (bytes_recv < 0) {
			printf("Error: recvfrom encountered an error\n");
			(void)fflush(stdout);
			return NULL;
		}

		if (bytes_recv == 0)
			continue;

		buf[bytes_recv] = '\0';
		cmd = buf;
		current = cmd;
		memset(alarmsFromNetwork, 0, sizeof(alarmsFromNetwork));
		numAlarms = 0;

		printf("%s\n", buf);

		while (*current != '\0' && (current-buf) < BUFFER_SIZE) {
			//printf("*current = %c\n", *current);
			if ((*current == '\n') || (current-buf) >= BUFFER_SIZE || (*current == '\0')) {
				// Sets to a null when it comes to a new line, into its own string
				*current = '\0';
				printf("Command received: \"%s\"\n", cmd);
				(void)fflush(stdout);

				// Process command when it finds a command
				if (sscanf(cmd, "%d %d %d %d %d", &month, &day, &year, &hour, &min) && cmd) {
					// TODO: Set alarm using alarm setting module
					// TM_fillStructTM or something?
					printf("test, got: %d %d %d %d %d\n", month, day, year, hour, min);
					TM_fillStructTM(day, month, year, hour, min, &alarm);
					alarmsFromNetwork[numAlarms] = TM_tmtoi(&alarm);
					numAlarms++;

				}
				// Increment pointers
				++current;
				cmd = current;
			} else {
				++current;
			}
		}

		if (numAlarms) {
			TM_updateAlarmCache(alarmsFromNetwork, numAlarms);
		}
	}

	(void)close(fd);

	return NULL;
}

/* Public functions */
int NM_init(void)
{
	loop = 1;
	return pthread_create(&networkThreadId, NULL, networkThread, NULL);
}

void NM_cleanup(void)
{
	loop = 0;
	(void)pthread_join(networkThreadId, NULL);
}
