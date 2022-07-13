#include "xc.h"
#include "tests.h"
#include "serialComms.h"
#include "motors.h"

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
