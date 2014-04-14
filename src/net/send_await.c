/*
 * =====================================================================================
 *
 *       Filename:  send_await.c
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  03/12/14 07:35:02
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jonesax (),
 *   Organization:
 *
 * =====================================================================================
 */
#include <stdio.h>
#include <assert.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include "send_await.h"
#include <jnxc_headers/jnxsocket.h>
#include <jnxc_headers/jnxthread.h>
typedef struct thrdata {
	int max_con;
	char *port;
	jnx_socket *s;
	tcp_socket_listener_callback c;
}thrdata;
static uint8_t *retval = NULL;
int await_reply(uint8_t *msg, size_t len, char *ip) {

	retval = msg;
	return 1;
}
void *start_listen(void *args) {
	thrdata *t = args;
	printf("Awaiting...\n");
	jnx_socket_tcp_listen(t->s,t->port,t->max_con,t->c);
	free(t);
}
char *send_and_await(char *host,char *port, char *message,
		char *rport,float timeout) {
	assert(host);
	assert(port);
	assert(message);
	assert(rport);
	assert(timeout);

	jnx_socket *out = jnx_socket_tcp_create(AF_INET);
	jnx_socket *in = jnx_socket_tcp_create(AF_INET);

	thrdata *r = malloc(sizeof(thrdata));
	r->max_con = 1;
	r->port = port;
	r->s = in;
	r->c = await_reply;

	jnx_thread_create_disposable(start_listen,r);
	sleep(.5);

	printf("Sending...\n");	

	jnx_socket_tcp_send(out,host,rport,message,strlen(message));

	jnx_socket_destroy(&out);
	clock_t st = clock();
	float current_time = 0;

	while(current_time < timeout && retval == NULL) {

		clock_t e = clock();
		current_time = (e - st) / CLOCKS_PER_SEC;

	}
	jnx_socket_destroy(&in);

	if(current_time < timeout) {
		return retval;
	}
	return NULL;
}
