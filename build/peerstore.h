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

typedef struct raw_peer{
	char *guid;
	char *command; 
	char *ip; 
	char *peerstring;
}raw_peer;

int peerstore_check_peer(char *guid);

char* peerstore_get_peerstring();

int peerstore_add_peer(raw_peer *rp);

int peerstore_remove_peer(char *guid);

int peerstore_update_peer(char *guid);


#endif

