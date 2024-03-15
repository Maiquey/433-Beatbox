// Joystick module 
// Part of the Hardware Abstraction Layer (HAL) 

#ifndef _JOYSTICK_H_
#define _JOYSTICK_H_

#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include <errno.h>		// Errors
#include <string.h>

#include <sys/epoll.h>  // for epoll()
#include <fcntl.h>      // for open()
#include <unistd.h>     // for close()

void joystick_init(void);
int joystick_getJoyStickPress(long long timeout);
bool joystick_checkIfPressed(void);
void joystick_waitForRelease(void);
bool joystick_isPressedUpDown(void);
bool joystick_isPressedLeftRight(void);

#endif