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

static unsigned char readI2cReg(int i2cFileDesc, unsigned char regAddr)
{
    // To read a register, must first write the address
    int res = write(i2cFileDesc, &regAddr, sizeof(regAddr));
    if (res != sizeof(regAddr)) {
        perror("I2C: Unable to write to i2c register.");
        exit(1);
    }
    // Now read the value and return it
    char value = 0;
    res = read(i2cFileDesc, &value, sizeof(value));
    if (res != sizeof(value)) {
        perror("I2C: Unable to read from i2c register");
        exit(1);
    }
    return value;
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
    unsigned char whoAmI = readI2cReg(i2cFileDesc, 0x0F);
    printf("who am I? %x.\n", whoAmI);
    
    // writeToFile(LEFT_DIRECTION, "out");
    // writeToFile(RIGHT_DIRECTION, "out");

}

void accelerometer_cleanup(void)
{
    assert(is_initialized);
    is_initialized = false;
    isRunning = false;
    close(i2cFileDesc);
}