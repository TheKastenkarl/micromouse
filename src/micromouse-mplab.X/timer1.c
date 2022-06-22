/*
 * File:   timer1.c
 * Author: Franz Kaschner
 *
 * Created on 15 May 2022, 11:18
 */

#include <stdio.h>

#include "timer1.h"
#include "IOconfig.h"
#include "generalDefines.h"
#include "pwm1.h"
#include "motorEncoders.h"
#include "serialComms.h"
#include "irSensors.h"

/***************************************************************************
* Function Name     : initTimer1                                           *
* Description       : Initialize timer1.                                   *
* Parameters        : timeInMs - time in milliseconds after which an     *
*                       interrupt is called. Max. valid value: 629 [ms]    *
*                       (= 256 * T_CYC * 65535). If input is larger, then  *                 
*                       it is set automatically to 629.                    * 
* Return Value      : None                                                 *
***************************************************************************/
void initTimer1(const float timeInMs) 
{
    float timeInS = timeInMs * 1e-3f;       // convert ms to s
    float T_timer;                          // period of timer
    unsigned int T_timer_count;             // count value for timer1 period register (PR1)
    const unsigned int maxCount = 0xFFFF;   // = 65535, 16 bit (max. possible value count value of timer1 for PR1 register)

    T1CON = 0;                              // ensure Timer 1 is in reset state
    
    // Find ideal F_CYC divide for prescaler
    if (timeInS < T_CYC * maxCount * 1)
    {
        T1CONbits.TCKPS = 0b00; // F_CYC divide by 1
        T_timer = T_CYC * 1;
    }
    else if (timeInS < T_CYC * maxCount * 8)
    {
        T1CONbits.TCKPS = 0b01; // F_CYC divide by 8
        T_timer = T_CYC * 8;
    }
    else if (timeInS < T_CYC * maxCount * 64)
    {
        T1CONbits.TCKPS = 0b10; // F_CYC divide by 64
        T_timer = T_CYC * 64;
    }
    else if (timeInS < T_CYC * maxCount * 64)
    {
        T1CONbits.TCKPS = 0b11; // F_CYC divide by 256
        T_timer = T_CYC * 256;
    }
    else // if input value "timeInMS" is too large
    {
        T1CONbits.TCKPS = 0b11; // F_CYC divide by 256
        T_timer = T_CYC * 256;
        timeInS = maxCount * T_timer; // use max. possible time
    }
    
    T_timer_count = timeInS/T_timer; // calc count value for timer 1 period register (implicit cast to unsigned int)
    T1CONbits.TCS = 0;      // select internal FCY clock source
    T1CONbits.TGATE = 0;    // gated time accumulation disabled
    TMR1 = 0;               // stores current count value
    PR1 = T_timer_count;    // set Timer 1 period register (count value)
    IFS0bits.T1IF = 0;      // reset Timer 1 interrupt flag
    IPC0bits.T1IP = 4;      // set Timer1 interrupt priority level to 4
    IEC0bits.T1IE = 1;      // enable Timer 1 interrupt
    T1CONbits.TON = 0;      // leave timer disabled initially
}

/***************************************************************************
* Function Name     : startTimer1                                         *
* Description       : Start timer1.                                        *
* Parameters        : None                                                 *
* Return Value      : None                                                 *
***************************************************************************/
void startTimer1() 
{
    T1CONbits.TON = 1;      // start timer
}

/***************************************************************************
* Function Name     : _T1Interrupt                                         *
* Description       : ISR of timer1. Toggles LED6 and counts the number of *
*                     interrupts of timer1.                                *
* Parameters        : None                                                 *
* Return Value      : None                                                 *
***************************************************************************/
void __attribute__((__interrupt__, auto_psv)) _T1Interrupt()
{
    static int myCount = 0; // count number of interrupts of timer1
    IFS0bits.T1IF = 0;      // reset Timer 1 interrupt flag 
    myCount++;

    modifyBrightnessIRSensor();
    
    // Toggle LED6 every 100 * 10ms
    if (myCount == 100)
    {
        LED6 = ~LED6;
        myCount = 0;
    }
}