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
            if(peerstore_check_peer(cl)) {
                printf("Connecting to %s\n",cl);
                char *public_key_str;
                char *cldup = strdup(cl);
                int ret = peerstore_get_value(cldup,&public_key_str);
                int has_hand_shaken = 0;
                if(ret != 0) {
                    printf("No previous handshake data...\n");
                } else {
                    has_hand_shaken = 1;
                }
                thread_data *thr = JNX_MEM_MALLOC(sizeof(thread_data));
                thr->previous_hand_shake = has_hand_shaken;
                thr->public_key_str = public_key_str;

                ASYNC_START(connectioncontrol_start,thr);
                free(cldup);
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
        }
    }
}










