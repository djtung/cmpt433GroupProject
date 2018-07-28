#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include <string.h>
#include <limits.h>
#include <errno.h>

#include "networkModule.h"

#define PORT                    12345
#define UDP_MAX_SIZE            1500
#define BUFFER_SIZE             UDP_MAX_SIZE * 2

static int loop = 0;
static pthread_t networkThreadId;

/* Helper functions */
// static int NM_buildStatusMessage(char *buf)
// {
//         (void)memset(buf, 0, BUFFER_SIZE);
//         return snprintf(buf, BUFFER_SIZE-1, "mode=%d\nvolume=%d\nbpm=%d",
//                 audio_getDrumMode(), audio_getVolume(), audio_getBPM());
// }

static void NM_sendMessage(struct sockaddr_in sa, int fd, char *buf, size_t msg_size)
{
	size_t sa_len = sizeof(sa);
	char udp_buf[UDP_MAX_SIZE] = {0};
	char *curr_pos = buf;
	char *start_pos = buf;

	while ((start_pos-buf) < msg_size) {
		(void)memset(udp_buf, '\0', sizeof(udp_buf));
		curr_pos += UDP_MAX_SIZE-1;

		// check if current buffer window is before the end
		// of the message (more data than udp buffer size)
		// segment messages by new line characters
		if ((curr_pos-buf) < msg_size) {
			// only the "get array" cmd should 
			while (*curr_pos != '\n')
				--curr_pos;
			(void)strncpy(udp_buf, start_pos, curr_pos-start_pos+1);
		} else {
			(void)strcpy(udp_buf, start_pos);
		}
		(void)sendto(fd, udp_buf, strlen(udp_buf), 0,
			(struct sockaddr *)&sa, sa_len);

		start_pos = curr_pos + 1;
	}
}

static void *networkThread(void *arg)
{
	int fd;
	struct sockaddr_in sa;
	char buf[BUFFER_SIZE] = {0};
	int bytes_recv;
	unsigned int sa_len;
	size_t msg_size;

	// Packet information for alarm 
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

	char *cmd, *current;
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

		while (*current != '\0' && (current-buf) < BUFFER_SIZE) {
			if ((*current == "\n") || (current-buf) >= BUFFER_SIZE) {
				// Sets to a null when it comes to a new line, into its own string
				*current = '\0';
				printf("Command received: \"%s\"\n", cmd);
				(void)fflush(stdout);

				// Process command when it finds a command
				if (sscanf(buf, "alarm=%d,%d,%d,%d,%d,%d", &month, &day, &year, &hour, &minute)) {
					// Limiting day variable
					if (day < 1) {
						day = 1;
					} else if (day > 31) {
						day = 31;
					}

					// Limiting month variable
					if (month < 1) {
						month = 1;
					} else if (month > 12) {
						month = 12;
					}

					// Limiting year variable
					if (year < 1) year = 1;

					// Limiting hour variable
					if (hour < 0) {
						hour = 0;
					} else if (hour > 24) {
						hour = 24;
					}

					// Limiting minute variable
					if (minute < 0) {
						minute = 0;
					} else if (minute > 60) {
						minute = 60;
					}
				}

				// TODO: Set alarm using alarm setting module
				// TM_fillStructTM or something?

				// Increment pointers
				++current;
				cmd = current;
			} else {
				++current;
			}
		}
	}

	msg_size = 0;

	if (msg_size <= 0)
		continue;
	else
		sendMessage(sa, fd, buf, msg_size);

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
