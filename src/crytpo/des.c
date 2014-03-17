/*
 * =====================================================================================
 *
 *       Filename:  des.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  03/17/14 14:30:34
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jonesax (), 
 *   Organization:  
 *
 * =====================================================================================
 */
#include <stdlib.h>
#include <jnxc_headers/jnxmem.h>
#include <openssl/des.h>
#include "des.h"

char *des_encrypt(char *key, char *msg, size_t size) {
	static char *res;
	int n = 0;

	DES_cblock key2;
	DES_key_schedule schedule;

	res = JNX_MEM_MALLOC(size);
	memcpy(key2,key,8);
	DES_set_odd_parity(&key2);
	DES_set_key_checked(&key2, &schedule);

	DES_cfb64_encrypt((unsigned char *) msg,(unsigned char *)res, size, &schedule,
			&key2,&n,DES_ENCRYPT);

	return res;
}
char *des_decrypt(char *key, char *msg, size_t size) {

}
