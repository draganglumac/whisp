/*
 * =====================================================================================
 *
 *       Filename:  protocol.c
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  03/03/14 18:09:36
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jonesax (),
 *   Organization:
 *
 * =====================================================================================
 */
#include <jnxc_headers/jnxmem.h>
#include <jnxc_headers/jnxlog.h>
#include <jnxc_headers/jnxthread.h>
#include <stdlib.h>
#include "serialization.h"
#include "peerstore.h"
jnx_thread_mutex peer_lock;
static char* guid_string = NULL;
#define MAX_STR 1024
#define GUID_KEY "GUID"
#define COMMAND_KEY "COMMAND"
#define PEERAGE_KEY "PEERAGE"
#define DELIMITER ":"
void serialiser_setup(jnx_hashmap *configuration) {
    if(!guid_string) {
        guid_string = jnx_hash_get(configuration,"GUID");
        assert(guid_string);
    }
}
size_t serialize_data(char **outbuffer,char *guid,char *command, char *peerage) {
    size_t s = 0;
    char buffer[1024];
    bzero(buffer,1024);
    const char *data_frame = "GUID:%s:COMMAND:%s:PEERAGE:%s:";
    sprintf(buffer,data_frame,guid,command,peerage);
    s = strlen(buffer);
    *outbuffer = JNX_MEM_CALLOC(s,sizeof(char));
    memcpy(*outbuffer,buffer,s);
    assert(s != 0);
    return s;

}
S_TYPES deserialize_data(raw_peer **outpeer, char *raw_message, size_t raw_message_len, char *interface_ip) {

    if(raw_message_len == 0) {
        return S_MALFORMED;
    }
    char *saveptr = NULL;
    char *t = strtok_r(raw_message,DELIMITER,&saveptr);
    if(t == NULL) {
        return S_UNKNOWN;
    }
    *outpeer = JNX_MEM_MALLOC(sizeof(raw_peer));
    while(t != NULL) {
        if(strcmp(t,GUID_KEY) == 0) {
            char *value = strtok_r(NULL,DELIMITER,&saveptr);
            if(value == NULL) {
                JNX_MEM_FREE(*outpeer);
                *outpeer = NULL;
                return S_MALFORMED;
            }
            (*outpeer)->guid = strdup(value);
            //        JNX_LOGC("GUID KEY %s with value %s\n",t,(*outpeer)->guid);
        }
        if(strcmp(t, COMMAND_KEY) == 0) {
            char *value = strtok_r(NULL,DELIMITER,&saveptr);
            if(value == NULL) {
                JNX_MEM_FREE((*outpeer)->guid);
                JNX_MEM_FREE(*outpeer);
                *outpeer = NULL;
                return S_MALFORMED;
            }
            (*outpeer)->command = strdup(value);
            //          JNX_LOGC("COMMAND KEY %s with value %s\n",t,(*outpeer)->command);
        }
        if(strcmp(t,PEERAGE_KEY) == 0) {
            char *value = strtok_r(NULL,DELIMITER,&saveptr);
            if(value == NULL) {
                JNX_MEM_FREE((*outpeer)->guid);
                JNX_MEM_FREE((*outpeer)->command);
                JNX_MEM_FREE(*outpeer);
                *outpeer = NULL;
                return S_MALFORMED;
            }
            (*outpeer)->peerstring = strdup(value);
//            JNX_LOGC("PEER KEY %s with value %s\n",t,(*outpeer)->peerstring);
        }
        (*outpeer)->ip = interface_ip;
        (*outpeer)->has_public_key = 0;
        (*outpeer)->publickey = NULL;

        t = strtok_r(NULL,DELIMITER,&saveptr);
    }
    return S_OKAY;
}











