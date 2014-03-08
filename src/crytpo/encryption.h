/*
 * =====================================================================================
 *
 *       Filename:  encryption.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  03/06/14 16:22:49
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jonesax (), 
 *   Organization:  
 *
 * =====================================================================================
 */
#ifndef __ENCRYPTION_H__
#define __ENCRYPTION_H__
#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <openssl/err.h>
#include <string.h>
#define PUB_EXP 3
typedef enum key_type { PUBLIC, PRIVATE } key_type;



RSA *generate_key(size_t len);

void free_key(RSA *key);

RSA *string_to_key(char *string, key_type type);

char *key_to_string(RSA *keypair,key_type type);

char* encrypt_message(RSA *keypair, char *message, size_t *out_encrypted_len);

char *decrypt_message(RSA *keypair, char *encrypted_message, size_t encrypted_message_len, size_t *out_decrypted_len);


#endif
