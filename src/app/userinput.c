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
#include "peerstore.h"
#include <jnxc_headers/jnxterm.h>
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
			printf("Okay estblishing link with %s\n",l);
        }
    }

}




