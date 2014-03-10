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
#include <jnxc_headers/jnxlog.h>
#include <jnxc_headers/jnxthread.h>
#include <unistd.h>
#include <string.h>
static jnx_thread_mutex store_lock;
static jnx_btree *store = NULL;

int peer_tree_compare(void *A, void *B);

size_t peerstore_get_count() {
    int count = 0;
    if(!store) {
        return count;
    }
    jnx_list *keys = jnx_list_create();
    jnx_thread_lock(&store_lock);
    jnx_btree_keys(store,keys);
    count = keys->counter;
    jnx_thread_unlock(&store_lock);
    jnx_list_destroy(&keys);
    return count;
}
int peerstore_check_peer(char *guid,raw_peer **inpeer) {
    if(!store) {
        store = jnx_btree_create(sizeof(int),peer_tree_compare);
        return 0;
    }
    int found = 0;
    jnx_list *keys = jnx_list_create();
    jnx_thread_lock(&store_lock);
    jnx_btree_keys(store,keys);
    while(keys->head) {
        raw_peer *rp= jnx_btree_lookup(store,keys->head->_data);
        if(rp) {
            if(strcmp(rp->guid,guid) == 0) {
                found = 1;
                jnx_list_destroy(&keys);
                *inpeer = rp;
                jnx_thread_unlock(&store_lock);
                return found;
            }
        }
        keys->head = keys->head->next_node;
    }
    jnx_list_destroy(&keys);
    *inpeer = NULL;
    jnx_thread_unlock(&store_lock);
    return found;
}
void peerstore_print_peers() {
    if(!store) {
        return;
    }

    jnx_list *keys = jnx_list_create();
    jnx_thread_lock(&store_lock);
    jnx_btree_keys(store,keys);
    int count = 0;
    while(keys->head) {

        raw_peer *rp= jnx_btree_lookup(store,keys->head->_data);
        if(rp) {
            printf("%d)%s\n",count,rp->guid);
        }
        ++count;
        keys->head = keys->head->next_node;
    }
    jnx_list_destroy(&keys);
    jnx_thread_unlock(&store_lock);
}
int peer_tree_compare(void *A, void *B) {
    if(A > B) {
        return -1;
    }
    if(B > A) {
        return 1;
    }

    return 0;
}
int peerstore_add_peer(raw_peer *rp) {
    if(!store) {
        store = jnx_btree_create(sizeof(int),peer_tree_compare);
    }
    jnx_thread_lock(&store_lock);
    jnx_btree_add(store,rp->guid,rp);
    printf("%s has come online!\n",rp->guid);
    jnx_thread_unlock(&store_lock);
    return 0;
}
