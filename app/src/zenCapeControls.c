#include "zenCapeControls.h"

#define NO_INPUT -1
#define JOYSTICK_UP 0
#define JOYSTICK_RIGHT 1
#define JOYSTICK_DOWN 2
#define JOYSTICK_LEFT 3
#define JOYSTICK_IN 4

#define JOYSTICK_DEBOUNCE_TIME 200
#define ACCELEROMETER_DEBOUNCE_TIME 100

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
    long long debounceTimestamp = getTimeInMs();
    unsigned char* accInitial = accelerometer_readOutVals();
    int16_t x_last = (accInitial[1] << 8) | accInitial[0];
    int16_t y_last = (accInitial[3] << 8) | accInitial[2];
    int16_t z_last = (accInitial[5] << 8) | accInitial[4];
    int16_t z_floor = z_last - 6000;
    free(accInitial);
    while(isRunning){
        if (getTimeInMs() - debounceTimestamp > ACCELEROMETER_DEBOUNCE_TIME)
        {
            unsigned char* accelerometerOutput = accelerometer_readOutVals();
            int16_t x_data = (accelerometerOutput[1] << 8) | accelerometerOutput[0];
            int16_t y_data = (accelerometerOutput[3] << 8) | accelerometerOutput[2];
            int16_t z_data = (accelerometerOutput[5] << 8) | accelerometerOutput[4];
            bool valRead = false;
            if (x_data < 0 && x_last > 0){
                printf("HIHAT\n");
                valRead = true;
            }
            if (y_data < 0 && y_last > 0){
                printf("SNARE\n");
                valRead = true;
            }
            if (z_data < z_floor && z_last > z_floor){
                printf("BASS\n");
                valRead = true;
            }
            x_last = x_data;
            y_last = y_data;
            z_last = z_data;
            // printf("OUT_X_L val: %d\n", z_data);
            free(accelerometerOutput);
            if (valRead){
                debounceTimestamp = getTimeInMs();
            }
        }
        sleepForMs(10);
    }
    pthread_exit(NULL);
}