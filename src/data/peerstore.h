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
	char *guid;
	char *command;
	char *ip;
	char *port;
	char *secure_port;
	char *peerstring;
} raw_peer;

size_t peerstore_get_count();

int peerstore_check_peer(char *guid,raw_peer **inpeer);

void peerstore_update_peer(raw_peer *new_peer,raw_peer *updated_peer);

void peerstore_print_peers();

int peerstore_add_peer(raw_peer *rp);

void peerstore_delete_peer(raw_peer *rp);

void peerstore_destroy();
#endif
