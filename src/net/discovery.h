/*
 * =====================================================================================
 *
 *       Filename:  discovery.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  03/03/14 18:06:58
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jonesax (), 
 *   Organization:  
 *
 * =====================================================================================
 */

#include <jnxc_headers/jnxnetwork.h>
#include <jnxc_headers/jnxsocket.h>
#include <jnxc_headers/jnxhash.h>
#include "peerstore.h"

typedef struct thread_data {
    char* port;
    jnx_socket *s;
    char *bgroup;
    //multicasting information
    size_t len;
    char *ip;
    char *msg;
} thread_data;

void discovery_setup(jnx_hashmap *configuration);
void *discovery_start(void *args);
void discovery_teardown();
