#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include <string.h>
#include <limits.h>
#include <errno.h>

#include "network.h"
#include "audio.h"

#define PORT                    12345
#define UDP_MAX_SIZE            1500
#define BUFFER_SIZE             UDP_MAX_SIZE * 2

static int loop = 0;
static pthread_t th;

/* Helper functions */
static int buildStatusMessage(char *buf)
{
        (void)memset(buf, 0, BUFFER_SIZE);
        return snprintf(buf, BUFFER_SIZE-1, "mode=%d\nvolume=%d\nbpm=%d",
                audio_getDrumMode(), audio_getVolume(), audio_getBPM());
}

static void sendMessage(struct sockaddr_in sa, int fd, char *buf, size_t msg_size)
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

static void *mainLoop(void *arg)
{
        int fd;
        struct sockaddr_in sa;
        char buf[BUFFER_SIZE] = {0};
        int bytes_recv;
        unsigned int sa_len;
        int val;
        size_t msg_size;

        // initialize socket
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

                printf("Command received: \"%s\"\n", buf);
                (void)fflush(stdout);

                // command processing here
                msg_size = 0;
                if (sscanf(buf, "mode=%d", &val)) {
                        if (val < AUDIO_MODE_NONE || val >= AUDIO_MODE_TOTAL)
                                val = AUDIO_MODE_NONE;
                        audio_setDrumMode(val);
                        msg_size = buildStatusMessage(buf);
                } else if (sscanf(buf, "volume=%d", &val)) {
                        val = val ? AUDIO_VOLUME_DIFF : -AUDIO_VOLUME_DIFF;
                        audio_setVolume(audio_getVolume() + val);
                        msg_size = buildStatusMessage(buf);
                } else if (sscanf(buf, "bpm=%d", &val)) {
                        val = val ? AUDIO_BPM_DIFF : -AUDIO_BPM_DIFF;
                        audio_setBPM(audio_getBPM() + val);
                        msg_size = buildStatusMessage(buf);
                } else if (sscanf(buf, "play=%d", &val)) {
                        if (val >= AUDIO_SOUND_FIRST && val <= AUDIO_SOUND_LAST) {
                                audio_queueSound(val);
                                msg_size = buildStatusMessage(buf);
                        } else {
                                printf("Warning: invalid sound requested to be played\n");
                                (void)fflush(stdout);
                        }
                } else if (!strcmp(buf, "poll")) {
                        // return system status to be displayed to web app
                        msg_size = buildStatusMessage(buf);
                }

                if (msg_size <= 0)
                        continue;
                else
                        sendMessage(sa, fd, buf, msg_size);

        }

	(void)close(fd);

        return NULL;
}

/* Public functions */
int network_init(void)
{
        loop = 1;
        return pthread_create(&th, NULL, mainLoop, NULL);
}

void network_cleanup(void)
{
        loop = 0;
        (void)pthread_join(th, NULL);
}
