/*
 * =====================================================================================
 *
 *       Filename:  protocol.h
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  03/03/14 18:09:10
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jonesax (),
 *   Organization:
 *
 * =====================================================================================
 */
#ifndef __SERIALISER_H__
#define __SERIALISER_H__
#include <jnxc_headers/jnxhash.h>
#include "peerstore.h"
typedef enum S_TYPES
{
	S_OKAY, S_GENERAL_ERROR, S_UNKNOWN, S_MALFORMED
} S_TYPES;

void serializer_setup(jnx_hashmap *configuration);

S_TYPES deserialize_data(raw_peer **outpeer, char *raw_message, size_t raw_message_len, char *interface_ip);

size_t serialize_data(char **outbuffer,char *guid,char *command,char *port, char *secure_port,char *peerage);

#endif

