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
void user_interact_session(char *session_id) {
    session *s;
    int r = session_get_session(session_id,&s);
    if(r) {
        jnx_term_printf_in_color(JNX_COL_CYAN,"Found session.\n");
        while(1) {

			if(s->current_state != SESSION_CONNECTED) {
				printf("Exiting from session mode\n");
				return;
			}
			jnx_term_color(JNX_COL_YELLOW);
            char *line;
            size_t n;
            if(getline(&line,&n,stdin) == -1) {
            }
			jnx_term_default();
            trim(line);
        	char *outmsg = JNX_MEM_MALLOC(sizeof(char) * strlen(line) +1);
			bzero(outmsg,strlen(line) +1);
			memcpy(outmsg,line,strlen(line));
			secure_channel_send(s,outmsg,strlen(line) +1);
			JNX_MEM_FREE(outmsg);
		}

    } else {
        jnx_term_printf_in_color(JNX_COL_RED,"Unable to find session.\n");
    }

}
void user_start_session(char *foriegn_peer_guid) {

    raw_peer *local_peer;
    if(peerstore_check_peer(jnx_hash_get(configuration,"GUID"),&local_peer)) {
        raw_peer *foriegn_peer;
        if(peerstore_check_peer(foriegn_peer_guid,&foriegn_peer)) {

            if(!session_check_exists(local_peer,foriegn_peer)) {
                char *session_handle  = session_create(local_peer,foriegn_peer);
                JNX_LOGF(JLOG_NORMAL,"CREATING NEW SESSION %s\n",session_handle);
                session *s;
                int ret = session_get_session(session_handle,&s);
                assert(ret == 1);
                assert(s->local_peer == local_peer);
                assert(s->foriegn_peer == foriegn_peer);

                //AUTHENTICATION ENTRY POINT [BLOCKING UI FROM FURTHER AUTHS]
                authentication_session_update(s);

            } else {
                JNX_LOGF(JLOG_NORMAL,"An existing session was found!\n");
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
        if(strcmp(line,"send") == 0) {
            printf("Please enter the session GUID to establish messaging with...\n");
            char *t;
            size_t b;
            getline(&t,&b,stdin);
            if(n == 0) {
                continue;
            }
            char *session_id = strndup(t,n);
            trim(session_id);

            user_interact_session(session_id);
            free(session_id);
        }
    }
}



