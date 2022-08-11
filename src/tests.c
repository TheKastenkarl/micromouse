#include "xc.h"
#include "tests.h"
#include "serialComms.h"
#include "motors.h"
#include <stdio.h>
#include "motorEncoders.h"
#include "dma.h"
#include "IOconfig.h" 
// #include "IOconfigDevBoard.h"

// Test module.
//
// Module to test peripherals

/**
 * Simulate sleep
 * 
 * @param operations: Number of operations to sleep.
 */
void sleep(long operations) {
    long i = 0;
    while (i < operations) {
        i++;
    }
}

/**
 * Test buttons and LEDs
 */
void testButtonAndLed() {
    if (BTN1 == BTN_PRESSED) {
        LED0 = LEDON;
    } else {
        LED0 = LEDOFF;
    }

    if (BTN2 == BTN_PRESSED) {
        LED1 = LEDON;
        LED2 = LEDON;
    } else {
        LED1 = LEDOFF;
        LED2 = LEDOFF;
    }
}

/**
 * Test IR sensors
 * 
 * @param irID: ID of IR sensor to test.
 *  0 for left, 1 for right, 2 for front.
 */
void testIR(unsigned char irID) {
    char *irNames[] = {"left", "right", "front"};
    unsigned int irValues[] = {IR_LEFT, IR_RIGHT, IR_FRONT};
    char msg[20];
    sprintf(msg, "IR %s : %u", irNames[irID], irValues[irID]);
    sendUART1(msg, 1);
}

/**
 * Test motor
 * 
 * @param motorID: ID of motor to test.
 *  0 for left, 1 for right.
 */
void testMotor(unsigned char motorID) {
    long sleepTime = 4000000;
    char msg[15];
    sprintf(msg, "motor %d", motorID);
    sendUART1(msg, 1);
    runMotor(-0.5, motorID, 0);
    sleep(sleepTime);
    runMotor(-1.0, motorID, 0);
    sleep(sleepTime);
    runMotor(0.5, motorID, 0);
    sleep(sleepTime);
    runMotor(1.0, motorID, 0);
    sleep(sleepTime);
    runMotor(0, motorID, 0);
    sleep(sleepTime);
    runMotor(1, motorID, 0);
    sleep(sleepTime);
    runMotor(0, motorID, 1);
    sleep(sleepTime);
}

/**
 * Test UART via Bluetooth.
 */
void testBluetoothUART() {
    char greeting[] = "Test Test \n I am a test.";
    sendUART1(greeting, 1);
}

/**
 * Test motor encoder
 * 
 * @param motorID: Encoder ID to test.
 *  0 for left, 1 for right.
 */
void testEncoder(unsigned char motorID) {
    updateEncoderStates(motorID);

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
