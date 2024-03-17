// Sample application module
// Normally, you might create a module such as this for each module in
// the application that does some high-level task; perhaps has its own thread.

#ifndef _DRUMBEAT_H_
#define _DRUMBEAT_H_

#include <stdio.h>
#include <stdbool.h>
#include <pthread.h>
#include <assert.h>
#include "hal/audioMixer.h"
#include "hal/accelerometer.h"
#include "hal/timing.h"

void drumBeat_init(void);
void drumBeat_cleanup(void);
void drumBeat_cycleBeat(void);
void drumBeat_adjustVolume(int increment);
void drumBeat_adjustBPM(int increment);
void drumBeat_playBass(void);
void drumBeat_playHiHat(void);
void drumBeat_playSoftSnare(void);
void drumBeat_playHardSnare(void);
void drumBeat_setBeat(int beatId);
char* drumBeat_getDrumBeatName(void);
int drumBeat_getBPM(void);

#endif