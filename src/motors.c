#include "xc.h"
#include "motors.h"
#include "IOconfig.h"
// #include "IOconfigDevBoard.h"
#include "pwm.h"
#include "utils.h"

/**
 * Clean the duty cycle value.
 * 
 * Currently not in use, as this is done inside the controller.
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
        setPWMDutyCycle(1.0, motorId);
    } else {
        setPWMDutyCycle(0.0, motorId);
    }
    
    if (motorId == 0) {
        M1_PLUS_EN = 0;
        M1_MINUS_EN = 0;
    } else if (motorId == 1) {
        M2_PLUS_EN = 0;
        M2_MINUS_EN = 0;
    }
}

void forwardMotor(float dc, unsigned char motorId) {
    if (motorId == 0) {
        M1_PLUS_EN = 1;
        M1_MINUS_EN = 0;
    } else if (motorId == 1) {
        M2_PLUS_EN = 0;
        M2_MINUS_EN = 1;
    }

    setPWMDutyCycle(cleanDc(dc), motorId);
}

void backwardMotor(float dc, unsigned char motorId) {
    if (motorId == 0) {
        M1_PLUS_EN = 0;
        M1_MINUS_EN = 1;
    } else if (motorId == 1) {
        M2_PLUS_EN = 1;
        M2_MINUS_EN = 0;
    }

    setPWMDutyCycle(dc, motorId);
}

/**
 * Function to drive the motors.
 * 
 * @param dc: duty cycle between [-1; 1] as a fraction, where negative values
 *  tell the corresponding wheel to turn backwards
 * @param motorId: motor ID to select. 0 for left, 1 for right
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
