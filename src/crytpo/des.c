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
#include <jnxc_headers/jnxencoder.h>
#include <openssl/des.h>
#include <string.h>
#include "des.h"

char *des_encrypt(char *key, char *msg, size_t size) {
	static char *res;
	int n = 0;


	DES_cblock key2;
	DES_key_schedule schedule;

	res = malloc(size);
	bzero(res,size);
	memcpy(key2,key,8);
	DES_set_odd_parity(&key2);
	DES_set_key_checked(&key2, &schedule);

	DES_cfb64_encrypt((unsigned char *) msg,(unsigned char *)res, size, &schedule,
			&key2,&n,DES_ENCRYPT);

	size_t elen;
	
        jnx_encoder *e = jnx_encoder_create();
        
        char *encoded = jnx_encoder_b64_encode(e,res,size,&elen);
	free(res);
	jnx_encoder_destroy(&e);

        return encoded;
}
char *des_decrypt(char *key, char *msg, size_t size) {
	
	static char *res;
	int n = 0;
	size_t dlen;
	
        jnx_encoder *e = jnx_encoder_create();
        msg = jnx_encoder_b64_decode(e,msg,size,&dlen);
        jnx_encoder_destroy(&e);

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
