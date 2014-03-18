/*
 * =====================================================================================
 *
 *       Filename:  secure_channel.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  03/17/14 18:00:11
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
#include <jnxc_headers/jnxlog.h>
#include <jnxc_headers/jnxsocket.h>
#include <jnxc_headers/jnxmem.h>
#include "secure_channel.h"
#include "des.h"
#include "local_macro.h"
static jnx_socket *secure_socket = NULL;
static jnx_hashmap *configuration = NULL;
session *last_session = NULL;

int secure_listener_callback(char *msg, size_t msg_len, char *ip) {

	if(last_session) {
		printf("Attempting to resolve message...\n");

		printf("Decrypting with %s\n",last_session->shared_secret);
		char *decrypted_message = des_decrypt(last_session->shared_secret,
				msg,msg_len);

		printf("decrypted message: %s\n",decrypted_message);
		
		free(decrypted_message);
		free(msg);

	}
	
	return 0;
}
size_t secure_channel_send(session *s, char *message, size_t msg_len) {

	jnx_socket *sec = jnx_socket_tcp_create(AF_INET);
	
	printf("encrypting \n%s\nwith %s\n",message,s->shared_secret);
	char *buffer = des_encrypt(s->shared_secret,message,msg_len);

    if(strcmp(jnx_hash_get(configuration,"DEBUG"),"YES") == 0) {
        jnx_socket_tcp_send(sec,"localhost",s->foriegn_peer->secure_port,buffer,strlen(buffer));
    } else {
        jnx_socket_tcp_send(sec,s->foriegn_peer->ip,s->foriegn_peer->secure_port,buffer,strlen(buffer));
    }

	free(buffer);
	jnx_socket_destroy(&sec);
}

int secure_channel_listener_resolve_with_session(session *s) {
	last_session = s;
	return 0;
}
void *secure_listener_start(void *args) {
	char *port = args;
	jnx_socket_tcp_listen(secure_socket,port,
			10, secure_listener_callback);
	return 0;
}
void secure_channel_setup(jnx_hashmap *config) {
	printf("Starting secure socket channel...\n");
	configuration = config;
	char *sport = jnx_hash_get(configuration,"SECUREPORT");
	assert(sport);
	secure_socket = jnx_socket_tcp_create(AF_INET);
	ASYNC_START(secure_listener_start,sport);
}
void secure_channel_start_dialogue(session *s) {
	while(1) {
		char *line;
		size_t n;
	
		secure_channel_send(s,"All is well",strlen("All is well") +1);
		sleep(2);
	}
}
