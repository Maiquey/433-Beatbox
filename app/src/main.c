// Main program to build the application
// Has main(); does initialization and cleanup and perhaps some basic logic.

#include <stdio.h>
#include <stdbool.h>
#include "badmath.h"
#include "hal/audioMixer.h"
#include "hal/timing.h"

#define SOURCE_FILE "wave-files/100060__menegass__gui-drum-splash-hard.wav"

int main()
{
    AudioMixer_init();

    // Initialize all modules; HAL modules first

    wavedata_t crash;
    AudioMixer_readWaveFileIntoMemory(SOURCE_FILE, &crash);
    // Main program logic:
    
    while (true){
        AudioMixer_queueSound(&crash);
        sleepForMs(1000);
    }
    

    // Cleanup all modules (HAL modules last)
    AudioMixer_cleanup();

    printf("!!! DONE !!!\n"); 

}