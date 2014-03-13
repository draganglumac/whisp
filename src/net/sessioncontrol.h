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
#include "peerstore.h"
typedef enum state {
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
	state current_state;
	char *session_id;
	char *session_origin_guid;
}session;

state session_get_state(char *session_id);

char* session_create(raw_peer *local, raw_peer *foriegn);

session* session_get_session(char *session_id);

void session_add(session *s);

void session_destroy(session *s);

int session_check_exists(raw_peer *local, raw_peer *foriegn);

#endif
