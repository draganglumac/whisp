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
#define ASYNC_START(X,Y)\
	jnx_thread_create_disposable(X,Y);

typedef struct thread_data {
    char* port;
    jnx_socket *s;
    char *bgroup;
    //multicasting information
    size_t len;
    char *ip;
    char *msg;

} thread_data;
jnx_thread_mutex discovery_lock;
static jnx_hashmap *configuration;
typedef int clock_interval;
static clock_interval interval = 5;
static clock_t start_t = NULL;
static clock_t end_t = NULL;
jnx_socket *multicast_pulse_out;
jnx_socket *multicast_pulse_in;

static thread_data *multicast_listen_thrdata;
static thread_data *multicast_send_thrdata;



///////ASYNC THREAD////////
void *multicast_serialization_process(void *args) {
    thread_data *p = (thread_data*)args;
    raw_peer *rp = NULL;
    JNX_LOGC("Deserializing [%s %d %s]\n",p->msg,p->len,p->ip);
    S_TYPES ret = deserialize_data(&rp,p->msg,p->len,p->ip);
    JNX_MEM_FREE(p);
    switch(ret) {
    case S_MALFORMED:
        JNX_LOGC("Malformed deserialization data...\n");
        break;
    case S_GENERAL_ERROR:
        JNX_LOGC("General error deserializing...\n");
        break;
    case S_UNKNOWN:
        JNX_LOGC("Unknown error deserializing...\n");
        break;
    case S_OKAY:
        if(p) {
            if(!peerstore_check_peer(rp->guid)) {
				peerstore_add_peer(rp);
            }
            JNX_MEM_FREE(rp);
        } else {
            JNX_LOGC("Error raw_peer data is null\n");
        }
        break;
    }
    return 0;
}
///////////////////////////


///////// THREAD TWO ///////
void multicast_listener(char *msg, size_t len, char *ip) {
    JNX_LOGC("Multicast listener got [%s]\n",msg);
    thread_data *thr = JNX_MEM_MALLOC(sizeof(thread_data));
    thr->len = len;
    thr->msg = msg;
    thr->ip = ip;
    ASYNC_START(multicast_serialization_process,thr);
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
    size_t len = serialize_data(&buffer);
    jnx_socket_udp_send(data->s,data->bgroup,data->port,buffer,len);
    JNX_MEM_FREE(buffer);
    return 0;
}
///////////////////////////
void discovery_setup(jnx_hashmap *configuration) {
    configuration = configuration;

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
void discovery_start() {
    while(1) {
        if(!start_t) {
            printf("Pulsing multicast beacon...\n");
            ASYNC_START(multicast_pulse,multicast_send_thrdata);
            start_t = clock();
        } else {
            end_t = clock();
            if(((end_t - start_t) / CLOCKS_PER_SEC) > interval) {
                printf("Pulsing multicast beacon...\n");
                ASYNC_START(multicast_pulse,multicast_send_thrdata);
                start_t = clock();
            }
        }
    }
}
void discovery_teardown() {

}









