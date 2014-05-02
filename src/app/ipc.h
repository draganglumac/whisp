/*
 * =====================================================================================
 *
 *       Filename:  ipc.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  04/14/14 08:32:38
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jonesax (), 
 *   Organization:  
 *
 * =====================================================================================
 */
#ifndef __IPC_H__
#define __IPC_H__
#include <jnxc_headers/jnxunixsocket.h>
#include <jnxc_headers/jnxhash.h>
void ipc_setup(jnx_hashmap *configuration);
void ipc_teardown();
#endif
