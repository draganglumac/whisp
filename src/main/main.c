/*
 * =====================================================================================
 *
 *       Filename:  main.c
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  03/03/14 18:03:34
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jonesax (),
 *   Organization:
 *
 * =====================================================================================
 */
#include <stdlib.h>
#include <stdio.h>

#include <jnxc_headers/jnxfile.h>
#include <jnxc_headers/jnxlog.h>
#include <jnxc_headers/jnxthread.h>

#include "app.h"
#include "userinput.h"
#include "discovery.h"
#include "connectioncontrol.h"

int main(int argc, char **argv) {
	jnx_mem_print_to_file("logs/mem.file");
	jnx_log_file_setup("logs/current.log");
	jnx_hashmap *configuration = jnx_file_read_kvp(CONFIG_PATH, 1024, "=");

	assert(configuration);

	print_welcome();
	//    print_config(configuration);

	resolve_interface_address(configuration);

	generate_ports(configuration);

	generate_guid(configuration);

	discovery_setup(configuration);

	serialiser_setup(configuration);

	// Calling it here before we use any of the OpenSSL API calls
	global_initialise_openSSL();
	
	jnx_thread_create_disposable(discovery_start,NULL);

	jnx_thread_create_disposable(connectioncontrol_setup,configuration);

	user_input_loop();

	// Cleaning up OpenSSL infrastructure for the process before return.
	global_cleanup_openSSL();

	jnx_hash_destroy(configuration);

	return 0;
}