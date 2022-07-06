/*
 * File:   sensor.h
 * Author: bauma
 *
 * Created on 23. Mai 2019, 11:22
 */

#ifndef SENSOR_H
#define	SENSOR_H

#include<xc.h>

void initQEI1(unsigned int startPos);
void initQEI2(unsigned int startPos);
void updateRotationCount(volatile uint16_t posCount, long* globalCount);
long getPositionInCounts(unsigned char encoderNumber);
float getPositionInRad(unsigned char encoderNumber);
int getVelocityInCountsPerSample(unsigned char encoderNumber);
float getVelocityInRadPerSample(unsigned char encoderNumber);

extern long g_rotationCounts[2];

#define GET_ENCODER_1(RIGHT_ENCODER_POSITION_VALUE) (RIGHT_ENCODER_POSITION_VALUE = g_rotationCounts[0] + POS1CNT)
#define GET_ENCODER_2(LEFT_ENCODER_POSITION_VALUE) (LEFT_ENCODER_POSITION_VALUE = g_rotationCounts[1] + POS2CNT)

// TODO
#define WHEEL_ROTATIONS_PERROBOT_ROTATION 2.5
#define TICKS_PER_WHEELROTATION (64*33)
#define TICKS_PER_CENTIMETER TICKS_PER_WHEELROTATION/12.566
#define METER_PER_TICkS 0.12566/TICKS_PER_WHEELROTATION
#define DELTATICKS_90_DEGREES  (0.25* WHEEL_ROTATIONS_PERROBOT_ROTATION*TICKS_PER_WHEELROTATION) 
#define DELTATICKS_180_DEGREES (0.5 * WHEEL_ROTATIONS_PERROBOT_ROTATION*TICKS_PER_WHEELROTATION)
#define DELTATICKS_CELL_GAP (11.5*TICKS_PER_CENTIMETER)




#endif	/* SENSOR_H */