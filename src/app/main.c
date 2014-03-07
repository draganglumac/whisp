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
#include <jnxc_headers/jnxnetwork.h>
#include <jnxc_headers/jnxmem.h>
#include <jnxc_headers/jnxlog.h>
#include <jnxc_headers/jnxthread.h>
#include <uuid/uuid.h>
#define CONFIG_PATH "conf/settings.conf"
#include <unistd.h>
#include "serialization.h"
#include "userinput.h"
#include "discovery.h"
#include "connectioncontrol.h"
void print_welcome() {
	system("clear");	
	printf("==================================\n");
	printf("=         Whisp(pre alpha)       =\n");
	printf("=                                =\n");
	printf("=     P2P decentralised comms    =\n");
	printf("==================================\n");
}
void print_config(jnx_hashmap *config) {
    const char **keys;
    int num = jnx_hash_get_keys(config, &keys);
    int x;
    for (x = 0; x < num; ++x) {
        printf("KEY:%s VALUE:%s\n", keys[x], jnx_hash_get(config, keys[x]));
    }
}
void resolve_interface_address(jnx_hashmap *config) {
    char *af = jnx_hash_get(config, "ADDFAMILY");
    assert(af);
    char *interface = jnx_hash_get(config, "INTERFACE");
    assert(interface);

    unsigned int family = AF_INET;

    if (strcmp(af, "AF_INET6") == 0) {
        family = AF_INET6;
    }
    char *ip = jnx_network_get_ip(interface, family);
    assert(ip);
    printf("Local IP %s for interface %s addressfamily %s\n", ip, interface, af);
    jnx_hash_put(config, "IP", ip);
}
void generate_guid(jnx_hashmap *config) {
    uuid_t g;
    uuid_generate_random(g);
    int i;
    char buffer[24] = {};

    for (i = 0; i < 16; ++i) {
        char micro[sizeof(unsigned int)];
        sprintf(micro, "%u", g[i]);
        strcat(buffer, micro);
    }
    printf("GUID %s\n", buffer);
    assert(buffer);
    jnx_hash_put(config, "GUID", strdup(buffer));
}
int random_in_range(unsigned int min, unsigned int max) {
	
	srand(time(NULL));	
	int base_random = rand();
	if(RAND_MAX == base_random) return random_in_range(min,max);
	int range = max - min,
		remainder = RAND_MAX % range,
		bucket = RAND_MAX / range;
	if(base_random < RAND_MAX - remainder) {
		return min + base_random / bucket;
	} else {
		return random_in_range(min,max);
	}
}
void generate_ports(jnx_hashmap *config) {

	int r = random_in_range(49152,65535);
	char *buffer = JNX_MEM_MALLOC(56);
	sprintf(buffer,"%d",r);
	printf("Adding tcp port %s to map\n",buffer);
	jnx_hash_put(config,"TPORT",buffer);

	r = random_in_range(41390,51952);
	char *buffertwo = JNX_MEM_MALLOC(56);
	sprintf(buffertwo,"%d",r);
	printf("Adding secure socket port %s to map\n",buffertwo);
	jnx_hash_put(config,"SECUREPORT",buffertwo);
}
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

    jnx_thread_create_disposable(discovery_start,NULL);
	
	jnx_thread_create_disposable(connectioncontrol_setup,configuration);

    user_input_loop();

    return 0;
}


