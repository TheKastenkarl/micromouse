/*
 * File:   potentiometer.c
 * Author: franz
 *
 * Created on 11 June 2022, 13:20
 */

#include "irSensors.h"
#include "IOconfig.h"
#include "pwm1.h"
#include "dma.h"

/***************************************************************************
* Function Name     : modifyBrightnessIRSensor                             *
* Description       : Use the IR sensor to modify the brightness of LED4.  *
*                     If the duty cycle is larger than 0.5, LED5 is turned *
*                     on.                                                  *
* Parameters        : None                                                 *
* Return Value      : None                                                 *
***************************************************************************/
void modifyBrightnessIRSensor()
{
    float duty_cycle = IO_1 / (float) 4095; // min. sensor value: 0; max. sensor value: 4095 (12bit)
    if (IO_1 > 4095/2.0)
    {
        LED5 = LEDON;
    }
    else
    {
        LED5 = LEDOFF;
    }
    setPWM1DCs(1-duty_cycle, 0, 0); // independent mode -> all pins are active low
}
