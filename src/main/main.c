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
#include <signal.h>
#include <jnxc_headers/jnxfile.h>
#include <jnxc_headers/jnxlist.h>
#include <jnxc_headers/jnxmem.h>
#include <jnxc_headers/jnxlog.h>
#include <jnxc_headers/jnxterm.h>
#include <jnxc_headers/jnxthread.h>
#include "local_macro.h"
#include "app.h"
#include "secure_channel.h"
#include "userinput.h"
#include "discovery.h"
#include "ipc.h"
extern jnx_hashmap *configuration;
void signal_callback(int sig) {

	switch(sig) {
		case SIGINT:
			jnx_term_printf_in_color(JNX_COL_CYAN,"\tExiting active session(s)\n");
			jnx_list *session_list = session_get_connected_sessions();
			if(session_list) {

				jnx_term_printf_in_color(JNX_COL_CYAN,"\tShutting down %d session(s)\n",
						session_list->counter);

				while(session_list->head) {
					session *s = session_list->head->_data;
					session_shutdown(s);
					session_list->head = session_list->head->next_node;
				}	
				jnx_list_destroy(&session_list);
			}	
			break;
		default:
			JNX_LOGC(JLOG_CRITICAL,"Tearing down discovery...\n");
			discovery_teardown();
			JNX_LOGC(JLOG_CRITICAL,"Stopping passive listener...\n");
			passive_listener_stop();
			JNX_LOGC(JLOG_CRITICAL,"Killing ipc listener...\n");
			ipc_teardown();	
			JNX_LOGC(JLOG_CRITICAL,"Destroying hashmap...\n");
			jnx_hash_destroy(configuration);
			exit(0);
			break;
	}
}
char *generate_log_path() {
	char buffer[2048];
	bzero(buffer,2048);

	char *path;
	jnx_file_mktempdir("/tmp",&path);
	if(path == NULL) {
		printf("Error creating temporary directory!\n");
			return NULL;
	}
	sprintf(buffer,"%s.log",path);
	free(path);
	return strdup(buffer);
}
int main(int argc, char **argv) {

	signal(SIGKILL,signal_callback);
	signal(SIGINT,signal_callback);
	signal(SIGTERM,signal_callback);


	jnx_hashmap *configuration = jnx_file_read_kvp(CONFIG_PATH, 1024, "=");

	assert(configuration);

    print_welcome();
	//    print_config(configuration);

	resolve_interface_address(configuration);

	generate_ports(configuration);

	generate_guid(configuration);

	ipc_setup(configuration);

	discovery_setup(configuration);

	passive_listener_setup(configuration);

	secure_channel_setup(configuration);

	serialiser_setup(configuration);

	// Calling it here before we use any of the OpenSSL API calls
	global_initialise_openSSL();

	jnx_thread_create_disposable(discovery_start,NULL);

	user_input_loop();

	// Cleaning up OpenSSL infrastructure for the process before return.
	global_cleanup_openSSL();

	jnx_hash_destroy(configuration);

	return 0;
}


