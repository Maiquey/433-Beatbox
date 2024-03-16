#include "drumBeat.h"

#define BASS_DRUM "wave-files/100051__menegass__gui-drum-bd-hard.wav"
#define HI_HAT "wave-files/100053__menegass__gui-drum-cc.wav"
#define HARD_SNARE "wave-files/100058__menegass__gui-drum-snare-hard.wav"
#define SOFT_SNARE "wave-files/100059__menegass__gui-drum-snare-soft.wav"

#define DEFAULT_BPM 120
#define MIN_BPM 60
#define MAX_BPM 200
#define EIGHTH_NOTE_LENGTH_NUM 30000
#define QUARTER_NOTE_LENGTH_NUM 60000
#define DEFAULT_BEAT_ID 0
#define NUM_BEATS 3

static wavedata_t bass;
static wavedata_t hiHat;
static wavedata_t hardSnare;
static wavedata_t softSnare;

static int BPM = 120;
static int beatID = DEFAULT_BEAT_ID;
// Calculation for quarter note = 60000 / BPM (in ms)
// Calculation for eighth note = 30000 / BPM (in ms)

static int currentBeat = 0;

static void* drumMachineThread();
static void* textDisplayThread();
static pthread_t drumThreadId;
static pthread_t statsThreadId;
static bool is_initialized = false;

static bool isRunning = false;
void drumBeat_init(void)
{
    assert(!is_initialized);
    
    AudioMixer_readWaveFileIntoMemory(BASS_DRUM, &bass);
    AudioMixer_readWaveFileIntoMemory(HI_HAT, &hiHat);
    AudioMixer_readWaveFileIntoMemory(HARD_SNARE, &hardSnare);
    AudioMixer_readWaveFileIntoMemory(SOFT_SNARE, &softSnare);

    isRunning = true;
    is_initialized = true;
    pthread_create(&drumThreadId, NULL, drumMachineThread, NULL);
    pthread_create(&statsThreadId, NULL, textDisplayThread, NULL);

    
}

static void* drumMachineThread()
{
    assert(is_initialized);
    while (isRunning){
        currentBeat = currentBeat % 4;
        switch (beatID){
            case 0: // silence
                break;
            case 1: // rock beat
                AudioMixer_queueSound(&hiHat);
                if (currentBeat % 2 == 0){
                    AudioMixer_queueSound(&bass);
                } else {
                    AudioMixer_queueSound(&hardSnare);
                }
                sleepForMs(EIGHTH_NOTE_LENGTH_NUM / BPM);
                AudioMixer_queueSound(&hiHat);
                sleepForMs(EIGHTH_NOTE_LENGTH_NUM / BPM);
                break;
            case 2: // half-time shuffle
                AudioMixer_queueSound(&hiHat);
                if (currentBeat == 0){
                    AudioMixer_queueSound(&bass);
                } else if (currentBeat == 2){
                    AudioMixer_queueSound(&hardSnare);
                }
                sleepForMs(QUARTER_NOTE_LENGTH_NUM / (BPM*3)); //triplets!
                AudioMixer_queueSound(&softSnare);
                sleepForMs(QUARTER_NOTE_LENGTH_NUM / (BPM*3));
                AudioMixer_queueSound(&hiHat);
                if (currentBeat % 2 != 0){
                    AudioMixer_queueSound(&bass);
                }
                sleepForMs(QUARTER_NOTE_LENGTH_NUM / (BPM*3));
                break;
        }
        
        currentBeat++;

    }

    pthread_exit(NULL);
}

static void* textDisplayThread()
{
    assert(is_initialized);
    AudioMixer_getStats();
    accelerometer_getStats();
    while (isRunning){
        sleepForMs(1000);
        Period_statistics_t* audioStats = AudioMixer_getStats();
        Period_statistics_t* accelerometerStats = accelerometer_getStats();
        printf("M%d %dbpm vol:%d Audio[%.3f, %.3f] avg %.3f/%d Accel[%.3f, %.3f] avg %.3f/%d\n",
            beatID, BPM, AudioMixer_getVolume(), 
            audioStats->minPeriodInMs, audioStats->maxPeriodInMs, audioStats->avgPeriodInMs, audioStats->numSamples, 
            accelerometerStats->minPeriodInMs, accelerometerStats->maxPeriodInMs, accelerometerStats->avgPeriodInMs, accelerometerStats->numSamples);
        
    }
    pthread_exit(NULL);
}

void drumBeat_cleanup(void)
{
    assert(is_initialized);
    isRunning = false;
    pthread_join(drumThreadId, NULL);
    pthread_join(statsThreadId, NULL);
    AudioMixer_freeWaveFileData(&bass);
    AudioMixer_freeWaveFileData(&hiHat);
    AudioMixer_freeWaveFileData(&hardSnare);
    AudioMixer_freeWaveFileData(&softSnare);
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

void drumBeat_playBass(void)
{
    AudioMixer_queueSound(&bass);
}

void drumBeat_playHiHat(void)
{
    AudioMixer_queueSound(&hiHat);
}

void drumBeat_playSoftSnare(void)
{
    AudioMixer_queueSound(&softSnare);
}

void drumBeat_playHardSnare(void)
{
    AudioMixer_queueSound(&hardSnare);
}