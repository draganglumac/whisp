/*
 * =====================================================================================
 *
 *       Filename:  connectioncontrol.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  03/06/14 19:27:30
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jonesax (), 
 *   Organization:  
 *
 * =====================================================================================
 */
#ifndef __CONNECTION_CONTROL_H__
#define __CONNECTION_CONTROL_H__

void* connectioncontrol_setup(void *args); 
void* connectioncontrol_start(void *args);
int  connectioncontrol_isconnected(void);
#endif
