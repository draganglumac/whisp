/*
 * =====================================================================================
 *
 *       Filename:  peerlist.h
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
#ifndef __PEERLIST_H__
#define __PEERLIST_H__

int peerlist_check_peer(char *guid);

int peerlist_add_peer(char *raw);

int peerlist_remove_peer(char *guid);

int peerlist_update_peer(char *guid);

#endif
