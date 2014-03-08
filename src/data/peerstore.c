/*
 * =====================================================================================
 *
 *       Filename:  peerstore.c
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  03/07/14 08:03:28
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jonesax (),
 *   Organization:
 *
 * =====================================================================================
 */
#include <stdlib.h>
#include "peerstore.h"
#include <jnxc_headers/jnxlist.h>
#include <unistd.h>
#include <string.h>

static jnx_list *store = NULL;

int peerstore_check_peer(char *guid,raw_peer **inpeer) {
    if(!store) {
        return 0;
    }

    jnx_node *localhead = store->head;
    jnx_node *rewind = store->head;
    int found = 0;
    while(localhead) {
        raw_peer *rp = localhead->_data;
        if(strcmp(rp->guid,guid) == 0) {
            found = 1;
            *inpeer = rp;
            break;
        }
        localhead = localhead->next_node;
    }
    store->head = rewind;
    return found;
}
char* peerstore_get_peerstring() {
    if(!store) {
        return "NULL";
    }

    jnx_node *localhead = store->head;
    jnx_node *rewind = store->head;
    char buffer[2048];
    bzero(buffer,2048);
    strcpy(buffer,"");
    while(localhead) {

        raw_peer *rp = localhead->_data;
        strcat(buffer,rp->guid);
        if(localhead->next_node) {
            strcat(buffer,",");
        }
        localhead = localhead->next_node;
    }
    store->head = rewind;
    return strdup(buffer);
}
int peerstore_add_peer(raw_peer *rp) {
    if(!store) {
        store = jnx_list_create();
    }
    jnx_list_add(store,rp);
    return 0;
}

