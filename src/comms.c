/*
 * =====================================================================================
 *
 *       Filename:  comms.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  03/03/14 18:08:08
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
#include <jnxc_headers/jnxthread.h>
#include <jnxc_headers/jnxmem.h>
#include "networkhandlers.h"
#include "comms.h"
#define ASYNC_START(X,Y)\
	jnx_thread_create_disposable(X,Y);

state current_state;
jnx_thread_mutex comms_lock;
static jnx_hashmap *configuration;
typedef int clock_interval;
static clock_interval interval = 5;
static clock_t start_t = NULL;
static clock_t end_t = NULL;
jnx_socket *multicast_pulse_out;
jnx_socket *multicast_pulse_in;

static thread_data *multicast_listen_thrdata;
static thread_data *multicast_send_thrdata;

void hint_new_current_state(state hint)
{
	jnx_thread_trylock(&comms_lock);
	current_state = hint;
	jnx_thread_unlock(&comms_lock);
}
state comms_current_state()
{
	jnx_thread_trylock(&comms_lock);
	state r = current_state;
	jnx_thread_unlock(&comms_lock);
	return r;
}
void comms_setup(jnx_hashmap *configuration)
{
	hint_new_current_state(SEARCH);
	configuration = configuration;

	char *af = jnx_hash_get(configuration,"ADDFAMILY");
	unsigned int family = AF_INET;
	if(strcmp(af,"AF_INET6") == 0)
	{
		family = AF_INET6;
	}
	char *bgroup = jnx_hash_get(configuration,"BGROUP");
	assert(bgroup);
	char *bport = jnx_hash_get(configuration,"BPORT");
	assert(bport);
	char *tport = jnx_hash_get(configuration,"TPORT");
	assert(tport);
	char *ip = jnx_hash_get(configuration,"IP");
	assert(ip);
	int cinterval = atoi(jnx_hash_get(configuration,"INTERVAL"));
	assert(cinterval);
	interval = cinterval;
	multicast_pulse_out = jnx_socket_udp_create(family);
	printf("Enabling multicast sender...\n");
	//Note: loopback has been disabled
	//It is useful to enable for local debugging
	jnx_socket_udp_enable_multicast_send(multicast_pulse_out,ip,1);
	multicast_pulse_in = jnx_socket_udp_create(family);
	printf("Enabling multicast listener...\n");
	jnx_socket_udp_enable_multicast_listen(multicast_pulse_in,ip,bgroup);

	multicast_listen_thrdata = JNX_MEM_MALLOC(sizeof(thread_data));
	multicast_listen_thrdata->s = multicast_pulse_in;
	multicast_listen_thrdata->port = bport;

	multicast_send_thrdata = JNX_MEM_MALLOC(sizeof(thread_data));
	multicast_send_thrdata->s = multicast_pulse_out;
	multicast_send_thrdata->port = bport;
	multicast_send_thrdata->bgroup = bgroup;
	ASYNC_START(multicast_listen_start,multicast_listen_thrdata);				
}
void comms_start()
{
	while(current_state != EXITING){
		switch(current_state)
		{
			case SEARCH:
				if(!start_t)
				{
					printf("Pulsing multicast beacon...\n");
					ASYNC_START(multicast_pulse,multicast_send_thrdata);
					start_t = clock();
				}else{
					end_t = clock();
					if(((end_t - start_t) / CLOCKS_PER_SEC) > interval)
					{
						printf("Pulsing multicast beacon...\n");
						ASYNC_START(multicast_pulse,multicast_send_thrdata);
						start_t = clock();
					}
				}			
				break;
			case DISCOVERY:

				break;
			case MESSAGE:

				break;
		}
	}
}
void comms_teardown()
{

}
