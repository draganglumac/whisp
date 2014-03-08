/*
 * =====================================================================================
 *
 *       Filename:  test_encryption.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  08/03/14 13:36:43
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Dragan Glumac (draganglumac), dragan.glumac@gmail.com
 *   Organization:  
 *
 * =====================================================================================
 */
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <jnxc_headers/jnxterm.h>
#include "encryption.h"

void test_encrypt_public_decrypt_private() {	
	jnx_term_printf_in_color(JNX_COL_WHITE, "\ttest_encrypt_public_decrypt_private(): ");

	RSA *keypair = generate_key(1024);
	RSA *keypair2 = generate_key(1024);
	char *plaintext = "Hello world!";
	size_t cipherlength, decryptlength;
	char *ciphertext = encrypt_message(keypair, plaintext, &cipherlength);
	char *decrypted = decrypt_message(keypair, ciphertext, cipherlength, &decryptlength);

	assert(strncmp(plaintext, decrypted, decryptlength) == 0);

	free(ciphertext);
	free(decrypted);

	ciphertext = encrypt_message(keypair2, plaintext, &cipherlength);
	decrypted = decrypt_message(keypair2, ciphertext, cipherlength, &decryptlength);

	assert(strncmp(plaintext, decrypted, decryptlength) == 0);

	free(ciphertext);
	free(decrypted);
	free_key(keypair);
	free_key(keypair2);

	jnx_term_printf_in_color(JNX_COL_GREEN, "Pass\n");
}

void test_encrypt_with_string_decrypt_with_key() {
	jnx_term_printf_in_color(JNX_COL_WHITE, "\ttest_encrypt_with_string_decrypt_with_key(): ");

	RSA *keypair = generate_key(1024);
	char *public = key_to_string(keypair, PUBLIC);
	RSA *publicrsa = string_to_key(public, PUBLIC);

	size_t cipherlength, decryptlength;
	char *plaintext = "Hello world!";
	char *ciphertext = encrypt_message(publicrsa, plaintext, &cipherlength);
	char *decrypted = decrypt_message(keypair, ciphertext, cipherlength, &decryptlength);
//	char *ciphertext = encrypt_message(keypair, plaintext, &cipherlength);
//	char *decrypted = decrypt_message(keypair, ciphertext, cipherlength, &decryptlength);

	assert(strncmp(plaintext, decrypted, decryptlength) == 0);

	free(public);
	// NOTE: For some reason freeing public string results in double-free
	// so it must be that somewhere else it gets freed. To investigate!
	free(decrypted);
	free(ciphertext);
	free_key(publicrsa);
	free_key(keypair);

	jnx_term_printf_in_color(JNX_COL_GREEN, "Pass\n");
}

int main() {
	global_initialise_openSSL();
	
	jnx_term_printf_in_color(JNX_COL_WHITE, "Testing encryption.c functions:\n");
	
	test_encrypt_with_string_decrypt_with_key();
	test_encrypt_public_decrypt_private();
	
	jnx_term_printf_in_color(JNX_COL_WHITE, "OK.\n");
	
	global_cleanup_openSSL();
	return 0;
}
