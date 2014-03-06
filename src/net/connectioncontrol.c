/*
 * =====================================================================================
 *
 *       Filename:  connectioncontrol.c
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  03/06/14 19:28:03
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
#include <unistd.h>
#include "connectioncontrol.h"
#include "discovery.h"
static int connecting = 0;


int connectioncontrol_isconnected(void) {

    return connecting;
}
void* connectioncontrol_start(void *args) {
    printf("Starting connection control\n");
	thread_data *thr = (thread_data*)args;






	sleep(5);
	connecting = 1;

	return 0;
}


