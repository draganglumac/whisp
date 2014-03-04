/*
 * =====================================================================================
 *
 *       Filename:  protocol.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  03/03/14 18:09:36
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jonesax (), 
 *   Organization:  
 *
 * =====================================================================================
 */
#include <jnxc_headers/jnxmem.h>
#include <stdlib.h>
#include "protocol.h"

static char* guid_string = NULL;

void protocol_setup(jnx_hashmap *configuration)
{
	if(!guid_string)
	{
		guid_string = jnx_hash_get(configuration,"GUID");
		assert(guid_string);
	}
}	
size_t protocol_get_multicast_pulse_data(char **data)
{
	size_t s = 0;

	char buffer[1024];
	bzero(buffer,1024);
	const char *data_frame = "[%s][%s]";
	sprintf(buffer,data_frame,guid_string,"PULSE");
	
	s = strlen(buffer);
	
	*data = JNX_MEM_CALLOC(s,sizeof(char));
	memcpy(*data,buffer,s);
	assert(s != 0);
	return s;
}	
