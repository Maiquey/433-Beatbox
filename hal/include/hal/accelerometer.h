#ifndef _ACCELEROMETER_H_
#define _ACCELEROMETER_H_

#include "hal/timing.h"
#include <assert.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include <pthread.h>
#include <string.h>

void accelerometer_init(void);
void accelerometer_cleanup(void);
// Returns dynamically allocated array of out values (must be freed by caller)
unsigned char* accelerometer_readOutVals(void);

#endif