/*
 * =====================================================================================
 *
 *       Filename:  ipc.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  04/14/14 08:34:51
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jonesax (), 
 *   Organization:  
 *
 * =====================================================================================
 */
#include <stdlib.h>
#include <stdio.h>
#include <jnxc_headers/jnxlog.h>
#include "ipc.h"
static jnx_hashmap *configuration;

int ipc_incoming_comms(uint8_t *out, size_t len, jnx_unix_socket *s) {

	printf("Received incoming --> %s\n",out);

	return 0;
}
void ipc_setup(jnx_hashmap *config) {

	configuration = config;
	JNX_LOG(DEFAULT_CONTEXT,"Starting Interprocess communications...\n");
	jnx_unix_socket *inc_sock = jnx_unix_datagram_socket_create(jnx_hash_get(configuration,"IPC_LISTEN"));
	jnx_hash_put(configuration,"IPC_LISTEN_SOCK",inc_sock);
	jnx_unix_datagram_socket_listen(inc_sock,ipc_incoming_comms);

}
void ipc_teardown() {

	jnx_unix_socket *s = jnx_hash_get(configuration,"IPC_LISTEN_SOCK");
	jnx_unix_socket_destroy(&s);
}
