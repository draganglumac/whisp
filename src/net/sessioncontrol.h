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
	SESSION_PRE_HANDSHAKE,
	SESSION_HANDSHAKE,
	SESSION_CONNECTED,
	SESSION_ERROR
}state;
typedef struct session {
	raw_peer *local_peer;
	raw_peer *foriegn_peer;
	//RSA keys
	RSA *local_keypair;
	RSA *foriegn_keypair;	
	char *shared_secret;
	state current_state;
	char *session_id;
}session;

state session_get_state(char *session_id);

char* session_create(raw_peer *local, raw_peer *foriegn);

session* session_get_session(char *session_id);

void session_destroy(session *s);

int session_check_exists(raw_peer *local, raw_peer *foriegn);
#endif
