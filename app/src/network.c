// network.c
// Internal implementation for networking side of the light sampling application
// Processes incoming udp packets and returns a reply based on the command received

#include "network.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include "drumBeat.h"

#define MAX_LEN 1500
#define MAX_WRITABLE_HISTORY 1470 //max number of bytes that can be sent for history without causing a line break
#define PORT 12345

static pthread_cond_t* mainCondVar;

static bool isRunning = true;
static bool is_initialized = false;
static struct sockaddr_in sin;
static int socketDescriptor;
static bool firstMessage = true;
static char lastMessage[MAX_LEN];

static pthread_t thread;

static void* receiveData();
static void processRx(char* messageRx, int bytesRx, struct sockaddr_in sinRemote, unsigned int sin_len);

// Begin/end the background thread which processes incoming data.
void Network_init(pthread_cond_t* stopCondVar)
{
    assert(!is_initialized);
    is_initialized = true;

    mainCondVar = stopCondVar;

    memset(&sin, 0, sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = htonl(INADDR_ANY);
    sin.sin_port = htons(PORT);

    socketDescriptor = socket(PF_INET, SOCK_DGRAM, 0);
    bind(socketDescriptor, (struct sockaddr*) &sin, sizeof(sin));
    pthread_create(&thread, NULL, receiveData, NULL);
}

// Cleanup
void Network_cleanup(void)
{
    assert(is_initialized);
    is_initialized = false;
    isRunning = false;
    close(socketDescriptor);
    pthread_join(thread, NULL);
}

// main thread loop
// constantly listening for udp packets
// constructs a response via processRx
static void* receiveData()
{
    assert(is_initialized);
    struct sockaddr_in sinRemote;
    char messageRx[MAX_LEN];

    while(isRunning){
        unsigned int sin_len = sizeof(sinRemote);
        int bytesRx = recvfrom(socketDescriptor, messageRx, MAX_LEN - 1, 0, (struct sockaddr*) &sinRemote, &sin_len);
        messageRx[bytesRx] = 0; //null-terminate
        processRx(messageRx, bytesRx, sinRemote, sin_len);

    }

    pthread_exit(NULL);
}

// Send a reply to sender based on the incoming command
static void processRx(char* messageRx, int bytesRx, struct sockaddr_in sinRemote, unsigned int sin_len)
{
    char messageTx[MAX_LEN];

    if (!firstMessage && bytesRx == 1 && messageRx[0]){
        messageRx = lastMessage;
    }
    if (strncmp(messageRx, "beat0", strlen("beat0")) == 0){
        drumBeat_setBeat(0);
        snprintf(messageTx, MAX_LEN, "Switching to None.\n");
    }
    else if (strncmp(messageRx, "beat1", strlen("beat1")) == 0){
        drumBeat_setBeat(1);
        snprintf(messageTx, MAX_LEN, "Switching to Rock Beat.\n");
    }
    else if (strncmp(messageRx, "beat2", strlen("beat2")) == 0){
        drumBeat_setBeat(2);
        snprintf(messageTx, MAX_LEN, "Switching to Half-Time Shuffle.\n");
    }
    else if (strncmp(messageRx, "vol-down", strlen("vol-down")) == 0){
        drumBeat_adjustVolume(-5);
        snprintf(messageTx, MAX_LEN, "Lowering Volume.\n");
    }
    else if (strncmp(messageRx, "vol-up", strlen("vol-up")) == 0){
        drumBeat_adjustVolume(5);
        snprintf(messageTx, MAX_LEN, "Raising Volume.\n");
    }
    else if (strncmp(messageRx, "bpm-down", strlen("bpm-down")) == 0){
        drumBeat_adjustBPM(-5);
        snprintf(messageTx, MAX_LEN, "Decreasing BPM.\n");
    }
    else if (strncmp(messageRx, "bpm-up", strlen("bpm-up")) == 0){
        drumBeat_adjustBPM(5);
        snprintf(messageTx, MAX_LEN, "Increasing BPM.\n");
    }
    else if (strncmp(messageRx, "bass", strlen("bass")) == 0){
        drumBeat_playBass();
        snprintf(messageTx, MAX_LEN, "Playing Bass Drum.\n");
    }
    else if (strncmp(messageRx, "hihat", strlen("hihat")) == 0){
        drumBeat_playHiHat();
        snprintf(messageTx, MAX_LEN, "Playing Hi-Hat.\n");
    }
    else if (strncmp(messageRx, "softsnare", strlen("softsnare")) == 0){
        drumBeat_playSoftSnare();
        snprintf(messageTx, MAX_LEN, "Playing Soft Snare.\n");
    }
    else if (strncmp(messageRx, "hardsnare", strlen("hardsnare")) == 0){
        drumBeat_playHardSnare();
        snprintf(messageTx, MAX_LEN, "Playing Hard Snare.\n");
    }
    else if (strncmp(messageRx, "getInfo", strlen("getInfo")) == 0){
        snprintf(messageTx, MAX_LEN, "{\"mode\":\"rock beat\",\"volume\":\"100\",\"BPM\":\"120\"}");
    }
    else if (strncmp(messageRx, "stop", strlen("stop")) == 0){
        snprintf(messageTx, MAX_LEN, "Program terminating.\n");
    }
    else{
        snprintf(messageTx, MAX_LEN, "unknown command: %s\n", messageRx);
    }

    sendto(socketDescriptor, messageTx, strlen(messageTx), 0, (struct sockaddr*) &sinRemote, sin_len);
    if (strncmp(messageRx, "stop", strlen("stop")) == 0){
        pthread_cond_signal(mainCondVar);
        isRunning = false;
    }
    firstMessage = false;
    memcpy(lastMessage, messageRx, sizeof(char) * bytesRx);
}