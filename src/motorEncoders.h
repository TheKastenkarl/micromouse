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
float getPositionInWheelRots(unsigned char encoderNumber);
float getPositionInRad(unsigned char encoderNumber);
int getVelocityInCountsPerSample(unsigned char encoderNumber);
float getVelocityInWheelRotsPerSample(unsigned char encoderNumber);
float getVelocityInRadPerSample(unsigned char encoderNumber);

extern long g_rotationCounts[2];

#define GET_ENCODER_1(RIGHT_ENCODER_POSITION_VALUE) (RIGHT_ENCODER_POSITION_VALUE = g_rotationCounts[0] + POS1CNT)
#define GET_ENCODER_2(LEFT_ENCODER_POSITION_VALUE) (LEFT_ENCODER_POSITION_VALUE = g_rotationCounts[1] + POS2CNT)

// All below include slack, i.e measured in real environment
#define COUNTS_PER_WHEELROTATION (16 * 4 * 33)
#define COUNTS_PER_CENTIMETER 0
#define COUNTS_PER_CELL (0 * COUNTS_PER_CENTIMETER)
#define WHEEL_ROTATIONS_PERROBOT_ROTATION 0
#define DELTA_COUNTS_90_DEGRESS (0.25 * WHEEL_ROTATIONS_PERROBOT_ROTATION * COUNTS_PER_WHEELROTATION)
#define DELTA_COUNTS_180_DEGRESS (0.5 * WHEEL_ROTATIONS_PERROBOT_ROTATION * COUNTS_PER_WHEELROTATION)
#define SLACK_PERC 0

#endif	/* SENSOR_H */