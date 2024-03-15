#include "drumBeat.h"

#define BASS_DRUM "wave-files/100051__menegass__gui-drum-bd-hard.wav"
#define HI_HAT "wave-files/100053__menegass__gui-drum-cc.wav"
#define SNARE "wave-files/100059__menegass__gui-drum-snare-soft.wav"

#define DEFAULT_BPM 120
#define MIN_BPM 60
#define MAX_BPM 200
#define EIGHTH_NOTE_LENGTH_NUM 30000
#define DEFAULT_BEAT_ID 1
#define NUM_BEATS 3

static wavedata_t bass;
static wavedata_t hiHat;
static wavedata_t snare;

static int BPM = 120;
static int beatID = DEFAULT_BEAT_ID;
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
        switch (beatID){
            case 0: // silence
                break;
            case 1: // rock beat
                if (halfBeat >= 8){
                    halfBeat = 0;
                }
                AudioMixer_queueSound(&hiHat);
                if (halfBeat % 4 == 0){
                    AudioMixer_queueSound(&bass);
                } else if (halfBeat % 2 == 0){
                    AudioMixer_queueSound(&snare);
                }
                break;
            case 2: // new beat (silence for now)
                break;
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

void drumBeat_cycleBeat(void)
{
    beatID = (beatID + 1) % NUM_BEATS;
}

void drumBeat_adjustVolume(int increment)
{
    AudioMixer_setVolume(AudioMixer_getVolume() + increment);
}

void drumBeat_adjustBPM(int increment)
{
    int newBPM = BPM + increment;
    if (newBPM > MAX_BPM){
        BPM = MAX_BPM;
    } else if (newBPM < MIN_BPM){
        BPM = MIN_BPM;
    } else {
        BPM = newBPM;
    }
}