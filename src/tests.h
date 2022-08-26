#ifndef TESTS_H
#define	TESTS_H
#include <stdbool.h>


void sleep(long operations);
void testButtonAndLed();
void testIR(unsigned char irID, float updateFrac);
void testMotor(unsigned char motorID);
void testBluetoothUART();
void testEncoder(unsigned char motorID);
void testControl(float target);

#endif	/* TESTS_H */

