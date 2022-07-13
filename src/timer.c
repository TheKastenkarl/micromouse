#include "xc.h"
#include "timer.h"
#include "utils.h"
#include "pwm.h"
#include "motorEncoders.h"
#include <stdio.h>
#include "serialComms.h"
#include "tests.h"

// max durations for a given prescale value:
// 1: 2.45 ms
// 8: 19.66 ms
// 64: 157.28 ms
// 256: 629.14 ms

int initTimer1(float periodMs) {
    T1CONbits.TON = 0; // initially off
    T1CONbits.TCS = 0; // use internal clock for counting
    T1CONbits.TGATE = 0; // use internal clock for counting

    // determine prescale value, the lower the better
    unsigned int prescaleValues[4] = {1, 8, 64, 256};
    unsigned char T1CONbits_TCKPSs[4] = {0b00, 0b01, 0b10, 0b11};
    unsigned char len = sizeof (prescaleValues) / sizeof (unsigned int);

    int positionIndex;
    int reqCount;

    getPositionSmallestPossiblePrescaler(
            periodMs,
            TIMER1_MAX_COUNT,
            INTERNAL_OSC_PERIOD_MICROSECS,
            prescaleValues,
            len,
            &positionIndex,
            &reqCount);

    if (positionIndex == -1) {
        return -1; // Error handling
    }

    T1CONbits.TCKPS = T1CONbits_TCKPSs[positionIndex]; // set prescale value
    PR1 = reqCount; // set period register

    // interrupts
    IFS0bits.T1IF = 0; // reset Timer 1 interrupt flag
    IPC0bits.T1IP = 4; // set Timer1 interrupt priority level to 4
    IEC0bits.T1IE = 1; // enable Timer 1 interrupt

    return 0;
}

/**
 * Resets the counter values and starts Timer 1.
 */
void startTimer1(void) {
    TMR1 = 0; // reset count
    T1CONbits.TON = 1; // turn on
}

/**
 * Stops Timer 1.
 */
void stopTimer1(void) {
    T1CONbits.TON = 0; // turn off
}

void virtualTimer(int actionEveryXCalls) {
    static int i = 0;

    i++;
    if (i == actionEveryXCalls) {
        i = 0;

        testEncoder(0);
    }
}

// Timer 1 interrupt
void __attribute__((__interrupt__, auto_psv)) _T1Interrupt(void) {
    IFS0bits.T1IF = 0; // reset Timer 1 interrupt flag 

    // updatePositionCount(0);
    // updateDeltaCountsSinceLastCall(0);
    // updatePositionCount(1);
    // updateDeltaCountsSinceLastCall(1);

    virtualTimer(10); // 10 * T_Interrupt = 1 s
}
