/*
 * =====================================================================================
 *
 *       Filename:  userinput.c
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  03/06/14 16:39:35
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
#include <string.h>
#include "discovery.h"
#include "peerstore.h"
#include "sessioncontrol.h"
#include <jnxc_headers/jnxmem.h>
#include <jnxc_headers/jnxterm.h>
#include <jnxc_headers/jnxlog.h>
extern jnx_hashmap *configuration;
static int isConnecting = 0;
void trim(char *line) {
    size_t ln = strlen(line) -1;
    if(line[ln] == '\n' || line[ln] == '\r') {
        line[ln] = '\0';
    }
}
void user_start_session(char *foriegn_peer_guid) {

    raw_peer *local_peer;
    if(peerstore_check_peer(jnx_hash_get(configuration,"GUID"),&local_peer)) {
        raw_peer *foriegn_peer;
        if(peerstore_check_peer(foriegn_peer_guid,&foriegn_peer)) {

            if(!session_check_exists(local_peer,foriegn_peer)) {
                printf("Creating new session\n");
                char *session_handle  = session_create(local_peer,foriegn_peer);
                session *s = session_get_session(session_handle);
                assert(s->local_peer == local_peer);
                assert(s->foriegn_peer == foriegn_peer);

				//AUTHENTICATION ENTRY POINT [BLOCKING UI FROM FURTHER AUTHS]
				authentication_start_session_from_convo(s);

            } else {
                printf("An existing session was found!\n");
            }
        }
    }
}
void user_input_loop() {

    while (1) {
        char *line;
        size_t n;
        if(getline(&line,&n,stdin) == -1) {
        }
        trim(line);

        if(strcmp(line,"list") == 0) {
            peerstore_print_peers();
        }

        if(strcmp(line,"convo") == 0) {
            printf("Okay please select GUID of the peer to contact...\n");

            char *l;
            getline(&l,&n,stdin);
            trim(l);

            user_start_session(strdup(l));
        }
    }
}
