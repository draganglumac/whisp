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
#include "secure_channel.h"
#include <jnxc_headers/jnxmem.h>
#include <jnxc_headers/jnxlog.h>
#include <jnxc_headers/jnxhash.h>
#include "peerstore.h"
#include "local_macro.h"
extern jnx_hashmap *configuration;

state authentication_get_progress(session *s) {
	return s->current_state;
}
void authentication_update_foriegn_session(session *s) {
    jnx_socket *sec = jnx_socket_tcp_create(AF_INET);
    char *buffer;
    size_t len = serialize_session_data(&buffer,s);
    JNX_LOGC("<<<<<<<<< SENDING OUTGOING SESSION DATA [%s]\n",s->session_id);
    if(strcmp(jnx_hash_get(configuration,"DEBUG"),"YES") == 0) {
        jnx_socket_tcp_send(sec,"localhost",s->foriegn_peer->port,buffer,len);
    } else {
        jnx_socket_tcp_send(sec,s->foriegn_peer->ip,s->foriegn_peer->port,buffer,len);
    }
    JNX_MEM_FREE(buffer);
    jnx_socket_destroy(&sec);
}
state authentication_start_with_session(session *s) {
    /*
     *
     * A ---> PREHANDSHAKE
     *        PUBLIC KEY EXH ---> B
     *  <---- SESSION HANDSHAKE
     *      [ Exchanged keys ]
     * A --> Crypto sign session key --> B
     *   A------Connected-------B
     *   Use shared session secret key for DES symmetrical encryption
     */
// local_public_key will spawn from session_orig_guid peer
// remote_public_key will spawn from session target peer
    switch(s->current_state) {
    case SESSION_ERROR:
        JNX_LOGC("SESSION ERROR\n");
        break;
    case SESSION_PRE_HANDSHAKE:               ///A------->>>
        JNX_LOGC("SESSION PRE HANDSHAKE\n");

        s->local_keypair = generate_key(2048);
        s->local_public_key = key_to_string(s->local_keypair,PUBLIC);

        JNX_LOGC("Generated local keypair\n");
        s->current_state = SESSION_PUBLIC_KEY_EXCHANGE;
        if(strcmp(jnx_hash_get(configuration,"GUID"),s->session_origin_guid) == 0) {
            printf("Sharing session...\n");
            authentication_update_foriegn_session(s);
        }
        break;
//B--->
    case SESSION_PUBLIC_KEY_EXCHANGE:
        JNX_LOGC("Okay foreign peer is returning public key...\n");
        s->local_keypair = generate_key(2048);
        s->foriegn_public_key = key_to_string(s->local_keypair,PUBLIC);

        s->current_state = SESSION_HANDSHAKING;
        authentication_update_foriegn_session(s); ///B------->>>>

        JNX_LOGC("SESSION KEY EXCHANGE\n");
        break;
    case SESSION_HANDSHAKING:

        JNX_LOGC("Got public keys. Time to encrypt a shared session key\n");
        char *shared_key = session_generate_secret();

        JNX_LOGC("UNENCRYPTED SHARED KEY [%s]\n",shared_key);
        size_t olen;

        RSA *their_pubpair = string_to_key(s->foriegn_public_key,PUBLIC);
        JNX_LOGC("Using their foriegn key for encryption\n%s\n",key_to_string(their_pubpair,PUBLIC));
        char *encrypted = encrypt_message(their_pubpair,shared_key,&olen);

        JNX_LOGC("ENCRYPTED SHARED KEY [%dbbytes][%s]\n",olen,encrypted);

        s->shared_secret = encrypted;
        s->shared_secret_len = olen;
        s->current_state = SESSION_CONNECTED;
        authentication_update_foriegn_session(s);

        RSA_free(their_pubpair);
        ///Re-entrant for local peer - he wants to get to connected state also
        JNX_LOGC("SESSION HANDSHAKING\n");
        ///IMPORTANT: Retain the plaintext shared secret
        s->shared_secret = shared_key;
		authentication_start_with_session(s);
		///we dont need to keep the encyrpted shared_secret locally
		free(encrypted);
        break;
    case SESSION_CONNECTED:
        JNX_LOGC("SESSION CONNECTED\n");
        size_t len;

        if(strcmp(jnx_hash_get(configuration,"GUID"),s->session_origin_guid) == 0) {
            printf("Local peer connected with shared secret of %s\n",s->shared_secret);

            secure_channel_listener_resolve_with_session(s);
		
			secure_channel_start_dialogue(s);
        } else {

			char *shared_secret = s->shared_secret;
			char *decrypted = decrypt_message(s->local_keypair,shared_secret,
                                              s->shared_secret_len,&len);
            s->shared_secret = decrypted;
            printf("Foriegn peer connected with shared secret of %s\n",s->shared_secret);
            secure_channel_listener_resolve_with_session(s);
			free(shared_secret);
			
			secure_channel_start_dialogue(s);
			
		}
        break;
    }
	return s->current_state;
}
