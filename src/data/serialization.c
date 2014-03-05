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

void serialiser_setup(jnx_hashmap *configuration) {
    if(!guid_string) {
        guid_string = jnx_hash_get(configuration,"GUID");
        assert(guid_string);
    }
}
char *get_peers_as_string()
{
	char *out = NULL; 
	jnx_thread_lock(&peer_lock);
	out = peerstore_get_peerstring();
	jnx_thread_unlock(&peer_lock);
	return out;
}
size_t get_pulse_data(char **data) {
    size_t s = 0;
	char *peerstring = get_peers_as_string();
    char buffer[1024];
    bzero(buffer,1024);
    const char *data_frame = "[GUID:%s][COMMAND:%s][PEERAGE:%s]";
    sprintf(buffer,data_frame,guid_string,"PULSE",peerstring);

    s = strlen(buffer);

    *data = JNX_MEM_CALLOC(s,sizeof(char));
    memcpy(*data,buffer,s);
    assert(s != 0);
    return s;
}
