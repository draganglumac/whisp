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
#include <jnxc_headers/jnxlog.h>
#include <jnxc_headers/jnxthread.h>
#include <jnxc_headers/jnxhash.h>
#include <jnxc_headers/jnxcheck.h>
#include <stdlib.h>
#include "serialization.h"
#include "peerstore.h"
#define MAX_STR 1024
#define GUID_KEY "GUID"
#define COMMAND_KEY "COMMAND"
#define PORT_KEY "TPORT"
#define SECUREPORT_KEY "SECUREPORT"
#define PEERAGE_KEY "PEERAGE"
#define UPDATE_KEY "IS_UPDATE"
#define LOCAL_PEER_KEY "LOCAL_PEER"
#define FORIEGN_PEER_KEY "FORIEGN_PEER"
#define SHARED_SECRET_KEY "SHARED_SECRET"
#define CURRENT_STATE_KEY "CURRENT_STATE"
#define SESSION_ID_KEY "SESSION_ID"
#define SESSION_ORIGIN_GUID_KEY "SESSION_ORIGIN_GUID"
#define LOCAL_PUBLIC_KEY "LOCAL_PUBLIC_KEY"
#define FORIEGN_PUBLIC_KEY "FORIEGN_PUBLIC_KEY"
#define SHARED_SECRET_LEN_KEY "SHARED_SECRET_LEN"
#define DELIMITER ":"
extern jnx_hashmap *configuration;
jnx_thread_mutex peer_lock;
static char* guid_string = NULL;
void serialiser_setup(jnx_hashmap *configuration) {
    if(!guid_string) {
        guid_string = jnx_hash_get(configuration,"GUID");
        JNXCHECK(guid_string);
    }
}
size_t serialize_data(char **outbuffer,char *guid,char *command,char *port, char *secure_port,char *peerage) {
    size_t s = 0;
    char buffer[1024];
    bzero(buffer,1024);
    const char *data_frame = "GUID:%s:COMMAND:%s:TPORT:%s:SECUREPORT:%s:PEERAGE:%s:";
    sprintf(buffer,data_frame,guid,command,port,secure_port,peerage);
    s = strlen(buffer);
    *outbuffer = calloc(s,sizeof(char));
    memcpy(*outbuffer,buffer,s);
    JNXCHECK(s != 0);
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
    *outpeer = malloc(sizeof(raw_peer));
    while(t != NULL) {
        if(strcmp(t,GUID_KEY) == 0) {
            char *value = strtok_r(NULL,DELIMITER,&saveptr);
            if(value == NULL) {
                free(*outpeer);
                *outpeer = NULL;
                return S_MALFORMED;
            }
            (*outpeer)->guid = strdup(value);
            //     JNX_LOG(DEFAULT_CONTEXT,"GUID KEY %s with value %s\n",t,(*outpeer)->guid);
        }
        if(strcmp(t, COMMAND_KEY) == 0) {
            char *value = strtok_r(NULL,DELIMITER,&saveptr);
            if(value == NULL) {
                free((*outpeer)->guid);
                free(*outpeer);
                *outpeer = NULL;
                return S_MALFORMED;
            }
            (*outpeer)->command = strdup(value);
            //        JNX_LOG(DEFAULT_CONTEXT,"COMMAND KEY %s with value %s\n",t,(*outpeer)->command);
        }
        if(strcmp(t, PORT_KEY) == 0) {
            char *value = strtok_r(NULL,DELIMITER,&saveptr);
            if(value == NULL) {
                free((*outpeer)->guid);
                free((*outpeer)->command);
                free(*outpeer);
                *outpeer = NULL;
                return S_MALFORMED;
            }
            (*outpeer)->port = strdup(value);
            //          JNX_LOG(DEFAULT_CONTEXT,"PORT KEY %s with value %s\n",t,(*outpeer)->port);
        }
        if(strcmp(t,SECUREPORT_KEY) == 0) {
            char *value = strtok_r(NULL,DELIMITER,&saveptr);
            if(value == NULL) {
                free((*outpeer)->guid);
                free((*outpeer)->command);
                free((*outpeer)->port);
                free(*outpeer);
                *outpeer = NULL;
                return S_MALFORMED;
            }
            (*outpeer)->secure_port = strdup(value);
//            JNX_LOG(DEFAULT_CONTEXT,"SECUREPORT KEY %s with value %s\n",t,(*outpeer)->secure_port);
        }
        if(strcmp(t,PEERAGE_KEY) == 0) {
            char *value = strtok_r(NULL,DELIMITER,&saveptr);
            if(value == NULL) {
                free((*outpeer)->guid);
                free((*outpeer)->command);
                free((*outpeer)->port);
                free(*outpeer);
                *outpeer = NULL;
                return S_MALFORMED;
            }
            (*outpeer)->peerstring = strdup(value);
//            JNX_LOG(DEFAULT_CONTEXT,"PEER KEY %s with value %s\n",t,(*outpeer)->peerstring);
        }
        (*outpeer)->ip = interface_ip;

        t = strtok_r(NULL,DELIMITER,&saveptr);
    }
    return S_OKAY;
}
S_TYPES deserialize_session_data(session **s,char *raw_message, size_t raw_message_len) {
    if(raw_message_len == 0) {
        return S_MALFORMED;
    }
    char *saveptr = NULL;
    char *t = strtok_r(raw_message,DELIMITER,&saveptr);
    *s = malloc(sizeof(session));
    while(t != NULL) {
        if(strcmp(t,LOCAL_PEER_KEY) == 0) {
            char *value = strtok_r(NULL,DELIMITER,&saveptr);
            if(value == NULL) {
                free(*s);
                *s = NULL;
                return S_MALFORMED;
            }
            if(strcmp(value,jnx_hash_get(configuration,"GUID")) == 0) {
                raw_peer *local_peer;
                if(peerstore_check_peer(value,&local_peer)) {
                    (*s)->local_peer = local_peer;
                } else {
                    JNX_LOG(DEFAULT_CONTEXT,"Error retrieving local peer deserialisation data\n");
                }

            } else {
                JNX_LOG(DEFAULT_CONTEXT,"Recieved session being deserialised holds a local peer that is not own,swapping out to foriegn peer\n");
                raw_peer *foriegn_peer;
                if(peerstore_check_peer(value,&foriegn_peer)) {
                    (*s)->foriegn_peer = foriegn_peer;
                } else {
                    JNX_LOG(DEFAULT_CONTEXT,"Error retrieving local copy of foriegn peer!\n");
                }

            }
        }
        if(strcmp(t,FORIEGN_PEER_KEY) == 0) {
            char *value = strtok_r(NULL,DELIMITER,&saveptr);
            if(value == NULL) {
                free(*s);
                *s = NULL;
                return S_MALFORMED;
            }
            if(strcmp(value,jnx_hash_get(configuration,"GUID")) == 0) {
                raw_peer *local_peer;
                if(peerstore_check_peer(value,&local_peer)) {
                    (*s)->local_peer = local_peer;
                } else {
                    JNX_LOG(DEFAULT_CONTEXT,"Error retrieving local peer deserialisation data\n");
                }
            } else {
                raw_peer *foriegn_peer;
                if(peerstore_check_peer(value,&foriegn_peer)) {
                    (*s)->foriegn_peer = foriegn_peer;
                } else {
                    JNX_LOG(DEFAULT_CONTEXT,"Error retrieving local copy of foriegn peer!\n");
                }
            }
        }
        if(strcmp(t,SHARED_SECRET_KEY) == 0) {
            char *value = strtok_r(NULL,DELIMITER,&saveptr);
            if(value == NULL) {
                free(*s);
                *s = NULL;
                return S_MALFORMED;
            }
	 		(*s)->shared_secret = strdup(value);
        }
        if(strcmp(t,CURRENT_STATE_KEY) == 0) {
            int value = atoi(strtok_r(NULL,DELIMITER,&saveptr));
            (*s)->current_state = value;
        }
        if(strcmp(t,SESSION_ID_KEY) == 0) {
            char *value = strtok_r(NULL,DELIMITER,&saveptr);
            if(value == NULL) {
                free((*s)->shared_secret);
                free(*s);
                *s = NULL;
                return S_MALFORMED;
            }
            (*s)->session_id = strdup(value);
        }
        if(strcmp(t,SESSION_ORIGIN_GUID_KEY) == 0) {
            char *value = strtok_r(NULL,DELIMITER,&saveptr);
            if(value == NULL) {
                free((*s)->shared_secret);
                free((*s)->session_id);
                free(*s);
                *s = NULL;
                return S_MALFORMED;
            }
            (*s)->session_origin_guid = strdup(value);
        }
		if(strcmp(t,LOCAL_PUBLIC_KEY) == 0) {
            char *value = strtok_r(NULL,DELIMITER,&saveptr);
            if(value == NULL) {
                free((*s)->shared_secret);
                free((*s)->session_id);
                free(*s);
            	free((*s)->session_origin_guid);
				*s = NULL;
				return S_MALFORMED;
			}
			(*s)->local_public_key = strdup(value);
		}
		if(strcmp(t,FORIEGN_PUBLIC_KEY) == 0) {
            char *value = strtok_r(NULL,DELIMITER,&saveptr);
            if(value == NULL) {
                free((*s)->shared_secret);
                free((*s)->session_id);
                free(*s);
            	free((*s)->session_origin_guid);
				free((*s)->local_public_key);
				*s = NULL;
				return S_MALFORMED;
			}
			(*s)->foriegn_public_key = strdup(value);
		}
		if(strcmp(t,SHARED_SECRET_LEN_KEY) == 0) {
            char *value = strtok_r(NULL,DELIMITER,&saveptr);
            if(value == NULL) {
                free((*s)->shared_secret);
                free((*s)->session_id);
                free(*s);
            	free((*s)->session_origin_guid);
				free((*s)->local_public_key);
				free((*s)->foriegn_public_key);
				*s = NULL;
				return S_MALFORMED;
			}
			(*s)->shared_secret_len = atoi(value);
		} 
		
		t = strtok_r(NULL,DELIMITER,&saveptr);
    }
    return S_OKAY;
}

size_t serialize_session_data(char **outbuffer,session *s) {
    size_t len = 0;
    char *buffer = malloc(sizeof(char) * 6000);
    const char *session_frame = "LOCAL_PEER:%s:FORIEGN_PEER:%s:SHARED_SECRET:%s:CURRENT_STATE:%d:SESSION_ID:%s:SESSION_ORIGIN_GUID:%s:LOCAL_PUBLIC_KEY:%s:FORIEGN_PUBLIC_KEY:%s:SHARED_SECRET_LEN:%d:";

    len = sprintf(buffer,session_frame,s->local_peer->guid,s->foriegn_peer->guid,
                  s->shared_secret,s->current_state,s->session_id,s->session_origin_guid,
				  s->local_public_key,s->foriegn_public_key,s->shared_secret_len);

    *outbuffer = buffer;
    return len;
}

