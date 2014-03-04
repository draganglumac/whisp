/*
 * =====================================================================================
 *
 *       Filename:  comms.h
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
typedef enum state { SEARCH, DISCOVERY, MESSAGE, EXITING } state;

void hint_new_current_state(state hint);
state comms_current_state();
void comms_setup(jnx_hashmap *configuration);
void comms_start();
