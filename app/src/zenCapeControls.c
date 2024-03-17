#include "zenCapeControls.h"

#define NO_INPUT -1
#define JOYSTICK_UP 0
#define JOYSTICK_RIGHT 1
#define JOYSTICK_DOWN 2
#define JOYSTICK_LEFT 3
#define JOYSTICK_IN 4

// TODO: fine-tune debounce timers - do at the end (Calculat what 1G is, divide each reading by that value)
#define JOYSTICK_DEBOUNCE_TIME 200
#define ACCELEROMETER_BASS_DEBOUNCE_TIME 100
#define ACCELEROMETER_HIHAT_DEBOUNCE_TIME 100
#define ACCELEROMETER_SNARE_DEBOUNCE_TIME 100

#define X_THRESHOLD 8000
#define X_REBOUND 6000
#define Y_THRESHOLD 8000
#define Y_REBOUND 8000
#define Z_THRESHOLD 8000
#define Z_REBOUND 6000

static void* joystickInputThread();
static void* accelerometerSamplingThread();
static pthread_t joyStickThreadId;
static pthread_t accelerometerThreadId;

static bool is_initialized = false;
static bool isRunning = false;

void zenCapeControls_init(void)
{
    assert(!is_initialized);
    isRunning = true;
    pthread_create(&joyStickThreadId, NULL, joystickInputThread, NULL);
    pthread_create(&accelerometerThreadId, NULL, accelerometerSamplingThread, NULL);
    is_initialized = true;
}

void zenCapeControls_cleanup(void)
{
    assert(is_initialized);
    isRunning = false;
    pthread_join(joyStickThreadId, NULL);
    pthread_join(accelerometerThreadId, NULL);
    is_initialized = false;
}

static void* joystickInputThread()
{
    long long debounceTimestamp = getTimeInMs();
    while(isRunning){
        if (getTimeInMs() - debounceTimestamp > JOYSTICK_DEBOUNCE_TIME){
            int joystickID = joystick_getJoyStickPress();
            if (joystickID != NO_INPUT){
                switch (joystickID) {
                    case JOYSTICK_IN:
                        // change beat
                        drumBeat_cycleBeat();
                        break;
                    case JOYSTICK_UP:
                        // volume up by 5
                        drumBeat_adjustVolume(5);
                        break;
                    case JOYSTICK_DOWN:
                        // volume down by 5
                        drumBeat_adjustVolume(-5);
                        break;
                    case JOYSTICK_LEFT:
                        // BPM down by 5
                        drumBeat_adjustBPM(-5);
                        break;
                    case JOYSTICK_RIGHT:
                        // BPM up by 5
                        drumBeat_adjustBPM(5);
                        break;
                }
                debounceTimestamp = getTimeInMs();
            }
        }
        //sample joystick every 10s
        sleepForMs(10);
    }
    pthread_exit(NULL);
}

static void* accelerometerSamplingThread()
{
    // X normal range = {176, 352}
    // Y normal range = {-384, -32}
    // Z normal range = {16464, 16672}
    long long debounceTimerBass = getTimeInMs();
    long long debounceTimerHiHat = debounceTimerBass;
    long long debounceTimerSnare = debounceTimerBass;
    unsigned char* accInitial = accelerometer_readOutVals();
    int16_t x_last = (accInitial[1] << 8) | accInitial[0];
    int16_t y_last = (accInitial[3] << 8) | accInitial[2];
    int16_t z_last = (accInitial[5] << 8) | accInitial[4];
    int16_t x_floor = x_last - X_THRESHOLD;
    int16_t y_floor = y_last - Y_THRESHOLD;
    int16_t z_floor = z_last - Z_THRESHOLD;
    int16_t min = z_last;
    int16_t max = z_last;
    free(accInitial);
    while(isRunning){
        unsigned char* accelerometerOutput = accelerometer_readOutVals();
        int16_t x_data = (accelerometerOutput[1] << 8) | accelerometerOutput[0];
        int16_t y_data = (accelerometerOutput[3] << 8) | accelerometerOutput[2];
        int16_t z_data = (accelerometerOutput[5] << 8) | accelerometerOutput[4];
        
        if ((getTimeInMs() - debounceTimerHiHat > ACCELEROMETER_HIHAT_DEBOUNCE_TIME) 
            && x_last < x_floor 
            && x_data > x_last + X_REBOUND){
            // printf("HIHAT\n");
            drumBeat_playHiHat();
            debounceTimerHiHat = getTimeInMs();
        }
        if ((getTimeInMs() - debounceTimerSnare > ACCELEROMETER_SNARE_DEBOUNCE_TIME) 
            && y_last < y_floor 
            && y_data > y_last + Y_REBOUND){
            // printf("SNARE\n");
            drumBeat_playHardSnare();
            debounceTimerSnare = getTimeInMs();
        }
        if ((getTimeInMs() - debounceTimerBass > ACCELEROMETER_BASS_DEBOUNCE_TIME) 
            && z_last < z_floor 
            && z_data > z_last + Z_REBOUND){
            // printf("BASS\n");
            drumBeat_playBass();
            debounceTimerBass = getTimeInMs();
        }
        x_last = x_data;
        y_last = y_data;
        z_last = z_data;
        // printf("OUT_X_L val: %d\n", z_data);
        free(accelerometerOutput);
        if (z_data < min){
            min = z_data;
        }
        if (z_data > max){
            max = z_data;
        }
        // printf("{%d, %d}\n", min, max);
        // printf("%d\n", z_data);
        
        sleepForMs(10);
    }
    pthread_exit(NULL);
}