/*
 * =====================================================================================
 *
 *       Filename:  sessioncontrol.h
 *
 *    Description:  Creates and controls online sessions
 *
 *        Version:  1.0
 *        Created:  03/10/14 14:23:50
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jonesax (), 
 *   Organization:  
 *
 * =====================================================================================
 */
#ifndef __SESSIONCONTROL_H__
#define __SESSIONCONTROL_H__
#include <jnxc_headers/jnxlist.h>
#include "peerstore.h"
typedef enum state {
	SESSION_CLOSING,
	SESSION_DORMANT,
	SESSION_ERROR,
	SESSION_PRE_HANDSHAKE, // <- all sessions start here
	SESSION_PUBLIC_KEY_EXCHANGE,
	SESSION_HANDSHAKING,
	SESSION_CONNECTED      // <- all sessions end here
}state;
typedef struct session {
	raw_peer *local_peer;
	raw_peer *foriegn_peer;
	//RSA keys
	RSA *local_keypair;
	char *local_public_key;
	char *foriegn_public_key;	
	char *shared_secret;
	size_t shared_secret_len;
	state current_state;
	char *session_id;
	char *session_origin_guid;
}session;

char *session_generate_secret();

state session_get_state(char *session_id);

char* session_create(raw_peer *local, raw_peer *foriegn);

int session_get_session(char *session_id, session **session_handle);

void session_add(session *s);

void session_destroy(session *s);

int session_check_exists(raw_peer *local, raw_peer *foriegn);

int session_check_exists_by_id(char *session_id);

int session_count();

jnx_list *session_get_connected_sessions();

void session_shutdown(session *s);
#endif
