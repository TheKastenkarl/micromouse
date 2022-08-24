#ifndef TESTS_H
#define	TESTS_H

void sleep(long operations);
void testButtonAndLed();
void testIR(unsigned char irID, float updateFrac);
void testMotor(unsigned char motorID);
void testBluetoothUART();
void testEncoder(unsigned char motorID);

#endif	/* TESTS_H */

