/*
 * =====================================================================================
 *
 *       Filename:  secure_channel.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  03/17/14 17:59:39
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jonesax (), 
 *   Organization:  
 *
 * =====================================================================================
 */
#ifndef __SECURE_CHANNEL_H__
#define __SECURE_CHANNEL_H__
#include <jnxc_headers/jnxhash.h>
#include "sessioncontrol.h"
int secure_channel_listener_resolve_with_session(session *s);
void secure_channel_setup(jnx_hashmap *configuration);
size_t secure_channel_send(session *s, char *message, size_t msg_len);
#endif
