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

char *generate_log();
void print_welcome();

void print_config(jnx_hashmap *config); 

void resolve_interface_address(jnx_hashmap *config);

void generate_guid(jnx_hashmap *config);

void generate_ports(jnx_hashmap *config);

// This function must be called once only per process
// before any other OpenSSL API function is called.
// Best done at the beginning of main.
void global_initialise_openSSL();

// This function should be called once only per process
// when OpenSSL API functions are no longer needed.
// Best done just before main returns.
void global_cleanup_openSSL();
#endif // __APP_H__
