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
#include <jnxc_headers/jnxlog.h>
#include <uuid/uuid.h>
#include <jnxc_headers/jnxcheck.h>
#include <string.h>
#include <openssl/rsa.h>
#include <openssl/pem.h>
static jnx_btree *session_tree = NULL;

char *session_generate_secret() {
    char *names[10] = {
        "Alpha","Beta","Charlie","Delta",
        "Epsilon","Foxtrot","Gamma","Hugo",
        "Ipswitch","Jupiter"
    };
    int r = (rand() % 10),
        l = (rand() % 10),
        n = (rand() % 10);
    char buffer[24];
    sprintf(buffer,
            "%s-%s-%s",names[r],names[l],names[n]);
    JNX_LOG(DEFAULT_CONTEXT,"SESSION SECRET: %s\n",buffer);
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
    JNX_LOG(DEFAULT_CONTEXT,"SESSION ID: %s\n", buffer);
    JNXCHECK(buffer);
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
        JNX_LOG(DEFAULT_CONTEXT,"Session not found\n");
        return SESSION_ERROR;
    }
    return s->current_state;
}
int session_get_session(char *session_id, session **session_handle) {
    if(!session_tree) {
        JNX_LOG(DEFAULT_CONTEXT,"Session get session: no tree found\n");
        return 0;
    }
    JNX_LOG(DEFAULT_CONTEXT,"================Getting session [%s]===========\n",session_id);
    jnx_list *keys = jnx_list_create();
    jnx_btree_keys(session_tree,keys);

    while(keys->head) {
        if(strcmp(keys->head->_data,session_id) == 0) {
            session *s = jnx_btree_lookup(session_tree,keys->head->_data);
            if(s) {
                JNX_LOG(DEFAULT_CONTEXT,"Found valid session %s\n",s->session_id);
                *session_handle = s;
                JNXCHECK(*session_handle);
                jnx_list_destroy(&keys);
                return 1;
            }
        }
        keys->head = keys->head->next_node;
    }
    jnx_list_destroy(&keys);
    return 0;
}
char * session_create(raw_peer *local, raw_peer *foriegn) {
    if(!session_tree) {
        JNX_LOG(DEFAULT_CONTEXT,"Session create: no tree found; making\n");
        session_tree = jnx_btree_create(sizeof(int),session_tree_compare);
    }
    session *s = malloc(sizeof(session));
    s->local_peer = local;
    s->foriegn_peer = foriegn;
    s->shared_secret = NULL;
    s->shared_secret_len = 0;
    s->local_public_key = NULL;
    s->foriegn_public_key = NULL;
    s->local_keypair = NULL;
    s->current_state = SESSION_PRE_HANDSHAKE;
    s->session_id = session_generate_id();
    s->session_origin_guid = local->guid;
    int c = session_count();
    session_add(s);
    JNXCHECK(session_count() == (c+1));
    return s->session_id;
}
void session_add(session *s) {
    if(!session_tree) {
        JNX_LOG(DEFAULT_CONTEXT,"SESSION ADD: No tree found, making...\n");
        session_tree = jnx_btree_create(sizeof(int),session_tree_compare);
    }
    int n = session_count();
    jnx_btree_add(session_tree,s->session_id,s);
    JNX_LOG(DEFAULT_CONTEXT,"==============Added to session tree [%s] Session Count %d============",s->session_id,session_count());
    JNXCHECK(session_count() == (n+1));
}
void session_destroy(session *s) {
    JNXCHECK(s);
    ///remove from the session tree
    JNX_LOG(DEFAULT_CONTEXT,"Removing from session tree\n");
    jnx_btree_remove(session_tree,s->session_id,NULL,NULL);
    JNX_LOG(DEFAULT_CONTEXT,"Deleting session id...\n");
    free(s->session_id);
    if(s->shared_secret) {
        JNX_LOG(DEFAULT_CONTEXT,"Deleting session shared secret...\n");
        free(s->shared_secret);
    }
    if(s->foriegn_public_key) {
        JNX_LOG(DEFAULT_CONTEXT,"Deleting session foriegn public key...\n");
        free(s->foriegn_public_key);
    }
    if(s->local_public_key) {
        JNX_LOG(DEFAULT_CONTEXT,"Deleting session public key...\n");
        free(s->local_public_key);
    }
    JNX_LOG(DEFAULT_CONTEXT,"Deleting session object...\n");
    free(s);
}
int session_check_exists(raw_peer *local, raw_peer *foriegn) {
    if(!session_tree) {
        JNX_LOG(DEFAULT_CONTEXT,"Session check exists : no tree found, making\n");
        session_tree = jnx_btree_create(sizeof(int),session_tree_compare);
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
int session_check_exists_by_id(char *session_id) {
    if(!session_tree) {
        JNX_LOG(DEFAULT_CONTEXT,"BUILDING SESSION TREE\n");
        session_tree = jnx_btree_create(sizeof(int),session_tree_compare);
        return 0;
    }
    jnx_list *keys = jnx_list_create();
    jnx_btree_keys(session_tree,keys);

    while(keys->head) {
        if(strcmp(keys->head->_data,session_id) == 0) {
            session *s = jnx_btree_lookup(session_tree,keys->head->_data);
            if(s) {
                JNX_LOG(DEFAULT_CONTEXT,"Found valid session %s\n",s->session_id);
                jnx_list_destroy(&keys);
                return 1;
            }
        }
        keys->head = keys->head->next_node;
    }
    jnx_list_destroy(&keys);
    return 0;
}
int session_count() {
    if(!session_tree) {
        JNX_LOG(DEFAULT_CONTEXT,"session_count: no session tree\n");
        return 0;
    }
    int n = 0;
    jnx_list *l = jnx_list_create();
    jnx_btree_keys(session_tree,l);
    n = l->counter;
    jnx_list_destroy(&l);
    return n;
}
jnx_list *session_get_connected_sessions() {
	if(!session_tree) {
		return NULL;
	}
	jnx_list *l = jnx_list_create();
	jnx_btree_keys(session_tree,l);
	if(l->counter == 0) {
		jnx_list_destroy(&l);
		return NULL;
	}
	jnx_list *cl = jnx_list_create();
	while(l->head) {
		session *s = jnx_btree_lookup(session_tree,l->head->_data);
		if(s->current_state == SESSION_CONNECTED) {
			jnx_list_add(cl,s);
		}
		l->head = l->head->next_node;
	}
	return cl;
}
void session_shutdown(session *s) {
	if(s->current_state != SESSION_CLOSING) {
		s->current_state = SESSION_CLOSING;
	}
	//need to replicate this event on the other client
	
}
