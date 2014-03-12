/*
 * =====================================================================================
 *
 *       Filename:  send_await.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  03/12/14 07:34:01
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jonesax (), 
 *   Organization:  
 *
 * =====================================================================================
 */
#ifndef __SEND_AWAIT_H__
#define __SEND_AWAIT_H__

char *send_and_await(char *host,char *port, char *message,
		char *rport,float timeout);

#endif
