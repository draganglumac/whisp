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
#include <jnxc_headers/jnxlog.h>
#include <jnxc_headers/jnxthread.h>
#include "local_macro.h"
#include "app.h"
#include "userinput.h"
#include "discovery.h"
extern jnx_hashmap *configuration;
void signal_callback(int sig) {

    JNX_LOGC("Tearing down discovery...\n");
    discovery_teardown();
    JNX_LOGC("Stopping passive listener...\n");
    passive_listener_stop();
    JNX_LOGC("Destroying hashmap...\n");
    jnx_hash_destroy(configuration);
    exit(0);
}
int main(int argc, char **argv) {

    signal(SIGKILL,signal_callback);
    signal(SIGINT,signal_callback);
    signal(SIGTERM,signal_callback);
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

    passive_listener_setup(configuration);

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

