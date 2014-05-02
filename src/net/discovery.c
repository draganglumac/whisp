/*
 * =====================================================================================
 *
 *       Filename:  discovery.c
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
#include <jnxc_headers/jnxsocket.h>
#include <jnxc_headers/jnxlog.h>
#include "discovery.h"
#include "serialization.h"
#include "peerstore.h"
#include "local_macro.h"
jnx_hashmap *configuration;
typedef int clock_interval;
static clock_interval interval = 5;
static clock_t start_t = NULL;
static clock_t end_t = NULL;
jnx_socket *multicast_pulse_out;
jnx_socket *multicast_pulse_in;
static jnx_thread_mutex clock_lock;
static thread_data *multicast_listen_thrdata;
static thread_data *multicast_send_thrdata;

static int is_not_exiting = 0;

///////ASYNC THREAD////////
void *multicast_serialization_process(void *args) {
    thread_data *p = (thread_data*)args;
    raw_peer *rp = NULL;
    S_TYPES ret = deserialize_data(&rp,p->msg,p->len,p->ip);
	free(p);
    switch(ret) {
    case S_MALFORMED:
        JNX_LOGC(JLOG_NORMAL,"malformed deserialization data...\n");
        break;
    case S_GENERAL_ERROR:
        JNX_LOGC(JLOG_NORMAL,"general error deserializing...\n");
        break;
    case S_UNKNOWN:
        JNX_LOGC(JLOG_NORMAL,"unknown error deserializing...\n");
        break;
    case S_OKAY:
        assert(rp);
        assert(rp->guid);
        assert(rp->command);
        assert(rp->ip);
        assert(rp->port);
        raw_peer *handle;
        if(peerstore_check_peer(rp->guid,&handle) == 0) {
            peerstore_add_peer(rp);
        } else {
			peerstore_update_peer(rp,handle);
			///Do not call peerstore_delete_peer
			//We want to keep the peer data, but delete the peer struct!!!!!
			JNX_MEM_FREE(rp);
		}
    }
    return 0;
}
///////////////////////////

///////// THREAD TWO ///////
int multicast_listener(uint8_t *msg, size_t len, jnx_socket *s) {
    thread_data *thr = JNX_MEM_MALLOC(sizeof(thread_data));
    thr->len = len;
    thr->msg = msg;
	assert(s->ipaddress);
	thr->ip = s->ipaddress;
 
	ASYNC_START(multicast_serialization_process,thr);
    return is_not_exiting;
}
void *multicast_listen_start(void *args) {
    thread_data *data = (thread_data*)args;
    jnx_socket_udp_listen(data->s,data->port,24,multicast_listener);
    return 0;
}
////////////////////////////

///////ASYNC THREAD////////
void *multicast_pulse(void *args) {
    thread_data *data = (thread_data*)args;
    char *buffer;
    size_t len = serialize_data(&buffer,jnx_hash_get(configuration,"GUID"),"PULSE",
                                jnx_hash_get(configuration,"TPORT"),jnx_hash_get(configuration,"SECUREPORT"),"PEER-NULL");
    jnx_socket_udp_send(data->s,data->bgroup,data->port,buffer,len);
    JNX_MEM_FREE(buffer);
    return 0;
}
///////////////////////////
void discovery_setup(jnx_hashmap *config) {
    configuration = config;
    assert(configuration);

    char *af = jnx_hash_get(configuration,"ADDFAMILY");
    unsigned int family = AF_INET;
    if(strcmp(af,"AF_INET6") == 0) {
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
    //Note: loopback has been ENABLED
    //It is useful to enable for local debugging
    jnx_socket_udp_enable_multicast_send(multicast_pulse_out,ip,0);
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
void* discovery_start(void *args) {
    while(is_not_exiting == 0) {
        if(!start_t) {
            jnx_thread_lock(&clock_lock);
        	multicast_pulse(multicast_send_thrdata);
			start_t = clock();
            jnx_thread_unlock(&clock_lock);
        } else {
            jnx_thread_lock(&clock_lock);
            end_t = clock();
            jnx_thread_unlock(&clock_lock);
            if(((end_t - start_t) / CLOCKS_PER_SEC) > interval) {
                jnx_thread_lock(&clock_lock);
        		multicast_pulse(multicast_send_thrdata);
                start_t = clock();
                jnx_thread_unlock(&clock_lock);
            }
        }
    }
    return 0;
}
void discovery_teardown() {
	is_not_exiting = 1;
    jnx_socket_destroy(&multicast_pulse_out);
    jnx_socket_destroy(&multicast_pulse_in);
    JNX_MEM_FREE(multicast_send_thrdata);
    JNX_MEM_FREE(multicast_listen_thrdata);
}
