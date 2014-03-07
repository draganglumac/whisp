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
#include "connectioncontrol.h"
#include <jnxc_headers/jnxmem.h>
#include <jnxc_headers/jnxterm.h>
#include <jnxc_headers/jnxlog.h>
static int isConnecting = 0;
void trim(char *line) {
    size_t ln = strlen(line) -1;
    if(line[ln] == '\n' || line[ln] == '\r') {
        line[ln] = '\0';
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
            printf("Fetching list...\n");
            printf("%s\n",peerstore_get_peerstring());
        }
        if(strcmp(line,"convo") == 0) {
            printf("Okay please select GUID of the peer to contact...\n");
            char *l;
            size_t t;
            getline(&l,&n,stdin);
            trim(l);
            char *cl = strdup(l);

            raw_peer *found_peer;
            if(peerstore_check_peer(cl,&found_peer)) {

                printf("==========FOUND STORED PEER===========\n");
                printf("GUID -> %s\n",found_peer->guid);
                printf("COMMAND -> %s\n",found_peer->command);
                printf("IP -> %s\n",found_peer->ip);
                printf("PEERAGE -> %s\n",found_peer->peerstring);
                if(found_peer->has_public_key) {
                    printf("Has stored public key!\n");
                } else {
                    printf("No previous public key stored\n");
                }
                printf("======================================\n");
				//We offload from this thread so we can monitor progress
                ASYNC_START(connectioncontrol_start,found_peer);
                if(connectioncontrol_isconnected() == 0) {
                    jnx_term_load_spinner(1);
                    while(connectioncontrol_isconnected() == 0) {
                    }
                    jnx_term_load_spinner(0);
                    printf("Connected!\n");


                    while(connectioncontrol_isconnected()) {

                    }
                }
            }
            free(cl);
        }
    }
}












