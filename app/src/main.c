// Main program to build the application
// Has main(); does initialization and cleanup and perhaps some basic logic.

#include <stdio.h>
#include <stdbool.h>
#include "drumBeat.h"

int main()
{
    // Initialize all modules; HAL modules first
    AudioMixer_init();
    drumBeat_init();
    
    // main logic
    while(true);

    // Cleanup all modules (HAL modules last)
    drumBeat_cleanup();
    AudioMixer_cleanup();

    printf("!!! DONE !!!\n"); 

}