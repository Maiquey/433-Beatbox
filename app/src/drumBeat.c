#include "drumBeat.h"

#define BASS_DRUM "wave-files/100051__menegass__gui-drum-bd-hard.wav"
#define HI_HAT "wave-files/100053__menegass__gui-drum-cc.wav"
#define SNARE "wave-files/100059__menegass__gui-drum-snare-soft.wav"

#define EIGHTH_NOTE_LENGTH_NUM 30000

static wavedata_t bass;
static wavedata_t hiHat;
static wavedata_t snare;

static int BPM = 120;
// Calculation for quarter note = 60000 / BPM (in ms)
// Calculation for eighth note = 30000 / BPM (in ms)
static void* drumMachineThread();
static pthread_t drumThreadId;
static bool is_initialized = false;

static bool isRunning = false;
void drumBeat_init(void)
{
    assert(!is_initialized);
    is_initialized = true;
    AudioMixer_readWaveFileIntoMemory(BASS_DRUM, &bass);
    AudioMixer_readWaveFileIntoMemory(HI_HAT, &hiHat);
    AudioMixer_readWaveFileIntoMemory(SNARE, &snare);

    isRunning = true;
    pthread_create(&drumThreadId, NULL, drumMachineThread, NULL);
}

// TODO: change sleeptime based on tempo
// for now assume 120 BPM
static void* drumMachineThread()
{
    assert(is_initialized);
    int halfBeat = 0;
    while (isRunning){
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
        sleepForMs(EIGHTH_NOTE_LENGTH_NUM / BPM);
    }

    pthread_exit(NULL);
}

void drumBeat_cleanup(void)
{
    assert(is_initialized);
    isRunning = false;
    pthread_join(drumThreadId, NULL);
    AudioMixer_freeWaveFileData(&bass);
    AudioMixer_freeWaveFileData(&hiHat);
    AudioMixer_freeWaveFileData(&snare);
    is_initialized = false;
}