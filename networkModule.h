#ifndef _NETWORK_H_
#define _NETWORK_H_

#include <netinet/in.h>

/**
 * Network module - UDP server to listen to user requests
 */

/**
 * Initializes the networking module and thread
 * @return 0 if successful, otherwise error
 */
int NM_init(void);

/**
 * Cleans up the module and destroys the associated thread
 */
void NM_cleanup(void);


#endif
