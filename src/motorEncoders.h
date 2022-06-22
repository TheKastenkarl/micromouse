/*
 * File:   motorEncoders.h
 * Author: Franz Kaschner
 *
 * Created on 23. Mai 2019, 11:22
 */

#ifndef MOTORENCODERS_H
#define	MOTORENCODERS_H

#include <xc.h>

// Initialize motor encoder 1.
void initQEI1(const unsigned int startPos);

// Initialize motor encoder 2.
void initQEI2(const unsigned int startPos);

// Get the position of the motor connected to QEI1 in radian.
float getPositionInRad_1();

// Get the position of the motor connected to QEI1 in radian.
float getVelocityInRadPerSecond_1();

// Get the position of the motor connected to QEI1 in the total number of counted pulses.
long getPositionInCounts_1();

// Get the velocity of the motor connected to QEI1 in CountsPerSample
// (number of counted pulses in the sample interval).
int getVelocityInCountsPerSample_1();

// Get the position of the motor connected to QEI2 in the total number of counted pulses.
long getPositionInCounts_2();

// Get the velocity of the motor connected to QEI2 in CountsPerSample
// (number of counted pulses in the sample interval).
int getVelocityInCountsPerSample_2();

extern long rotationCount1; // total number of ticks since initialization (QEI1)
extern long rotationCount2; // total number of ticks since initialization (QEI2)

#define GET_ENCODER_1(RIGHT_ENCODER_POSITION_VALUE) (RIGHT_ENCODER_POSITION_VALUE=rotationCount1+POSCNT)
#define GET_ENCODER_2(LEFT_ENCODER_POSITION_VALUE) (LEFT_ENCODER_POSITION_VALUE=rotationCount2+POS2CNT)

#define WHEEL_ROTATIONS_PERROBOT_ROTATION 2.5
#define TICKS_PER_WHEELROTATION (16*4*33)
#define TICKS_PER_CENTIMETER TICKS_PER_WHEELROTATION/12.566
#define METER_PER_TICKS 0.12566/TICKS_PER_WHEELROTATION
#define DELTATICKS_90_DEGREES  (0.25* WHEEL_ROTATIONS_PERROBOT_ROTATION*TICKS_PER_WHEELROTATION) 
#define DELTATICKS_180_DEGREES (0.5 * WHEEL_ROTATIONS_PERROBOT_ROTATION*TICKS_PER_WHEELROTATION)
#define DELTATICKS_CELL_GAP (11.5*TICKS_PER_CENTIMETER)

#endif	/* MOTORENCODERS_H */