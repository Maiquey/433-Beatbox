#include "hal/accelerometer.h"

#define I2CDRV_LINUX_BUS1 "/dev/i2c-1"

#define LEFT_DIGIT_GPIO_PATH "/sys/class/gpio/gpio61/"
#define RIGHT_DIGIT_GPIO_PATH "/sys/class/gpio/gpio44/"

#define LEFT_VALUE LEFT_DIGIT_GPIO_PATH "value"
#define LEFT_DIRECTION LEFT_DIGIT_GPIO_PATH "direction"
#define RIGHT_VALUE RIGHT_DIGIT_GPIO_PATH "value"
#define RIGHT_DIRECTION RIGHT_DIGIT_GPIO_PATH "direction"

#define I2C_DEVICE_ADDRESS 0x18

#define CTRL_REG1 0x20
#define OUT_X_L 0x28
#define OUT_X_H 0x29
#define OUT_Y_L 0x2A
#define OUT_Y_H 0x2B
#define OUT_Z_L 0x2C
#define OUT_Z_H 0x2D

#define OUTPUT_BUFFER_SIZE 6

static bool is_initialized = false;
static bool isRunning = true;
static int i2cFileDesc;

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

// static unsigned char readI2cReg(int i2cFileDesc, unsigned char regAddr)
// {
//     // To read a register, must first write the address
//     int res = write(i2cFileDesc, &regAddr, sizeof(regAddr));
//     if (res != sizeof(regAddr)) {
//         perror("I2C: Unable to write to i2c register.");
//         exit(1);
//     }
//     // Now read the value and return it
//     char value = 0;
//     res = read(i2cFileDesc, &value, sizeof(value));
//     if (res != sizeof(value)) {
//         perror("I2C: Unable to read from i2c register");
//         exit(1);
//     }
//     return value;
// }

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

// static void writeToFile(const char* filePath, const char* input)
// {
//     FILE *f = fopen(filePath, "w");
//     if (!f) {
//         printf("ERROR: Unable to file.\n");
//         exit(-1);
//     }
//     int charWritten = fprintf(f, "%s", input);
//     if (charWritten <= 0) {
//         printf("ERROR WRITING DATA");
//         exit(1);
//     }
//     fclose(f);
// }

void accelerometer_init(void)
{
    assert(!is_initialized);
    is_initialized = true;

    runCommand("config-pin p9_18 i2c");
    runCommand("config-pin p9_17 i2c");

    i2cFileDesc = initI2cBus(I2CDRV_LINUX_BUS1, I2C_DEVICE_ADDRESS);
    writeI2cReg(i2cFileDesc, CTRL_REG1, 0x27); // set power mode to 1
    // writeI2cReg(i2cFileDesc, REG_DIRB, 0x00);
    // unsigned char whoAmI = readI2cReg(i2cFileDesc, 0x0F);
    // printf("who am I? %x.\n", whoAmI);
    // char reg = (char)readI2cReg(i2cFileDesc, OUT_X_L);
    // printf("OUT_X_L? %x.\n", reg);
    // reg = (char)readI2cReg(i2cFileDesc, OUT_X_H);
    // printf("OUT_X_H? %x.\n", reg);
    // reg = (char)readI2cReg(i2cFileDesc, OUT_Y_L);
    // printf("OUT_Y_L? %x.\n", reg);
    // reg = (char)readI2cReg(i2cFileDesc, OUT_Y_H);
    // printf("OUT_Y_H? %x.\n", reg);
    // reg = (char)readI2cReg(i2cFileDesc, OUT_Z_L);
    // printf("OUT_Z_L? %x.\n", reg);
    // reg = (char)readI2cReg(i2cFileDesc, OUT_Z_H);
    // printf("OUT_Z_H? %x.\n", reg);

    // unsigned char regAddr = OUT_X_L + 0x80;
    // int res = write(i2cFileDesc, &regAddr, sizeof(regAddr));
    // if (res != sizeof(regAddr)) {
    //     perror("I2C: Unable to write i2c register.");
    //     exit(1);
    // }

    // // Now read the value and return it
    // // unsigned char* readBufferMalloc = (unsigned char*)malloc(OUTPUT_BUFFER_SIZE*sizeof(unsigned char));
    // signed char readBuffer[OUTPUT_BUFFER_SIZE] = {0};
    // res = read(i2cFileDesc, &readBuffer, OUTPUT_BUFFER_SIZE);
    // if (res != OUTPUT_BUFFER_SIZE) {
    //     perror("I2C: Unable to read from i2c registers");
    //     exit(1);
    // }
    // for (int i = 0; i < OUTPUT_BUFFER_SIZE; i++){
    //     printf("register %d: %x = %d\n", i, readBuffer[i], readBuffer[i]);
    // }
    // free(readBuffer);

}

void accelerometer_cleanup(void)
{
    assert(is_initialized);
    is_initialized = false;
    isRunning = false;
    close(i2cFileDesc);
}

unsigned char* accelerometer_readOutVals()
{
    // TODO: make this a seperate function
    // To read a register, must first write the address
    // unsigned char buffer[OUTPUT_BUFFER_SIZE];
    // buffer[0] = OUT_X_L + 0x80;
    // int res = write(i2cFileDesc, buffer, OUTPUT_BUFFER_SIZE);
    // if (res != OUTPUT_BUFFER_SIZE) {
    //     perror("I2C: Unable to write i2c register.");
    //     exit(1);
    // }

    // // Now read the value and return it
    // unsigned char* readBuffer = (unsigned char*)malloc(OUTPUT_BUFFER_SIZE*sizeof(unsigned char));
    // res = read(i2cFileDesc, &readBuffer, sizeof(readBuffer));
    // if (res != sizeof(readBuffer)) {
    //     perror("I2C: Unable to read from i2c register");
    //     exit(1);
    // }
    // for (int i = 0; i < OUTPUT_BUFFER_SIZE; i++){
    //     printf("register %d: %x\n", i, readBuffer[i]);
    // }
    // return readBuffer;


    unsigned char regAddr = OUT_X_L + 0x80;
    int res = write(i2cFileDesc, &regAddr, sizeof(regAddr));
    if (res != sizeof(regAddr)) {
        perror("I2C: Unable to write i2c register.");
        exit(1);
    }

    // Now read the value and return it
    unsigned char* outputBuffer = (unsigned char*)malloc(OUTPUT_BUFFER_SIZE*sizeof(unsigned char));
    unsigned char readBuffer[OUTPUT_BUFFER_SIZE] = {0};
    res = read(i2cFileDesc, &readBuffer, OUTPUT_BUFFER_SIZE);
    if (res != OUTPUT_BUFFER_SIZE) {
        perror("I2C: Unable to read from i2c registers");
        exit(1);
    }
    memcpy(outputBuffer, readBuffer, sizeof(unsigned char) * (OUTPUT_BUFFER_SIZE));
    // for (int i = 0; i < OUTPUT_BUFFER_SIZE; i++){
    //     printf("register %d: %x = %d\n", i, readBuffer[i], readBuffer[i]);
    // }
    return outputBuffer;
}