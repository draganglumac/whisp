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

int connected = 0;
static jnx_thread_mutex control_lock;
static jnx_hashmap *configuration;

static jnx_socket *session_tcp_listener;
static jnx_socket *session_tcp_connector;

////SESSION DETAILS /////
static char *theirpublickey = NULL;
static char *theirport = NULL;
static char *theirip = NULL;

RSA *mysessionkeypair= NULL;
static char *mysessionpublickeystr;
//Not really thread safe


typedef enum session_command { START, REPLY } session_command;
#define DELIMITER "||||"


char *build_payload(char *myguid, char *mypublickey, session_command s) {

    char *session_command_s = "ERROR";
    if(s == START) {
        session_command_s = "START";
    } else if (s == REPLY) {
        session_command_s = "REPLY";
    }
    char buffer[2048];
    bzero(buffer,2048);
    sprintf(buffer,"%s%s%s%s%s",session_command_s,DELIMITER,myguid,DELIMITER,mypublickey);

    return strdup(buffer);
}
char *wrap_and_send_my_key(char *incoming_publickey) {

    size_t outlen;
    char *encrypted_my_pubkey = encrypt_message(mysessionkeypair,
                                strdup(incoming_publickey),&outlen);

    printf("-> %zu\n",outlen);
    return encrypted_my_pubkey;
}
char *decrypted_key_with_local(char *incoming_publickey) {
    ///When we decrypt this with our private key, it will show their public key
}
int session_listener_callback(char *msg, size_t len , char *ip) {
    char *saveptr;
    char *t = strtok_r(msg,DELIMITER,&saveptr);
    char *incoming_command = NULL;
    char *incoming_guid = NULL;
    char *incoming_publickey = NULL;
    int n = 0;
    while(t != NULL) {

        switch(n) {
        case 0:
            incoming_command = t;
            break;
        case 1:
            incoming_guid = t;
            break;

        case 2:
            incoming_publickey = t;
            break;
        }
        t = strtok_r(NULL,DELIMITER,&saveptr);
        ++n;
    }
    printf("Received ~~~~~~~%s\n~~~~~~%s\n~~~~~%s\n",incoming_command,incoming_guid,
           incoming_publickey);

    if(strcmp(incoming_command,"START") == 0) {
        ///you are being sent a start handshake request. Lets acknowledge this
        mysessionkeypair = generate_key(2048);
        mysessionpublickeystr = key_to_string(mysessionkeypair,PUBLIC);
        //send this key back to our target by getting his raw_peer information
        printf("Checking to see if we have the peer stored...\n");
        raw_peer *rp;
        if(peerstore_check_peer(incoming_guid,&rp) == 1) {
            //lets send back our public key!
            assert(rp);
            printf("Sending our key as reply");
            char *mypayload = build_payload(jnx_hash_get(configuration,"GUID"), mysessionpublickeystr, REPLY);
            session_tcp_connector = jnx_socket_tcp_create(AF_INET);
            char *is_debug = jnx_hash_get(configuration,"DEBUG");
            assert(is_debug);
            if(strcmp(is_debug,"YES") == 0) {
                printf("RUNNING LOCAL ONLY DEBUG\n");
                jnx_socket_tcp_send(session_tcp_connector,"localhost",rp->port,mypayload,strlen(mypayload));
            } else {
                printf("RUNNING LIVE\n");
                jnx_socket_tcp_send(session_tcp_connector,rp->ip,rp->port,mypayload,strlen(mypayload));
            }
            jnx_socket_destroy(&session_tcp_connector);

            connected = 1;
        } else if(strcmp(incoming_command,"REPLY") == 0) {
            //you ave being sent back a reply
            printf("Got our targets public key -> %s\n",incoming_publickey);
            theirpublickey = incoming_publickey;

            raw_peer *their_stored_peer;
            if(peerstore_check_peer(incoming_guid,&their_stored_peer)) {
                //update
                their_stored_peer->publickey = strdup(incoming_publickey);
                printf("Storing a copy of public key in their peer store info\n");
            }
            ///And we are done!
            connected = 1;
        }
    }
    return 0;
}
void* connectioncontrol_setup(void *args) {
    configuration =(jnx_hashmap*)args;

    session_tcp_listener = jnx_socket_tcp_create(AF_INET);
    JNX_LOGC("Starting TCP listener socket on %s\n",jnx_hash_get(configuration,"TPORT"));
    jnx_socket_tcp_listen(session_tcp_listener,jnx_hash_get(configuration,"TPORT"),10,
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
    mysessionkeypair = NULL;
    theirport = NULL;
    theirip = NULL;
    connected = 0;
    raw_peer *rp = (raw_peer*)args;

    if(rp->has_public_key) {
        theirpublickey = rp->publickey;
        assert(theirpublickey);
        JNX_LOGC("Already have peer public key it is %s\n",theirpublickey);

        ///Different control flow..
    }
    theirport = rp->port;
    theirip = rp->ip;
    //GENERATE PUBLIC/PRIVATE KEY PAIR
    JNX_LOGC("Generating local keys...\n");

    mysessionkeypair = generate_key(2048);
    mysessionpublickeystr = key_to_string(mysessionkeypair,PUBLIC);
    printf("%s\n",mysessionpublickeystr);



    char *mypayload = build_payload(jnx_hash_get(configuration,"GUID"), mysessionpublickeystr, START);

    printf("My payload START handshake %s\n",mypayload);
    //-------DO WE ALREADY HAVE THEIR PUBLIC KEY   IF YES GOTO ***
    ///Send our public key to target

    //Wait in a loop until the TCP listener receives their public key, encrypted with ours
    JNX_LOGC("Sending to %s %s\n",rp->ip,rp->port);
    session_tcp_connector = jnx_socket_tcp_create(AF_INET);

    char *is_debug = jnx_hash_get(configuration,"DEBUG");
    assert(is_debug);
    if(strcmp(is_debug,"YES") == 0) {
        printf("RUNNING LOCAL ONLY DEBUG\n");
        jnx_socket_tcp_send(session_tcp_connector,"localhost",rp->port,mypayload,strlen(mypayload));
    } else {
        printf("RUNNING LIVE\n");
        jnx_socket_tcp_send(session_tcp_connector,rp->ip,rp->port,mypayload,strlen(mypayload));
    }
    jnx_socket_destroy(&session_tcp_connector);


    //Decrypt their public key

    ///keep their public key string in peerstore
    //***
    return 0;
}














