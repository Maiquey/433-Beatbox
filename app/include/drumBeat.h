// Sample application module
// Normally, you might create a module such as this for each module in
// the application that does some high-level task; perhaps has its own thread.

#ifndef _DRUMBEAT_H_
#define _DRUMBEAT_H_

#include <stdio.h>
#include <stdbool.h>
#include "hal/audioMixer.h"
#include "hal/timing.h"

void drumBeat_init(void);
void drumBeat_startRockBeat(void);
void drumBeat_cleanup(void);

#endif