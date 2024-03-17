// High-level module for sampling user input on the zen cape
// spawns two threads to continuously monitor and sample the joystick and accelerometer
// calls the necessary functions from drumBeat interface when certain events triggered

#ifndef _ZENCAPE_CONTROLS_H_
#define _ZENCAPE_CONTROLS_H_

#include <stdio.h>
#include <stdbool.h>
#include <pthread.h>
#include <assert.h>
#include "hal/audioMixer.h"
#include "hal/joystick.h"
#include "hal/accelerometer.h"
#include "hal/timing.h"
#include "drumBeat.h"

void zenCapeControls_init(void);
void zenCapeControls_cleanup(void);

#endif