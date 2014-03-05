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
void serialiser_setup(jnx_hashmap *configuration) {
    if(!guid_string) {
        guid_string = jnx_hash_get(configuration,"GUID");
        assert(guid_string);
    }
}
size_t serialize_data(char **outbuffer) {
    size_t s = 0;
    char buffer[1024];
    bzero(buffer,1024);
    const char *data_frame = "GUID:%s:COMMAND:%s:PEERAGE:%s:";
    sprintf(buffer,data_frame,guid_string,"PULSE","NULL");
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



    return S_OKAY;
}



