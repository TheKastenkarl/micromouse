#include "xc.h"
#include "serialComms.h"
#include "motors.h"
#include <stdio.h>
#include "motorEncoders.h"
#include "dma.h"
#include "IOconfig.h" 
#include "irSensors.h"
#include "controller.h"
// #include "IOconfigDevBoard.h"

// Test module.
//
// Module to test peripherals

/**
 * Simulate sleep.
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
 * Test IR sensors.
 * 
 * @param irID: ID of IR sensor to test.
 *  0 for left, 1 for right, 2 for front.
 * @param updateFrac: Value between 0-1 indicates how much to trust new value.
 */
void testIR(unsigned char irID, float updateFrac) {
    typedef float (*f)(float);
    f func[3] = {&getLeftIR, &getRightIR, &getFrontIR};
    char *irNames[] = {"left", "right", "front"};

    char msg[20];
    sprintf(msg, "IR %s : %.2f", irNames[irID], (double)func[irID](updateFrac));
    sendUART1(msg, 1);
}

/**
 * Test motor.
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
 * Test motor encoder.
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
void testControl()
{   
    char motorID = 0;
    updateEncoderStates(motorID);

    // read motor encoder values and send via UART
    long positionInCounts_0 = g_counts[motorID];
    float positionInRad_0 = convertCountsToRad(positionInCounts_0);
    float positionInWheelRots_0 = convertCountsToWheelRots(positionInCounts_0);
    int velocityInCountsPerSample_0 = g_deltaCountsSinceLastCall[motorID];
    float velocityInRadPerSample_0 = convertCountsToRad(velocityInCountsPerSample_0);
    float velocityInWheelRotsPerSample_0 = convertCountsToWheelRots(velocityInCountsPerSample_0);
    
    char motorID1=1;
    updateEncoderStates(motorID1);

    // read motor encoder values and send via UART
    long positionInCounts_1 = g_counts[motorID1];
    float positionInRad_1 = convertCountsToRad(positionInCounts_1);
    float positionInWheelRots_1 = convertCountsToWheelRots(positionInCounts_1);
    int velocityInCountsPerSample_1 = g_deltaCountsSinceLastCall[motorID1];
    float velocityInRadPerSample_1 = convertCountsToRad(velocityInCountsPerSample_1);
    float velocityInWheelRotsPerSample_1 = convertCountsToWheelRots(velocityInCountsPerSample_1);

    
    float deltaT = 0.05f;
    //float motorDutyCycle = posControl(-10.0f,  positionInRad,  deltaT,  velocityInRadPerSample);
    controlLoop(30.0F,0.0F,positionInRad_0,0.0F,deltaT,velocityInRadPerSample_1,velocityInRadPerSample_0);
    runMotor(differentialWheels.dutyCycle_Right, motorID1, 0);
    runMotor(differentialWheels.dutyCycle_Left, motorID, 0);


    char sendData[2][100];

    sprintf(sendData[1], "pos rad: %.2f", (double) positionInRad_0);
    int i;
    for (i = 0; i < 2; i++) {
        sendUART1(sendData[i], 1);
    }
}
