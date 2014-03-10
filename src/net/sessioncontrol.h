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
	PRE_HANSHAKE,
	HANDSHAKE,
	ENCRYPTING,
	DECRYPTING,
}state;
typedef struct session {
	raw_peer *local_peer;
	raw_peer *foriegn_peer;
	state current_state;
	char *session_id;
}session;

int session_check(char *session_id);
session* session_create(raw_peer *local, raw_peer *foriegn);

#endif
