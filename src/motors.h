#ifndef MOTORS_H
#define	MOTORS_H

#define MAX_DC 0.66
#define MIN_DC 0.0

void runMotor(float dc, unsigned char motorId, unsigned char isFastStopMode);

#endif	/* MOTORS_H */

