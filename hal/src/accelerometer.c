#include "hal/accelerometer.h"

#define I2CDRV_LINUX_BUS1 "/dev/i2c-1"

#define I2C_DEVICE_ADDRESS 0x18

#define CTRL_REG1 0x20
#define OUT_FIRST_BYTE 0x28

#define OUTPUT_BUFFER_SIZE 6

static bool is_initialized = false;
static bool isRunning = true;
static int i2cFileDesc;

static Period_statistics_t *pStats;

// From I2C Guide
static int initI2cBus(char* bus, int address)
{
    int i2cFileDesc = open(bus, O_RDWR);
    int result = ioctl(i2cFileDesc, I2C_SLAVE, address);
    if (result < 0) {
        perror("I2C: Unable to set I2C device to slave address.");
        exit(1);
    }
    return i2cFileDesc;
}

static void writeI2cReg(int i2cFileDesc, unsigned char regAddr, unsigned char value)
{
    unsigned char buff[2];
    buff[0] = regAddr;
    buff[1] = value;
    int res = write(i2cFileDesc, buff, 2);
    if (res != 2) {
        perror("I2C: Unable to write i2c register.");
        exit(1);
    }
}

// From Assignment 1
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

void accelerometer_init(void)
{
    assert(!is_initialized);
    is_initialized = true;

    runCommand("config-pin p9_18 i2c");
    runCommand("config-pin p9_17 i2c");

    i2cFileDesc = initI2cBus(I2CDRV_LINUX_BUS1, I2C_DEVICE_ADDRESS);
    writeI2cReg(i2cFileDesc, CTRL_REG1, 0x27); // set power mode to 1
    pStats = (Period_statistics_t*)malloc(sizeof(Period_statistics_t));

}

void accelerometer_cleanup(void)
{
    assert(is_initialized);
    is_initialized = false;
    isRunning = false;
    free(pStats);
    close(i2cFileDesc);
}

// Reads output bytes in a single stream and returns a byte array
unsigned char* accelerometer_readOutVals()
{
    assert(is_initialized);
    // Must write to the register first to read it
    unsigned char regAddr = OUT_FIRST_BYTE + 0x80;
    int res = write(i2cFileDesc, &regAddr, sizeof(regAddr));
    if (res != sizeof(regAddr)) {
        perror("I2C: Unable to write i2c register.");
        exit(1);
    }

    // Now read the whole stream of bytes and return it
    unsigned char* outputBuffer = (unsigned char*)malloc(OUTPUT_BUFFER_SIZE*sizeof(unsigned char));
    unsigned char readBuffer[OUTPUT_BUFFER_SIZE] = {0};
    res = read(i2cFileDesc, &readBuffer, OUTPUT_BUFFER_SIZE);
    if (res != OUTPUT_BUFFER_SIZE) {
        perror("I2C: Unable to read from i2c registers");
        exit(1);
    }

    Period_markEvent(PERIOD_EVENT_SAMPLE_ACCELEROMETER);

    //memcpy required since read() was not happy working with a malloced array
    memcpy(outputBuffer, readBuffer, sizeof(unsigned char) * (OUTPUT_BUFFER_SIZE));
    return outputBuffer;
}

Period_statistics_t* accelerometer_getStats()
{
    Period_getStatisticsAndClear(PERIOD_EVENT_SAMPLE_ACCELEROMETER, pStats);
    return pStats;
}