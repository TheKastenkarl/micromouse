#include "xc.h"
#include "tests.h"
#include "serialComms.h"
#include "motors.h"
#include <stdio.h>
#include "motorEncoders.h"

void sleep(long operations) {
    long i = 0;
    while (i < operations) {
        i++;
    }
}

void testMotors() {
    int i = 0;
    long sleepTime = 100000;
    for (i = 0; i < 2; i++) {
        char msg[15];
        sprintf(msg, "motor %d", i);
        sendUART1(msg, 1);
        runMotor(0.5, i, 0);
        sleep(sleepTime);
        runMotor(-0.5, i, 0);
        sleep(sleepTime);
        runMotor(0, i, 0);
        sleep(sleepTime);
        runMotor(0.5, i, 0);
        sleep(sleepTime);
        runMotor(0, i, 1);
        sleep(sleepTime);
    }
}

void testBluetoothUART() {
    char greeting[] = "Test Test";
    sendUART1(greeting, 1);
}

void testEncoder(unsigned char motorID) {
    // read motor encoder values and send via UART
    long positionInCounts = g_counts[motorID];
    float positionInRad = convertCountsToRad(positionInCounts);
    float positionInWheelRots = convertCountsToWheelRots(positionInCounts);
    int velocityInCountsPerSample = g_deltaCountsSinceLastCall[motorID];
    float velocityInRadPerSample = convertCountsToRad(velocityInCountsPerSample);
    float velocityInWheelRotsPerSample = convertCountsToWheelRots(velocityInCountsPerSample);

    char sendData[7][100];
    sprintf(sendData[0], "motorID: %d", (int) motorID);
    sprintf(sendData[1], "pos count: %ld", (long) positionInCounts);
    sprintf(sendData[2], "pos rad: %.2f", (double) positionInRad);
    sprintf(sendData[3], "pos rots: %.2f", (double) positionInWheelRots);
    sprintf(sendData[4], "vel count/samp: %d", (int) velocityInCountsPerSample);
    sprintf(sendData[5], "vel rad/samp: %.2f", (double) velocityInRadPerSample);
    sprintf(sendData[6], "vel rots/samp: %.2f", (double) velocityInWheelRotsPerSample);

    int i;
    for (i = 0; i < 7; i++) {
        sendUART1(sendData[i], 1);
    }
}
