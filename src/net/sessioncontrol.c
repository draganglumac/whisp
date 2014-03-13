/*
 * =====================================================================================
 *
 *       Filename:  sessioncontrol.c
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  03/10/14 15:02:40
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jonesax (),
 *   Organization:
 *
 * =====================================================================================
 */
#include <stdlib.h>
#include "sessioncontrol.h"
#include <jnxc_headers/jnxbtree.h>
#include <jnxc_headers/jnxmem.h>
#include <jnxc_headers/jnxlog.h>
#include <uuid/uuid.h>
#include <openssl/rsa.h>
#include <openssl/pem.h>
static jnx_btree *session_tree = NULL;

char *session_generate_secret() {
    char *names[10] = {
        "Alpha","Bravo","Charlie","Delta",
        "Epsilon","Foxtrot","Gamma","Hugo",
        "Indy","Jupiter"
    };
    int r = (rand() % 10),
        l = (rand() % 10),
        n = (rand() % 10);
    char buffer[124];
    sprintf(buffer,
            "%s-%s-%s",names[r],names[l],names[n]);
    printf("SESSION SECRET: %s\n",buffer);
    return strdup(buffer);
}
char *session_generate_id() {
    uuid_t g;
    uuid_generate_random(g);
    int i;
    char buffer[24] = {};
    for (i = 0; i < 16; ++i) {
        char micro[sizeof(unsigned int)];
        sprintf(micro, "%u", g[i]);
        strcat(buffer, micro);
    }
    printf("SESSION ID: %s\n", buffer);
    assert(buffer);
    return strdup(buffer);
}
int session_tree_compare(void *a, void *b) {
    if(a > b) {
        return -1;
    }
    if(b > a) {
        return 1;
    }
    return 0;
}
state session_get_state(char *session_id) {
    if(!session_tree) {
        return SESSION_ERROR;
    }
    session *s = jnx_btree_lookup(session_tree,session_id);
    if(!s) {
        printf("Session not found\n");
        return SESSION_ERROR;
    }
    return s->current_state;
}
session* session_get_session(char *session_id) {
    if(!session_tree) {
        return NULL;
    }
    session *s = jnx_btree_lookup(session_tree,session_id);
    return s;
}
char * session_create(raw_peer *local, raw_peer *foriegn) {
    if(!session_tree) {
        session_tree = jnx_btree_create(sizeof(int),session_tree_compare);
    }
    session *s = JNX_MEM_MALLOC(sizeof(session));
    s->local_peer = local;
    s->foriegn_peer = foriegn;
    s->shared_secret = NULL;
	s->foriegn_public_key = NULL;
	s->local_keypair = NULL;
    s->current_state = SESSION_PRE_HANDSHAKE;
    s->session_id = session_generate_id();
    s->session_origin_guid = local->guid;
	jnx_btree_add(session_tree,s->session_id,s);
    return s->session_id;
}
void session_add(session *s) {
    if(!session_tree) {
        session_tree = jnx_btree_create(sizeof(int),session_tree_compare);
    }
    jnx_btree_add(session_tree,s->session_id,s);
}
void session_destroy(session *s) {
    assert(s);
	JNX_LOGC("Deleting session id...\n");
	free(s->session_id);
    if(s->shared_secret) {
		JNX_LOGC("Deleting session shared secret...\n");
        free(s->shared_secret);
    }
	if(s->foriegn_public_key) {
		JNX_LOGC("Deleting session foriegn public key...\n");
		free(s->foriegn_public_key);
	}
	if(s->local_keypair) {
		JNX_LOGC("Deleting session keypair...\n");
		RSA_free(s->local_keypair);
	}
	JNX_LOGC("Deleting session object...\n");
	JNX_MEM_FREE(s);
}
int session_check_exists(raw_peer *local, raw_peer *foriegn) {
    if(!session_tree) {
        return 0;
    }
    jnx_list *keys = jnx_list_create();
    jnx_btree_keys(session_tree,keys);

    while(keys->head) {

        session *s = jnx_btree_lookup(session_tree,keys->head->_data);
        if(s) {
            if(local == s->local_peer &&
                    foriegn == s->foriegn_peer) {
                jnx_list_destroy(&keys);
                return 1;
            }
        }
        keys->head = keys->head->next_node;
    }
    jnx_list_destroy(&keys);
    return 0;
}
