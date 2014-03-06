/*
 * =====================================================================================
 *
 *       Filename:  peerstore.c
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  03/04/14 18:31:23
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
#include <jnxc_headers/jnxbtree.h>
#include <jnxc_headers/jnxlog.h>
#include "peerstore.h"

typedef struct peerstore {
    jnx_btree *peer_tree;
    int count;
} peerstore;

static peerstore *local_peerstore = NULL;

int hash_peer(char *peer, int store_size) {
    assert(peer != 0 && peer[0] != 0);
    int i;
    unsigned h = peer[0];
    for(i=1; peer[i]!=0; ++i) {
        h=(h<<4)+peer[i];
    }
    return h % store_size;
}
int evaluate(void *A, void *B) {
    if(A > B) {
        return -1;
    }
    if(B > A) {
        return 1;
    }

    return 0;
}

void initialise_store() {
    if(!local_peerstore) {
        local_peerstore = JNX_MEM_MALLOC(sizeof(local_peerstore));
        local_peerstore->peer_tree = jnx_btree_create(6,evaluate);
        local_peerstore->count = 0;
    }
}
void deinitialise_store() {
    if(!local_peerstore) return;

    //:TODO
    ///get all keys
    ///free all raw_peers
    jnx_btree_destroy(local_peerstore->peer_tree);

}
int peerstore_check_peer(char *guid) {
    JNX_LOGF("Checking for peer %s\n",guid);
    if(!local_peerstore) return 0;
    int found = 0;
    jnx_list *keylist = jnx_list_create();
    jnx_btree_keys(local_peerstore->peer_tree,keylist);

    jnx_node *reset = keylist->head;
    while(keylist->head) {
        char *current_key = keylist->head->_data;
        if(strcmp(guid,current_key) == 0) {
            found = 1;
            JNX_LOGF("FOUND %s already in peerstore\n",guid);
            break;
        }
        keylist->head = keylist->head->next_node;
    }

    keylist->head = reset;
    jnx_list_destroy(&keylist);
    JNX_LOGF("Peer found %d\n", found  );
    return found;
}
char* peerstore_get_peerstring() {
    if(!local_peerstore) return "None";
    if(local_peerstore->count == 0) {
        return "None";
    }
    jnx_list *keylist = jnx_list_create();
    jnx_btree_keys(local_peerstore->peer_tree,keylist);

    jnx_node *reset = keylist->head;

    char buffer[2048];
    bzero(buffer,2048);
    while(keylist->head) {
        char *current_key = keylist->head->_data;
        strncat(buffer,current_key,strlen(current_key));
        if(keylist->head->next_node) {
            strncat(buffer,",",1);
        }
        keylist->head = keylist->head->next_node;
    }
    reset = keylist->head;
    jnx_list_destroy(&keylist);

    return strdup(buffer);
}

int peerstore_add_peer(raw_peer *rp) {
    if(!local_peerstore) {
        initialise_store();
    }
    JNX_LOGF("Adding peer %s\n",rp->guid);
    jnx_btree_add(local_peerstore->peer_tree,rp->guid,rp->ip);
    local_peerstore->count++;
}
int peerstore_remove_peer(char *guid) {
    if(!local_peerstore) return -1;


}

int peerstore_update_peer(char *guid) {
    if(!local_peerstore) return -1;


}






