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
#include <jnxc_headers/jnxlog.h>
#include <jnxc_headers/jnxsocket.h>
#include <jnxc_headers/jnxcheck.h>
#include "encryption.h"
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
int passive_listener_callback(uint8_t *msg, size_t msg_len, jnx_socket *s) {

    //deserialise session
    session *new_session;
    S_TYPES ret = deserialize_session_data(&new_session,msg,msg_len);
    free(msg);
    if(ret != S_OKAY) {
        JNX_LOG(DEFAULT_CONTEXT,"Deserialization error from passive listener\n");
        return is_not_exiting;
    }
    JNX_LOG(DEFAULT_CONTEXT,"===========Incoming session [%s] ==========\n",new_session->session_id);
    
	if(!session_check_exists_by_id(new_session->session_id)) {
		JNX_LOG(DEFAULT_CONTEXT,">>>>>>>>>>>SESSION DOES NOT EXIST CREATING - %d ACTIVE SESSIONS...\n",session_count());
			session_add(new_session);
    } else {


		session *old_session;
		int ret = session_get_session(new_session->session_id,&old_session);
		if(!ret) {
			JNX_LOG(DEFAULT_CONTEXT,"Error getting old session :-(\n");
			return 0;
		}
		JNXCHECK(old_session->session_origin_guid);
		JNXCHECK(old_session->session_id);

		if(old_session->local_keypair) {
			new_session->local_keypair = old_session->local_keypair;
		}
		session_destroy(old_session);
		/*
		 * Nothing is retained from the old session, however the local_keypair 
		 * is transferred to the local new session as it is not held or transmitted 
		 * anywhere else
		 */	
		session_add(new_session);	
		
	}
    authentication_session_update(new_session);

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
    JNXCHECK(tport);
    JNX_LOG(DEFAULT_CONTEXT,"Enabling passive TCP listener...\n");
    passive_tcp_socket = jnx_socket_tcp_create(AF_INET);
    ASYNC_START(passive_listener_start,tport);
}
void passive_listener_stop() {
    jnx_socket_destroy(&passive_tcp_socket);
    is_not_exiting = 1;
}

