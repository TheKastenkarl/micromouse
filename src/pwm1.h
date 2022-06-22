/* 
 * File:   
 * Author: Franz Kaschner
 * Comments:
 * Revision history: 
 */

// This is a guard condition so that contents of this file are not included
// more than once.  
#ifndef __PWM1_H__
#define	__PWM1_H__

#include <xc.h> // include processor files - each processor file is guarded.

// Set up PWM1 (independent mode). Use set_PWM1_DCs to set the duty cycles (here the DCs are all set to 0).
void setupPWM1(const float f_pwm, const int PEN1H, const int PEN1L, const int PEN2H, const int PEN2L, const int PEN3H, const int PEN3L);

// Change the duty cycles of the 3 channels of PWM1.
void setPWM1DCs(const float dcChannel1, const float dcChannel2, const float dcChannel3);

#endif	/* PWM1_H */

