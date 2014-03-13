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

void authentication_update_foriegn_session(session *s) {
    jnx_socket *sec = jnx_socket_tcp_create(AF_INET);
    char *buffer;
    size_t len = serialize_session_data(&buffer,s);

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
// local_public_key will spawn from session_orig_guid peer
// remote_public_key will spawn from session target peer

	///need to find a way to stop constantly looping whilst waiting for other end
	//
	//
        switch(s->current_state) {
        case SESSION_ERROR:
            JNX_LOGC("SESSION ERROR\n");
            break;
        case SESSION_PRE_HANDSHAKE:               ///A------->>>
            JNX_LOGC("SESSION PRE HANDSHAKE\n");
	
			s->local_keypair = generate_key(2048);
			///this will be swapped on remote
			s->local_public_key = key_to_string(s->local_keypair,PUBLIC);

			JNX_LOGC("Generated local keypair\n");
			//I want your keys!
			s->current_state = SESSION_PUBLIC_KEY_EXCHANGE;
			
			//HOW ARE WE PRESERVING LOCAL KEYPAIRS??
			
			if(strcmp(jnx_hash_get(configuration,"GUID"),s->session_origin_guid) == 0) {
      		  printf("Sharing session...\n");
    		    authentication_update_foriegn_session(s);
  			  }
			break;
//B--->
		case SESSION_PUBLIC_KEY_EXCHANGE: 
			JNX_LOGC("Okay foreign peer is returning public key...\n");
			s->local_keypair = generate_key(2048);
			///this will be swapped on remote
			s->foriegn_public_key = key_to_string(s->local_keypair,PUBLIC);

			///Now you have my key, lets shake on it!
			s->current_state = SESSION_HANDSHAKING;
			///SEND UPDATE
    		authentication_update_foriegn_session(s); ///B------->>>>

            JNX_LOGC("SESSION KEY EXCHANGE\n");
            break;
        case SESSION_HANDSHAKING:

			JNX_LOGC("Got public keys. Time to encrypt a shared session key\n");
			char *shared_key = session_generate_secret();

			JNX_LOGC("UNENCRYPTED SHARED KEY [%s]\n",shared_key);
			size_t olen;
			RSA *their_pubpair = string_to_key(s->foriegn_public_key,PUBLIC);
			char *encrypted = encrypt_message(their_pubpair,shared_key,&olen);
			JNX_LOGC("ENCRYPTED SHARED KEY [%s]\n",encrypted);	
			///encrypt with [session_orig_guid public key]
			///FOR NOW I WILL TAKE IT ON FAITH, WE ALWAYS SET LOCAL_PUBLIC_KEY AS MESSAGE ORIGINATOR
			
			s->shared_secret = encrypted;
			s->current_state = SESSION_CONNECTED;	
    		
			authentication_update_foriegn_session(s);
			///FREE FREE FREE FREE
			///FREE FREE FREE FREE
			///FREE FREE FREE FREE
			///FREE FREE FREE FREE
			///FREE FREE FREE FREE
			JNX_LOGC("SESSION HANDSHAKING\n");
            break;
        case SESSION_CONNECTED:
    		printf("Session Connected\n");
			
			size_t len;
			char *decrypted = decrypt_message(s->local_keypair,s->shared_secret,
				strlen(s->shared_secret),&len);	
			printf("Unencrypted shared secret --- [%s]\n",decrypted);
            break;
        }
        sleep(5);
}
