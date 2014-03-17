/*
 * =====================================================================================
 *
 *       Filename:  des.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  03/17/14 14:30:16
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jonesax (), 
 *   Organization:  
 *
 * =====================================================================================
 */
#ifndef __DES_H__
#define __DES_H__
char *des_encrypt(char *key, char *msg, size_t size);
char *des_decrypt(char *key, char *msg, size_t size);
#endif
