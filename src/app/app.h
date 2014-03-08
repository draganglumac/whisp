/*
 * =====================================================================================
 *
 *       Filename:  app.c
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  08/03/14 10:30:34
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  draganglumac (),
 *   Organization:
 *
 * =====================================================================================
 */
#ifndef __APP_H__
#define __APP_H__

#include <jnxc_headers/jnxhash.h>

#define CONFIG_PATH "conf/settings.conf"

void print_welcome();

void print_config(jnx_hashmap *config); 

void resolve_interface_address(jnx_hashmap *config);

void generate_guid(jnx_hashmap *config);

void generate_ports(jnx_hashmap *config);

#endif // __APP_H__
