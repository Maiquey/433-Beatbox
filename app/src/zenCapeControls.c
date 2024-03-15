#include "zenCapeControls.h"

#define NO_INPUT -1
#define JOYSTICK_UP 0
#define JOYSTICK_RIGHT 1
#define JOYSTICK_DOWN 2
#define JOYSTICK_LEFT 3
#define JOYSTICK_IN 4

#define JOYSTICK_DEBOUND_TIME 200

static void* joystickInputThread();
static pthread_t joyStickThreadId;

static bool is_initialized = false;
static bool isRunning = false;

void zenCapeControls_init(void)
{
    assert(!is_initialized);
    isRunning = true;
    pthread_create(&joyStickThreadId, NULL, joystickInputThread, NULL);
    is_initialized = true;
}

void zenCapeControls_cleanup(void)
{
    assert(is_initialized);
    pthread_join(joyStickThreadId, NULL);
    is_initialized = false;
}

static void* joystickInputThread()
{
    long long debounceTimestamp = getTimeInMs();
    while(isRunning){
        if (getTimeInMs() - debounceTimestamp > JOYSTICK_DEBOUND_TIME){
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