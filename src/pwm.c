#include "pwm.h"
#include <xc.h>
#include "utils.h"
#include <math.h>
#include "IOconfig.h"
// #include "IOconfigDevBoard.h"
#include "motors.h"


// PWM module.
//
// Caution! For driving the motors, please use the function runMotors() in
// motors.c instead.

/**
 * Setup PWM. Also determines the best prescale value.
 * 
 * @param PWMPeriodMs: Period of PWM signal in ms.
 * @return 0 if desired pwm period is possible with the given prescale values.
 */
int setupPWM(float PWMPeriodMs) {
    // determine best prescale value, the smaller, the better
    unsigned int prescaleValues[4] = {1, 4, 16, 64};
    unsigned char P1TCONbits_PTCKPS[4] = {0b00, 0b01, 0b10, 0b11};
    unsigned char len = sizeof (prescaleValues) / sizeof (unsigned char);

    int positionIndex; // index of best prescale value
    int basePeriodCount; // counter value that represents one PWM period

    getPositionSmallestPossiblePrescaler(
            PWMPeriodMs,
            PWMTIMER_MAX_COUNT,
            INTERNAL_OSC_PERIOD_MICROSECS,
            prescaleValues,
            len,
            &positionIndex,
            &basePeriodCount);

    if (positionIndex == -1) {
        return -1; // Error handling
    }

    P1TCONbits.PTEN = 0; // Switch off PWM generator
    P1TCONbits.PTCKPS = P1TCONbits_PTCKPS[positionIndex]; // Sets prescaler
    P1TPER = basePeriodCount;
    PWM1CON1bits.PMOD1 = 1; // set PWM unit 1 to independent mode
    PWM1CON1bits.PMOD2 = 1; // set PWM unit 2 to independent mode

    PWM1CON1bits.PEN1H = 0; // disable  PWM driver
    PWM1CON1bits.PEN2H = 0; // disable PWM driver
    PWM1CON1bits.PEN3H = 0; // disable PWM driver
    PWM1CON1bits.PEN1L = 1; // enable PWM driver PWM1L1 
    PWM1CON1bits.PEN2L = 1; // enable PWM driver PWM1L2
    PWM1CON1bits.PEN3L = 0; // disable PWM driver

    P1TCONbits.PTEN = 1; // Switch on PWM generator

    // at first set all duty cycles on 0
    P1DC1 = 0;
    P1DC2 = 0;
    P1DC3 = 0;

    return 0;
}

/**
 * Set the PWM duty cycle.
 * Includes safety checks to prevent motor from damage
 * 
 * @param perc: Float between [0, 1] that indicates the duty cycle.
 * @param numDutyCycleGenerator: Use duty cycle generator P1DCX.
 */
void setPWMDutyCycle(float perc, unsigned char numDutyCycleGenerator) {
    volatile uint16_t * P1DCs[3] = {&P1DC1, &P1DC2, &P1DC3};

    // safety checks to prevent motor from damage
    if (perc > MAX_DC) {
        perc = MAX_DC;
    } else if (perc < 0) {
        perc = 0;
    }

    *(P1DCs[numDutyCycleGenerator]) = (uint16_t) (perc * 2 * P1TPER);
}

/**
 * Helper function to periodicly switch duty cycles.
 * Currently not in use.
 */
void periodicDutyCycleSwitching() {
    static int dutyCyclyIndex = 0;
    static float dutyCycles[4] = {0.1, 0.5, 0.75, 1.0};

    setPWMDutyCycle(dutyCycles[++dutyCyclyIndex], 0);
    if (dutyCyclyIndex == 4) {
        dutyCyclyIndex = 0;
    }
}

/**
 * Helper function to periodicly switch duty cycles in a sinus curve.
 * Currently not in use.
 * 
 * @param periodSteps: How many function calls are equal to one period.
 */
void sinusoidDutyCycleSwitching(int periodSteps) {
    static int i = 0;
    float dc = 0;

    i++;
    dc = (sin(2 * M_PI * i / periodSteps) + 1) / 2;

    setPWMDutyCycle(dc, 0);

    if (i == periodSteps) {
        i = 0;
    }
}