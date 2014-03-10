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
#include <jnxc_headers/jnxthread.h>
#include <unistd.h>
#include <string.h>
static jnx_thread_mutex store_lock;
static jnx_btree *store = NULL;

int peerstore_check_peer(char *guid,raw_peer **inpeer) {
    if(!store) {
        return 0;
    }
    int found = 0;
	jnx_list *keys = jnx_list_create();
	jnx_thread_lock(&store_lock);
	jnx_btree_keys(store,keys);	
	while(keys->head) {
		
		raw_peer *rp= jnx_btree_lookup(store,keys->head->_data);
		if(rp) {
			found = 1;
			jnx_list_destroy(&keys);
			*inpeer = rp;
			jnx_thread_unlock(&store_lock);
			return found;
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
