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
#include "encryption.h"
#include "authentication.h"
#include <jnxc_headers/jnxmem.h>
#include <jnxc_headers/jnxlog.h>
#include <jnxc_headers/jnxhash.h>
#include "peerstore.h"
#include "local_macro.h"
extern jnx_hashmap *configuration;

void authentication_update_foriegn_session(session *s, char *is_update) {
    jnx_socket *sec = jnx_socket_tcp_create(AF_INET);
    char *buffer;
    size_t len = serialize_session_data(&buffer,s,is_update);

    if(strcmp(jnx_hash_get(configuration,"DEBUG"),"YES") == 0) {
        jnx_socket_tcp_send(sec,"localhost",s->foriegn_peer->port,buffer,len);
    } else {
        jnx_socket_tcp_send(sec,s->foriegn_peer->ip,s->foriegn_peer->port,buffer,len);
    }
    JNX_MEM_FREE(buffer);
    jnx_socket_destroy(&sec);
}
void authentication_start_with_session(session *s) {

/*
 * 
 * A ---> PREHANDSHAKE  
 *        PUBLIC KEY EXH ---> B
 *  <---- SESSION HANDSHAKE    
 *      [ Exchanged keys ]
 * A --> Crypto sign session key --> B
 *  <--- Acknowledge key with reciept
 *   A------Connected-------B
 */

    state current_state;
    while((current_state = session_get_state(s->session_id)) != SESSION_CONNECTED) {
        switch(current_state) {
        case SESSION_ERROR:

            JNX_LOGC("SESSION ERROR\n");
            break;
        case SESSION_PRE_HANDSHAKE:
            JNX_LOGC("SESSION PRE HANDSHAKE\n");
	
			s->local_keypair = generate_key(2048);
			///this will be swapped on remote
			s->local_public_key = key_to_string(s->local_keypair,PUBLIC);

			JNX_LOGC("Generated local keypair\n");
			//I want your keys!
			s->current_state = SESSION_PUBLIC_KEY_EXCHANGE;
			
    		if(strcmp(jnx_hash_get(configuration,"GUID"),s->session_origin_guid) == 0) {
      		  printf("Sharing session...\n");
    		    authentication_update_foriegn_session(s,"NO");
  			  }
			break;
        case SESSION_PUBLIC_KEY_EXCHANGE:
			JNX_LOGC("Okay foreign peer is returning public key...\n");
			s->local_keypair = generate_key(2048);
			///this will be swapped on remote
			s->local_public_key = key_to_string(s->local_keypair,PUBLIC);

			///Now you have my key, lets shake on it!
			s->current_state = SESSION_HANDSHAKING;
			///SEND UPDATE
    		authentication_update_foriegn_session(s,"YES");

            JNX_LOGC("SESSION KEY EXCHANGE\n");
            break;
        case SESSION_HANDSHAKING:
            JNX_LOGC("SESSION HANDSHAKING\n");
            break;
        case SESSION_CONNECTED:
            JNX_LOGC("SESSION CONNECTED\n");
            break;
        }
        sleep(5);
    }
    printf("Session Connected\n");
}
