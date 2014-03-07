/*
 * =====================================================================================
 *
 *       Filename:  connectioncontrol.c
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  03/06/14 19:28:03
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
#include <unistd.h>
#include <jnxc_headers/jnxthread.h>
#include <jnxc_headers/jnxlog.h>
#include <jnxc_headers/jnxhash.h>
#include "peerstore.h"
#include "connectioncontrol.h"
#include "encryption.h"
#include "discovery.h"

static int connected = 0;
static jnx_thread_mutex control_lock;
static jnx_hashmap *configuration;

static jnx_socket *session_tcp_listener;
static jnx_socket *session_tcp_connector;

////SESSION DETAILS /////
static char *theirpublickey = NULL;


static RSA *mysessionkeypair;
static char *mysessionpublickeystr;
//Not really thread safe

int session_listener_callback(char *msg, size_t len , char *ip) {

	return 0;
}
void* connectioncontrol_setup(void *args) {
    configuration =(jnx_hashmap*)args;

	session_tcp_listener = jnx_socket_tcp_create(AF_INET);
	session_tcp_connector = jnx_socket_tcp_create(AF_INET);
	JNX_LOGC("Starting TCP listener socket on %s\n",jnx_hash_get(configuration,"TPORT"));
	jnx_socket_tcp_listen(session_tcp_connector,jnx_hash_get(configuration,"TPORT"),10,
			session_listener_callback);
}
int connectioncontrol_isconnected(void) {

    jnx_thread_lock(&control_lock);
    int ret = connected;
    jnx_thread_unlock(&control_lock);
    return ret;
}
void* connectioncontrol_start(void *args) {
    printf("Starting connection control\n");
    theirpublickey = NULL;
	mysessionpublickeystr = NULL;
	raw_peer *rp = (raw_peer*)args;

    if(rp->has_public_key) {
        theirpublickey = rp->publickey;
		assert(theirpublickey);
        JNX_LOGC("Already have peer public key it is %s\n",theirpublickey);

		///Different control flow..
	}

    //GENERATE PUBLIC/PRIVATE KEY PAIR
	JNX_LOGC("Generating local keys...\n");
	
	RSA *localkeypair = generate_key(2048);
	
	mysessionpublickeystr = key_to_string(localkeypair,PUBLIC);

	printf("%s\n",mysessionpublickeystr);

    //-------DO WE ALREADY HAVE THEIR PUBLIC KEY   IF YES GOTO ***
    ///Send our public key to target

    //Wait in a loop until the TCP listener receives their public key, encrypted with ours

    //Decrypt their public key


    ///keep their public key string in peerstore
    //***

    connected = 1;
    return 0;
}



