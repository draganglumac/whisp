/*
 * =====================================================================================
 *
 *       Filename:  authentication.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  03/10/14 18:34:36
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jonesax (), 
 *   Organization:  
 *
 * =====================================================================================
 */
#ifndef __AUTHENTICATION_H__
#define __AUTHENTICATION_H__
#include "sessioncontrol.h"
#include <jnxc_headers/jnxhash.h>

void authentication_passive_listen(jnx_hashmap *configuration); 
// This is where session requests will be transmitted over the tcp socket
void authentication_start_session_from_incoming_convo(session *s);

void authentication_start_session_from_convo(session *s);

#endif
