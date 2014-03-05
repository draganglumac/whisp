/*
 * =====================================================================================
 *
 *       Filename:  protocol.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  03/03/14 18:09:10
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jonesax (), 
 *   Organization:  
 *
 * =====================================================================================
 */
#ifndef __SERIALISER_H__
#define __SERIALISER_H__
#include <jnxc_headers/jnxhash.h>


void serialiser_setup(jnx_hashmap *configuration); 

size_t get_pulse_data(char **data); 


#endif
