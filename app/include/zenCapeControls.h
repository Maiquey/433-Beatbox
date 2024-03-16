// Sample application module
// Normally, you might create a module such as this for each module in
// the application that does some high-level task; perhaps has its own thread.

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