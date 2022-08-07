#ifndef TESTS_H
#define	TESTS_H

void sleep(long operations);
void testButtonAndLed(unsigned char buttonID, unsigned char ledID);
void testIR(unsigned char irID);
void testMotor(unsigned char motorID);
void testBluetoothUART();
void testEncoder(unsigned char motorID);

#endif	/* TESTS_H */

