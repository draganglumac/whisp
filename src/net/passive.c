/*
 * =====================================================================================
 *
 *       Filename:  passive.c
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  03/11/14 07:21:12
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jonesax (),
 *   Organization:
 *
 * =====================================================================================
 */
#include <stdlib.h>
#include <jnxc_headers/jnxmem.h>
#include <jnxc_headers/jnxlog.h>
#include <jnxc_headers/jnxsocket.h>
#include "passive.h"
#include "serialization.h"
#include "local_macro.h"
extern jnx_hashmap *configuration;
static jnx_socket *passive_tcp_socket;
static int is_not_exiting = 0;
/*
 * The purpose here is to provide another entry point for a foriegn peer
 * to establish a session on both ends
 * This is the foriegn peer route to authentication
 */
int passive_listener_callback(char *msg, size_t msg_len, char *ip) {

   	//deserialise session 
	session *s;
	S_TYPES ret = deserialize_session_data(&s,msg,msg_len);
	free(msg);
	if(ret != S_OKAY) {
		printf("Deserialization error from passive listener\n");
		return is_not_exiting;
	}	
	printf("Incoming session ==========\n");
	printf("SESSION ID:%s\n",s->session_id);
	printf("SESSION SHARED SECRET:%s\n",s->shared_secret);
	printf("SESSION LOCAL PEER:%s\n",s->local_peer->guid);
	printf("SESSION FORIEGN PEER:%s\n",s->foriegn_peer->guid);
	printf("SESSION STATE:%d\n",s->current_state);
	printf("===========================\n");
	///push this session object into the authentication

	if(!session_check_exists(s->local_peer,s->foriegn_peer)) {
		printf("Adding new session...\n");
		session_add(s);
	} else {

		///TODO: SESSION UPDATE
	}

	authentication_start_with_session(s);


    return is_not_exiting;
}
void* passive_listener_start(void *args) {

    char *port = args;
    jnx_socket_tcp_listen(passive_tcp_socket,port,
                          10,passive_listener_callback);
    return 0;
}
void passive_listener_setup(jnx_hashmap *configuration) {
    char *tport = jnx_hash_get(configuration,"TPORT");
    assert(tport);
    printf("Enabling passive TCP listener...\n");
    passive_tcp_socket = jnx_socket_tcp_create(AF_INET);
    ASYNC_START(passive_listener_start,tport);
}
void passive_listener_stop() {
	jnx_socket_destroy(&passive_tcp_socket);
	is_not_exiting = 1;	
}
