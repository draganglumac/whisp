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
#include <uuid/uuid.h>
#define CONF "conf/settings.conf"
#include <unistd.h>
#include "protocol.h"
#include "comms.h"
void print_config(jnx_hashmap *config)
{
	const char **keys;
	int num = jnx_hash_get_keys(config,&keys);
	int x;
	for(x=0;x<num;++x)
	{
		printf("KEY:%s VALUE:%s\n",keys[x],jnx_hash_get(config,keys[x]));
	}
}
void resolve_interface_address(jnx_hashmap *config)
{
	char *af = jnx_hash_get(config,"ADDFAMILY");
	assert(af);
	char *interface = jnx_hash_get(config,"INTERFACE");
	assert(interface);

	unsigned int family = AF_INET;

	if(strcmp(af,"AF_INET6") == 0)
	{
		family = AF_INET6;
	}
	char *ip = jnx_network_get_ip(interface,family);
	assert(ip);
	printf("Local IP %s for interface %s addressfamily %s\n",ip,interface,af);
	jnx_hash_put(config,"IP",ip);
}
void generate_guid(jnx_hashmap *config)
{
	uuid_t g;
	uuid_generate_random(g);
	int i;
	char buffer[24] = {};

	for(i=0;i<16;++i)
	{
		char micro[sizeof(unsigned int)];
		sprintf(micro,"%u",g[i]);
		strcat(buffer,micro);
	}
	printf("GUID %s\n",buffer);
	assert(buffer);
	jnx_hash_put(config,"GUID",strdup(buffer));
}
int main(int argc, char **argv)
{
	jnx_mem_print_to_file("logs/mem.file");
	
	jnx_hashmap *configuration = jnx_file_read_kvp(CONF,1024,"=");
	
	assert(configuration);
	
	print_config(configuration);
	
	resolve_interface_address(configuration);	
	
	generate_guid(configuration);

	comms_setup(configuration);

	protocol_setup(configuration);

	//Initial setup done
	comms_start();	
	return 0;
}
