#include "drumBeat.h"

#define BASS_DRUM "wave-files/100051__menegass__gui-drum-bd-hard.wav"
#define HI_HAT "wave-files/100053__menegass__gui-drum-cc.wav"
#define SNARE "wave-files/100059__menegass__gui-drum-snare-soft.wav"

static wavedata_t bass;
static wavedata_t hiHat;
static wavedata_t snare;

static bool isDrumBeat = false;
void drumBeat_init(void)
{
    AudioMixer_readWaveFileIntoMemory(BASS_DRUM, &bass);
    AudioMixer_readWaveFileIntoMemory(HI_HAT, &hiHat);
    AudioMixer_readWaveFileIntoMemory(SNARE, &snare);
}

// TODO: make run on seperate thread?
// Tempo control?
// for now assume 120 BPM
void drumBeat_startRockBeat(void)
{
    isDrumBeat = true;
    int halfBeat = 0;
    while (isDrumBeat){
        if (halfBeat >= 8){
            halfBeat = 0;
        }
        AudioMixer_queueSound(&hiHat);
        if (halfBeat % 4 == 0){
            AudioMixer_queueSound(&bass);
        } else if (halfBeat % 2 == 0){
            AudioMixer_queueSound(&snare);
        }
        halfBeat++;
        sleepForMs(250);
    }
}

void drumBeat_cleanup(void)
{
    AudioMixer_freeWaveFileData(&bass);
    AudioMixer_freeWaveFileData(&hiHat);
    AudioMixer_freeWaveFileData(&snare);
}