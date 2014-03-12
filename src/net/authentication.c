/*
 * =====================================================================================
 *
 *       Filename:  authentication.c
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  03/10/14 18:38:13
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jonesax (),
 *   Organization:
 *
 * =====================================================================================
 */
#include <stdlib.h>
#include "authentication.h"
#include <jnxc_headers/jnxmem.h>
#include <jnxc_headers/jnxhash.h>
#include "peerstore.h"
#include "local_macro.h"
extern jnx_hashmap *configuration;

void authentication_update_foriegn_session(session *s) {
	jnx_socket *sec = jnx_socket_tcp_create(AF_INET);
    char *buffer;
	size_t len = serialize_session_data(&buffer,s);
  
  	if(strcmp(jnx_hash_get(configuration,"DEBUG"),"YES") == 0) {
        jnx_socket_tcp_send(sec,"localhost",s->foriegn_peer->port,buffer,len);
    } else {
        jnx_socket_tcp_send(sec,s->foriegn_peer->ip,s->foriegn_peer->port,buffer,len);
    }
    JNX_MEM_FREE(buffer);
    jnx_socket_destroy(&sec);
}
void authentication_start_session_from_incoming_convo(session *s) {
	
	//Recipiant gets here and reads the session command
}

void authentication_start_session_from_convo(session *s) {

	authentication_update_foriegn_session(s);

	///after telling our recipiant about the new session we'll wait for them to reply
}

