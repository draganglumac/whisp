/*
 * =====================================================================================
 *
 *       Filename:  discovery.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  03/03/14 18:06:58
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jonesax (), 
 *   Organization:  
 *
 * =====================================================================================
 */

#include <jnxc_headers/jnxnetwork.h>
#include <jnxc_headers/jnxsocket.h>
#include <jnxc_headers/jnxhash.h>

void discovery_setup(jnx_hashmap *configuration);
void discovery_start();
