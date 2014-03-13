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
int passive_listener_callback(char *msg, size_t msg_len, char *ip) {

    //deserialise session
    session *new_session;
    S_TYPES ret = deserialize_session_data(&new_session,msg,msg_len);
    free(msg);
    if(ret != S_OKAY) {
        printf("Deserialization error from passive listener\n");
        return is_not_exiting;
    }

    printf("Incoming session ==========\n");
    printf("SESSION ID:%s\n",new_session->session_id);
    printf("SESSION ORIGIN:%s\n",new_session->session_origin_guid);
    printf("SESSION SHARED SECRET:%s\n",new_session->shared_secret);
    printf("SESSION LOCAL PEER:%s\n",new_session->local_peer->guid);
    printf("SESSION FORIEGN PEER:%s\n",new_session->foriegn_peer->guid);
    printf("SESSION STATE:%d\n",new_session->current_state);
	printf("LOCAL(Possibly remote)PUBLIC KEY: %s\n",new_session->local_public_key);
	printf("FORIEGN(Possibly local)PUBLIC KEY: %s\n",new_session->foriegn_public_key);
	printf("===========================\n");

    ///Careful with replication!!!!!!!!!!!
    if(!session_check_exists_by_id(new_session->session_id)) {
		JNX_LOGC("[%s]SESSION DOES NOT EXIST CREATING - %d ACTIVE SESSSIONS...\n",
				new_session->session_id,session_count());
            session_add(new_session);
    } else {
		JNX_LOGC("Finding old session...\n");
		session *old_session = session_get_session(new_session->session_id);

		RSA *lkey = old_session->local_keypair;
		assert(lkey);	
		
		JNX_LOGC("Deleting old session...\n");	
		session_destroy(old_session);
		session_add(new_session);
			
		new_session->local_keypair = lkey;
	}
    authentication_start_with_session(new_session);


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

