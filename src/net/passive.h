/*
 * =====================================================================================
 *
 *       Filename:  passive.h
 *
 *    Description:  passive tcp listener, starts authentication process
 *
 *        Version:  1.0
 *        Created:  03/11/14 07:15:33
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jonesax (), 
 *   Organization:  
 *
 * =====================================================================================
 */
#ifndef __PASSIVE_H__
#define __PASSIVE_H__
#include "authentication.h"
#include "peerstore.h"
void* passive_listener_start(void *args);

void passive_listener_setup(jnx_hashmap *configuration);

void passive_listener_stop();

#endif
