// Implementation of HAL Joystick module

#include "hal/joystick.h"

#define JSUP_DIRECTION "/sys/class/gpio/gpio26/direction"
#define JSUP_EDGE      "/sys/class/gpio/gpio26/edge"
#define JSUP_IN        "/sys/class/gpio/gpio26/value"
#define JSRT_DIRECTION "/sys/class/gpio/gpio47/direction"
#define JSRT_EDGE      "/sys/class/gpio/gpio47/edge"
#define JSRT_IN        "/sys/class/gpio/gpio47/value"
#define JSDN_DIRECTION "/sys/class/gpio/gpio46/direction"
#define JSDN_EDGE      "/sys/class/gpio/gpio46/edge"
#define JSDN_IN        "/sys/class/gpio/gpio46/value"
#define JSLFT_DIRECTION "/sys/class/gpio/gpio65/direction"
#define JSLFT_EDGE      "/sys/class/gpio/gpio65/edge"
#define JSLFT_IN        "/sys/class/gpio/gpio65/value"

#define NUM_DIRECTIONS 4
#define JOYSTICK_UP 0
#define JOYSTICK_RIGHT 1
#define JOYSTICK_DOWN 2
#define JOYSTICK_LEFT 3
#define TIMEOUT_CODE 4
#define ERROR_CODE -1
#define BUFF_SIZE 1024
#define INDEFINITE_TIMEOUT -1

// Lists for looping through different files for all 4 joystick directions
// Always in the order Up->Right->Down->Left

const char* DirectionFiles[NUM_DIRECTIONS] = {
    JSUP_DIRECTION,
    JSRT_DIRECTION,
    JSDN_DIRECTION,
    JSLFT_DIRECTION
};

const char* ValueFiles[NUM_DIRECTIONS] = {
    JSUP_IN,
    JSRT_IN,
    JSDN_IN,
    JSLFT_IN
};

const char* EdgeFiles[NUM_DIRECTIONS] = {
    JSUP_EDGE,
    JSRT_EDGE,
    JSDN_EDGE,
    JSLFT_EDGE
};

// Courtesy of Assignment Description Doc
static void runCommand(char* command)
{
	// Execute the shell command (output into pipe)
	FILE *pipe = popen(command, "r");
	// Ignore output of the command; but consume it
	// so we don't get an error when closing the pipe.
	char buffer[1024];
	while (!feof(pipe) && !ferror(pipe)) {
		if (fgets(buffer, sizeof(buffer), pipe) == NULL)
			break;
		// printf("--> %s", buffer); // Uncomment for debugging
	}
	// Get the exit code from the pipe; non-zero is an error:
	int exitCode = WEXITSTATUS(pclose(pipe));
	if (exitCode != 0) {
		perror("Unable to execute command:");
		printf(" command: %s\n", command);
		printf(" exit code: %d\n", exitCode);
	}
}

static int readLineFromFile(const char* fileName, char* buff, unsigned int maxLength)
{
	FILE *file = fopen(fileName, "r");
	int bytes_read = getline(&buff, (size_t*) &maxLength, file);
	fclose(file);
	return bytes_read;
}

static void writeToFile(const char* fileName, const char* value)
{
	FILE *pFile = fopen(fileName, "w");
	fprintf(pFile, "%s", value);
	fclose(pFile);
}

// Modified from Dr. Brian Fraser's code example and the epoll man page
static int waitForGpioEdge(const char** fileNamesForGpioValue, long long timeout) 
{
	// for multiple file triggers
    int fdList[NUM_DIRECTIONS];

	// create an epoll instance
	int epollfd = epoll_create(1);
	if (epollfd == -1) {
		fprintf(stderr, "ERROR: epoll_create() returned (%d) = %s\n", epollfd, strerror(errno));
		return -1;
	}

	// create an array of epoll events
	struct epoll_event events[NUM_DIRECTIONS];

	// open GPIO value file for each joystick direction
    for (int i = 0; i < NUM_DIRECTIONS; i++){
		int fdLeft = open(fileNamesForGpioValue[i], O_RDONLY | O_NONBLOCK);
		fdList[i] = fdLeft;
		if (fdLeft == -1) {
			fprintf(stderr, "ERROR: unable to open() GPIO value file (%d) = %s\n", fdLeft, strerror(errno));
			return -1;
		}

		// configure epoll to wait for events: read operation | edge triggered | urgent data
		struct epoll_event epollStruct;
		epollStruct.events = EPOLLIN | EPOLLET | EPOLLPRI;
		epollStruct.data.fd = fdLeft;

		// register file descriptor on the epoll instance, see: man epoll_ctl
		if (epoll_ctl(epollfd, EPOLL_CTL_ADD, fdLeft, &epollStruct) == -1) {
			fprintf(stderr, "ERROR: epoll_ctl() failed on add control interface (%d) = %s\n", fdLeft, strerror(errno));
			return -1;
		}

		// Add epoll event to list
		events[i] = epollStruct;
	}

	// ignore first trigger
	for (int i = 0; i < 2; i++) {
		int waitRet = epoll_wait(
				epollfd, 
				events, 
				NUM_DIRECTIONS,                // maximum # events (4 possible directions)
				timeout);              // timeout in ms, -1 = wait indefinite; 0 = return immediately
		if (waitRet == -1){
			fprintf(stderr, "ERROR: epoll_wait() failed (%d) = %s\n", waitRet, strerror(errno));
			for (int i = 0; i < NUM_DIRECTIONS; i++){
				close(fdList[i]);
			}
			close(epollfd);
			return -1; //error
		} else if (waitRet == 0) { // in case of timeout
			return 1;
		}
	}

	// cleanup epoll instance:
	for (int i = 0; i < NUM_DIRECTIONS; i++){
		close(fdList[i]);
	}
	close(epollfd);
	return 0;
}

void joystick_init(void)
{
	// Force the joystick's pins to be treated as GPIO
	runCommand("config-pin p8.14 gpio");
	runCommand("config-pin p8.15 gpio");
	runCommand("config-pin p8.16 gpio");
	runCommand("config-pin p8.17 gpio");
	// Configure joystick input and edge triggers
	for (int i = 0; i < NUM_DIRECTIONS; i++){
        writeToFile(DirectionFiles[i], "in");
        // .. set edge trigger; options are "none", "rising", "falling", "both"
        writeToFile(EdgeFiles[i], "both");
    }
}

// Returns the ID of the joystick direction pressed according to the order specified at the top of this file
// Returns TIMEOUT_CODE 4 if timeout on edge trigger reached
// Returns -1 in case of an error (edge trigger error or no joystick not pressed in any direction)
int joystick_getJoyStickPress(long long timeout){
	// Block and wait for edge triggered change on GPIO pin

	// Wait for an edge trigger:
	int ret = waitForGpioEdge(ValueFiles, timeout);
	if (ret == -1) {
		return ERROR_CODE;
	} else if (ret == 1){
		return TIMEOUT_CODE;
	}

	for (int i = 0; i < NUM_DIRECTIONS; i++){
		char buff[BUFF_SIZE];
		int bytesRead = readLineFromFile(ValueFiles[i], buff, BUFF_SIZE);
		if (bytesRead > 0) {
			if (buff[0] == 48){ // 48 == '0' in ASCII
				return i;
			}
		} else {
			fprintf(stderr, "ERROR: Read 0 bytes from GPIO input: %s\n", strerror(errno));
		}
	}
	return ERROR_CODE;
}

bool joystick_checkIfPressed(void){
	for (int i = 0; i < NUM_DIRECTIONS; i++){
		char buff[BUFF_SIZE];
		int bytesRead = readLineFromFile(ValueFiles[i], buff, BUFF_SIZE);
		if (bytesRead > 0) {
			if (buff[0] == 48){ // 48 == '0' in ASCII
				return true;
			}
		} else {
			fprintf(stderr, "ERROR: Read 0 bytes from GPIO input: %s\n", strerror(errno));
		}
	}
	return false;
}

// Called when requesting joystick release for user
// Waits indefinitely for edge trigger on release
void joystick_waitForRelease(void){
	waitForGpioEdge(ValueFiles, INDEFINITE_TIMEOUT);
}

// Check if UP/DOWN being pressed
bool joystick_isPressedUpDown(void){
	for (int i = 0; i < NUM_DIRECTIONS; i++){
		char buff[BUFF_SIZE];
		int bytesRead = readLineFromFile(ValueFiles[i], buff, BUFF_SIZE);
		if (bytesRead > 0) {
			if (buff[0] == 48 && (i == JOYSTICK_UP || i == JOYSTICK_DOWN)){ // 48 == '0' in ASCII
				return true;
			}
		} else {
			fprintf(stderr, "ERROR: Read 0 bytes from GPIO input: %s\n", strerror(errno));
		}
	}
	return false;
}

// Check if LEFT/RIGHT being pressed
bool joystick_isPressedLeftRight(void){
	for (int i = 0; i < NUM_DIRECTIONS; i++){
		char buff[BUFF_SIZE];
		int bytesRead = readLineFromFile(ValueFiles[i], buff, BUFF_SIZE);
		if (bytesRead > 0) {
			if (buff[0] == 48 && (i == JOYSTICK_LEFT || i == JOYSTICK_RIGHT)){ // 48 == '0' in ASCII
				return true;
			}
		} else {
			fprintf(stderr, "ERROR: Read 0 bytes from GPIO input: %s\n", strerror(errno));
		}
	}
	return false;
}