/*
 * =====================================================================================
 *
 *       Filename:  networkhandlers.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  03/03/14 22:06:09
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jonesax (), 
 *   Organization:  
 *
 * =====================================================================================
 */
#include <stdlib.h>
#include <stdio.h>
#include "networkhandlers.h"
#include <jnxc_headers/jnxmem.h>
void multicast_listener(char *msg, size_t len, char *ip)
{
	printf("[%s]\n",msg);
}
void *multicast_listen_start(void *args)
{
	thread_data *data = (thread_data*)args;
	jnx_socket_udp_listen(data->s,data->port,24,multicast_listener);


	return 0;
}
void *multicast_pulse(void *args)
{
	printf("Pulsing...\n");
	thread_data *data = (thread_data*)args;

	assert(data->bgroup);
	jnx_socket_udp_send(data->s,data->bgroup,data->port,"hi",2);	
	return 0;
}
