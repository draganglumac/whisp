/*
 * =====================================================================================
 *
 *       Filename:  authentication.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  03/10/14 18:38:13
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jonesax (), 
 *   Organization:  
 *
 * =====================================================================================
 */
#include <stdlib.h>
#include "authentication.h"
#include "peerstore.h"
#include "local_macro.h"
void authentication_start_session_from_incoming_convo(session *s) {

}

void authentication_start_session_from_convo(session *s) {
	printf("Telling foriegn peer about local session...\n");

	QUICK_TCP(ck,"localhost",s->foriegn_peer->port,
		"Hello");	

}
