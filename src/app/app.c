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
#include <stdlib.h>
#include <stdio.h>
#include <uuid/uuid.h>
#include <unistd.h>

#include <jnxc_headers/jnxnetwork.h>
#include "app.h"
#include "serialization.h"
#include "discovery.h"

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

        char *ip;
        jnx_network_interface_to_string(&ip,interface, family);
	
        
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
	return (min + base_random % (max - min + 1));
}
void generate_ports(jnx_hashmap *config) {

	int r = random_in_range(49152,65535);
	char *buffer = malloc(56);
	sprintf(buffer,"%d",r);
	printf("Adding tcp port %s to map\n",buffer);
	jnx_hash_put(config,"TPORT",buffer);

	r = random_in_range(41390,51952);
	char *buffertwo = malloc(56);
	sprintf(buffertwo,"%d",r);
	printf("Adding secure socket port %s to map\n",buffertwo);
	jnx_hash_put(config,"SECUREPORT",buffertwo);
}

// Code taken straight from OpenSSL libcrypto wiki page
// http://wiki.openssl.org/index.php/Libcrypto_API
void global_initialise_openSSL() {
	// Load the human readable error strings for libcrypto
	ERR_load_crypto_strings();

	// Load all digest and cipher algorithms
	OpenSSL_add_all_algorithms();

	// Load config file, and other important initialisation
	OPENSSL_config(NULL);
}
void global_cleanup_openSSL() {
	// Removes all digests and ciphers
	EVP_cleanup();

	// Remove error strings
	ERR_free_strings();
}
