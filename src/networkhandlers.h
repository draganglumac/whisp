/*
 * =====================================================================================
 *
 *       Filename:  networkhandlers.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  03/03/14 22:05:54
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jonesax (), 
 *   Organization:  
 *
 * =====================================================================================
 */
#ifndef __NETWORKHANDLERS_H__
#define __NETWORKHANDLERS_H__
#include <jnxc_headers/jnxsocket.h>
#include "protocol.h"
typedef struct thread_data{
	jnx_socket *s;
	char* port;
	char* bgroup;
}thread_data;

/*
 * The idea is that actual sending of network traffic and resolution of NAT/Firewalls
 * is done behind this interface
 *
 *
 */
void *multicast_listen_start(void *args);
void *multicast_pulse(void *args);
#endif
