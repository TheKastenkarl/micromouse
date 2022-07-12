#include "xc.h"
#include "motors.h"
#include "IOconfig.h"
#include "pwm.h"
#include "utils.h"

/**
 * Clean the duty cycle value.
 * 
 * @param dc: duty cycle as a fraction between 0 and 1.
 * @return cleaned duty cycle in a value range that the motor is able to work
 */
float cleanDc(float dc) {
    // map [0; 1] -> [MIN, MAX]
    return convertRanges(0, 1, MIN_DC, MAX_DC, dc);
}

void stopMotor(unsigned char motorId, unsigned char isFastStopMode) {
    if (isFastStopMode) {
        setPWMDutyCycle(1, motorId - 1);
    } else {
        setPWMDutyCycle(0, motorId - 1);
    }
    
    if (motorId == 1) {
        M1_PLUS_EN = 0;
        M1_MINUS_EN = 0;
    } else if (motorId == 2) {
        M2_PLUS_EN = 0;
        M2_MINUS_EN = 0;
    }
}

void forwardMotor(float dc, unsigned char motorId) {
    if (motorId == 1) {
        M1_PLUS_EN = 1;
        M1_MINUS_EN = 0;
    } else if (motorId == 2) {
        M2_PLUS_EN = 0;
        M2_MINUS_EN = 1;
    }

    setPWMDutyCycle(cleanDc(dc), motorId - 1);
}

void backwardMotor(float dc, unsigned char motorId) {
    if (motorId == 1) {
        M1_PLUS_EN = 0;
        M1_MINUS_EN = 1;
    } else if (motorId == 2) {
        M2_PLUS_EN = 1;
        M2_MINUS_EN = 0;
    }

    setPWMDutyCycle(cleanDc(dc), motorId - 1);
}

/**
 * Function to drive the motors.
 * 
 * @param dc: duty cycle between [-1; 1] as a fraction, where negative values
 *  tell the corresponding wheel to turn backwards
 * @param motorId: motor ID to select. 1 for left, 2 for right
 * @param isFastStopMode: If to stop the motor fast when dc == 0
 */
void runMotor(float dc, unsigned char motorId, unsigned char isFastStopMode) {
    if (dc == 0) {
        stopMotor(motorId, isFastStopMode);
    } else if (dc > 0) {
        forwardMotor(dc, motorId);
    } else if (dc < 0) {
        backwardMotor(-dc, motorId);
    }
}
