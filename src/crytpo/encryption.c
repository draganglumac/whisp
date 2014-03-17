/*
 * =====================================================================================
 *
 *       Filename:  encryption.c
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  03/06/14 16:23:08
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jonesax (),
 *   Organization:
 *
 * =====================================================================================
 */
#include <stdlib.h>
#include "encryption.h"
#include <jnxc_headers/jnxmem.h>
#include <jnxc_headers/jnxbase64.h>
#include <jnxc_headers/jnxlog.h>
RSA *generate_key(size_t len) {
    srand(time(0));
    return RSA_generate_key(len,PUB_EXP,NULL,NULL);
}
void free_key(RSA *key) {
    if (key != NULL)
        RSA_free(key);
}
char *key_to_string(RSA *keypair,key_type type) {
    BIO *key = BIO_new(BIO_s_mem());
    switch(type) {
    case PUBLIC:
        PEM_write_bio_RSAPublicKey(key,keypair);
        break;

    case PRIVATE:
        PEM_write_bio_RSAPrivateKey(key,keypair,NULL,NULL,0,NULL,NULL);
        break;
    }
    size_t len = BIO_pending(key);
    char *skey = JNX_MEM_MALLOC(len + 1);
    size_t read = BIO_read(key,skey,len);
    skey[read + 1] = '\0';
    BIO_free(key);
    return skey;
}
RSA *string_to_key(char *string, key_type type) {
    BIO *key = BIO_new_mem_buf((void*)string,strlen(string));
    RSA *rsa = RSA_new();
    switch(type) {
    case PUBLIC:
        PEM_read_bio_RSAPublicKey(key,&rsa,0,NULL);
        break;
    }
    BIO_free(key);
    return rsa;
}
char* encrypt_message(RSA *keypair, char *message, size_t *out_encrypted_len) {
    assert(keypair);
    assert(message);
    char *encrypt = JNX_MEM_MALLOC(RSA_size(keypair));
    bzero(encrypt,RSA_size(keypair));
    char *err = JNX_MEM_MALLOC(130);

    ///Removing +1 on strlen seems to have an effect...
    if((*out_encrypted_len = RSA_public_encrypt(strlen(message) +1,(unsigned char*)message,
                             (unsigned char*)encrypt,keypair,RSA_PKCS1_OAEP_PADDING)) == -1) {
        ERR_load_crypto_strings();
        ERR_error_string(ERR_get_error(),err);
        printf("%s\n",err);
        JNX_MEM_FREE(err);
        return NULL;
    }
    JNX_MEM_FREE(err);
    size_t elen;
    char *encoded = jnx_base64_encode(encrypt,*out_encrypted_len,&elen);
    free(encrypt);
    return encoded;
}
char *decrypt_message(RSA *keypair, char *encrypted_message, size_t encrypted_message_len, size_t *out_decrypted_len) {
    assert(keypair);
    assert(encrypted_message);
    assert(encrypted_message_len);
    size_t dlen;
    encrypted_message = jnx_base64_decode(encrypted_message,strlen(encrypted_message),&dlen);
    char *decrypt = JNX_MEM_MALLOC(RSA_size(keypair));
    bzero(decrypt,RSA_size(keypair));
    char *err = JNX_MEM_MALLOC(130);
    if((*out_decrypted_len = RSA_private_decrypt(encrypted_message_len,(unsigned char*)encrypted_message,(unsigned char*)
                             decrypt,keypair,RSA_PKCS1_OAEP_PADDING)) == -1) {
        ERR_load_crypto_strings();
        ERR_error_string(ERR_get_error(),err);
        printf("%s\n",err);
        JNX_MEM_FREE(err);
        return NULL;
    }
    JNX_MEM_FREE(err);
    return decrypt;
}
