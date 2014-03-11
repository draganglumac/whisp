/*
 * =====================================================================================
 *
 *       Filename:  local_macro.h
 *

 *
 *        Version:  1.0
 *        Created:  03/11/14 07:43:08
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jonesax (), 
 *   Organization:  
 *
 * =====================================================================================
 */
#ifndef __LOCAL_MACRO_H__
#define __LOCAL_MACRO_H__
#include <jnxc_headers/jnxthread.h>
#include <jnxc_headers/jnxsocket.h>

#define ASYNC_START(X,Y)\
	jnx_thread_create_disposable(X,Y);

//NAME,HOST, PORT, MESSAGE
#define QUICK_TCP(N,X,Y,Z)\
	jnx_socket *N = jnx_socket_tcp_create(AF_INET); \
	jnx_socket_tcp_send(N,X,Y,Z,strlen(Z)); \
	jnx_socket_destroy(&N);
#endif
