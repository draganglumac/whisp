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
#include <jnxc_headers/jnxbase64.h>
#include <openssl/des.h>
#include "des.h"

char *des_encrypt(char *key, char *msg, size_t size) {
	static char *res;
	int n = 0;


	DES_cblock key2;
	DES_key_schedule schedule;

	res = JNX_MEM_MALLOC(size);
	bzero(res,size);
	memcpy(key2,key,8);
	DES_set_odd_parity(&key2);
	DES_set_key_checked(&key2, &schedule);

	DES_cfb64_encrypt((unsigned char *) msg,(unsigned char *)res, size, &schedule,
			&key2,&n,DES_ENCRYPT);

	size_t elen;
	char *encoded = jnx_base64_encode(res,size,&elen);
	JNX_MEM_FREE(res);
	return encoded;
}
char *des_decrypt(char *key, char *msg, size_t size) {
	
	static char *res;
	int n = 0;
	size_t dlen;
	msg = jnx_base64_decode(msg,strlen(msg),&dlen);
	DES_cblock key2;
	DES_key_schedule schedule;
	
	res = malloc(size);
	bzero(res,size);
	memcpy(key2,key,8);
	DES_set_odd_parity(&key2);
	DES_set_key_checked(&key2, &schedule);

	DES_cfb64_encrypt((unsigned char *)msg, (unsigned char *)res,
			size, &schedule, &key2, &n, DES_DECRYPT);
	
	return res;
}
