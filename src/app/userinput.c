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

        if(connectioncontrol_isconnected()) {
			if(strcmp(line,"exit") == 0) { 
				connectioncontrol_stop();
			}
			///send everything down the pipe
            if(connectioncontrol_secure_message(line) != 0) {
                printf("secure_message error\n");
            }
        }
        if(strcmp(line,"list") == 0) {
        	peerstore_print_peers();
		}
        if(strcmp(line,"convo") == 0) {
            printf("Okay please select GUID of the peer to contact...\n");

	  		char *l;
            getline(&l,&n,stdin);
            trim(l);
		   	
			
			/* 
	  		char *l;
            size_t t;
            getline(&l,&n,stdin);
            trim(l);
            char *cl = strdup(l);

            raw_peer *found_peer;
          
		  	if(peerstore_check_peer(cl,&found_peer)) {
				
				assert(found_peer->guid);
				assert(found_peer->command);
				assert(found_peer->ip);
				assert(found_peer->port);
				assert(found_peer->secure_port);
                
				if(found_peer->has_public_key) {
                    printf("Has stored public key!\n");
                } else {
                    printf("No previous public key stored\n");
                }
                printf("======================================\n");
                //We offload from this thread so we can monitor progress
                ASYNC_START(connectioncontrol_start,found_peer);
                system("clear");
                printf("======================================\n");
                printf("Connected!\n");
                printf("======================================\n");
            }
            free(cl);

			*/
        }
    }
}













