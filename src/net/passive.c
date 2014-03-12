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

/*
 * The purpose here is to provide another entry point for a foriegn peer
 * to establish a session on both ends
 * This is the foriegn peer route to authentication
 */
int passive_listener_callback(char *msg, size_t msg_len, char *ip) {

    printf("Passive listener intercept - %s\n",msg);
    raw_peer *rp = NULL;
    S_TYPES ret = deserialize_data(&rp,msg,msg_len,ip);
    switch(ret) {
    case S_MALFORMED:
        JNX_LOGC("malformed deserialization data...\n");
        break;
    case S_GENERAL_ERROR:
        JNX_LOGC("general error deserializing...\n");
        break;
    case S_UNKNOWN:
        JNX_LOGC("unknown error deserializing...\n");
        break;
    case S_OKAY:
        assert(rp);
        assert(rp->guid);
        assert(rp->command);
        assert(rp->ip);
        assert(rp->port);
        raw_peer *foriegn_peer;

        if(peerstore_check_peer(rp->guid,&foriegn_peer)) {
            peerstore_update_peer(rp,foriegn_peer);
            printf("Incoming known foriegn peer request for %s from %s\n",foriegn_peer->command, foriegn_peer->guid);

            raw_peer *local_peer;
            if(peerstore_check_peer(jnx_hash_get(configuration,"GUID"),&local_peer)) {

                if(!session_check_exists(local_peer,foriegn_peer)) {
                    printf("New local session to match foriegn create session\n");
                    char *session_handle  = session_create(local_peer,foriegn_peer);
                    session *s = session_get_session(session_handle);
                    assert(s->local_peer == local_peer);
                    assert(s->foriegn_peer == foriegn_peer);
                    ///authentication entry point [ BLOCKING LISTENER FROM FURTHER AUTHS ]
                    authentication_start_session_from_incoming_convo(s);
                }

            } else {
                peerstore_delete_peer(rp);
            }
        }
    }
    return 0;
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

}
