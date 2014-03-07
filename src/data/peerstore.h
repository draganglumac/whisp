/*
 * =====================================================================================
 *
 *       Filename:  peerstore.h
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  03/04/14 18:31:09
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jonesax (),
 *   Organization:
 *
 * =====================================================================================
 */
#ifndef __PEERSTORE_H__
#define __PEERSTORE_H__
#include <jnxc_headers/jnxbtree.h>
#include <openssl/rsa.h>
#include <openssl/pem.h>
typedef struct raw_peer
{
	int has_public_key;
	char *publickey;
	char *guid;
	char *command;
	char *ip;
	char *peerstring;
} raw_peer;

int peerstore_check_peer(char *guid,raw_peer **inpeer);

char* peerstore_get_peerstring();

int peerstore_add_peer(raw_peer *rp);



#endif


