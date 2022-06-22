/*
 * File:   pwm1.c
 * Author: Franz Kaschner
 *
 * Created on 27 May 2022, 11:10
 */

#include <math.h>
#include <xc.h>
#include "pwm1.h"
#include "generalDefines.h"

/***************************************************************************
* Function Name     : setupPWM1                                            *
* Description       : Set up PWM1 (independent mode). Use set_PWM1_DCs to  *
*                     set the duty cycles (here the DCs are all set to 0). *
* Parameters        : f_pwm - PWM1 frequency in Hz (min. valid value: 3.18 *
*                       (= 1/(64 * T_CYC * 32767)). If input is smaller,   *
*                       then it is set automatically to 3.18.              *
*                     DC - duty cycle of PWM1 (between 0 and 1)            *
*                     PEN1H - enable (1) or disable (0) PWM1H1             *
*                     PEN1L - enable (1) or disable (0) PWM1L1             *
*                     PEN2H - enable (1) or disable (0) PWM1H2             *
*                     PEN2L - enable (1) or disable (0) PWM1L2             *
*                     PEN3H - enable (1) or disable (0) PWM1H3             *
*                     PEN3L - enable (1) or disable (0) PWM1L3             *
* Return Value      : None                                                 *
***************************************************************************/
void setupPWM1(const float f_pwm, const int PEN1H, const int PEN1L, const int PEN2H, const int PEN2L, const int PEN3H, const int PEN3L)
{
    P1TCONbits.PTEN = 0;                    // Switch off PWM generator
    float T_pwm_timer;                      // Period of PWM timer
    float T_pwm = 1/f_pwm;                  // PWM period
    const unsigned int maxCount = 0x7FFF;  // = 32767, 15 bit (max. possible value count value of PWM timer for the P1TPER register)
    unsigned int T_pwm_count;               // Count value of PWM timer for the P1TPER register
    
    // Find ideal F_CYC divide for prescaler
    if (T_pwm < T_CYC * maxCount * 1)
    {
        P1TCONbits.PTCKPS = 0b00; // F_CYC divide by 1
        T_pwm_timer = T_CYC * 1;
    }
    else if (T_pwm < T_CYC * maxCount * 4)
    {
        P1TCONbits.PTCKPS = 0b01; // F_CYC divide by 8
        T_pwm_timer = T_CYC * 4;
    }
    else if (T_pwm < T_CYC * maxCount * 16)
    {
        P1TCONbits.PTCKPS = 0b10; // F_CYC divide by 64
        T_pwm_timer = T_CYC * 16;
    }
    else if (T_pwm < T_CYC * maxCount * 64)
    {
        P1TCONbits.PTCKPS = 0b11; // F_CYC divide by 64
        T_pwm_timer = T_CYC * 64;
    }
    else // if input value "pwm_frequency" is too small and therefore "T_pwm" too high
    {
        P1TCONbits.PTCKPS = 0b11; // F_CYC divide by 64
        T_pwm_timer = T_CYC * 64;
        T_pwm = maxCount * T_pwm_timer; // use max. possible time
    }
    

    T_pwm_count = T_pwm/T_pwm_timer; // calc count value for P1TPER register (implicit cast to unsigned int) 
    P1TPER = T_pwm_count;   // set count value of PWM timer (15 bit register)
    PWM1CON1bits.PMOD1 = 1; // set PWM unit 1 to independent mode
    
    PWM1CON1bits.PEN1H = PEN1H; // enable or disable PWM driver PWM1H1
    PWM1CON1bits.PEN2H = PEN2H; // enable or disable PWM driver PWM1H2
    PWM1CON1bits.PEN3H = PEN3H; // enable or disable PWM driver PWM1H3
    PWM1CON1bits.PEN1L = PEN1L; // enable or disable PWM driver PWM1L1
    PWM1CON1bits.PEN2L = PEN2L; // enable or disable PWM driver PWM1L2
    PWM1CON1bits.PEN3L = PEN3L; // enable or disable PWM driver PWM1L3

    P1TCONbits.PTEN = 1; // Switch on PWM generator
    // Set DC to zero for all channels
    P1DC1 = 0;
    P1DC2 = 0;
    P1DC3 = 0;
}

/***************************************************************************
* Function Name     : setPWM1DCs                                           *
* Description       : Change the duty cycles of the 3 channels of PWM1.    *
* Parameters        : dcChannel1 - DC of PWM1 channel 1 (between 0 and 1)  *
*                     dcChannel2 - DC of PWM1 channel 2 (between 0 and 1)  * 
*                     dcChannel3 - DC of PWM1 channel 3 (between 0 and 1)  *
* Return Value      : None                                                 *
***************************************************************************/
void setPWM1DCs(const float dcChannel1, const float dcChannel2, const float dcChannel3)
{
    P1DC1 = dcChannel1 * 2 * P1TPER; // Set DC of channel 1 (to get 100% DC, you need to write twice the P1TPER value)
    P1DC2 = dcChannel2 * 2 * P1TPER; // Set DC of channel 2
    P1DC3 = dcChannel3 * 2 * P1TPER; // Set DC of channel 3
}