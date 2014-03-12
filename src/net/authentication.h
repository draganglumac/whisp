/*
 * =====================================================================================
 *
 *       Filename:  authentication.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  03/10/14 18:34:36
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jonesax (), 
 *   Organization:  
 *
 * =====================================================================================
 */
#ifndef __AUTHENTICATION_H__
#define __AUTHENTICATION_H__
#include "sessioncontrol.h"
#include <jnxc_headers/jnxhash.h>

void authentication_start_with_session(session *s);

#endif
