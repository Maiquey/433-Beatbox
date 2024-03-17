// Joystick module 
// Part of the Hardware Abstraction Layer (HAL)
// From Assignment 1 with some changes
// initializes joystick and supports press Detection

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
int joystick_getJoyStickPressEpoll(long long timeout);
int joystick_getJoyStickPress(void);
bool joystick_checkIfPressed(void);
void joystick_waitForRelease(void);
bool joystick_isPressedUpDown(void);
bool joystick_isPressedLeftRight(void);
void joystick_cleanup(void);

#endif