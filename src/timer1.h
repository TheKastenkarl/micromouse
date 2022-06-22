/* 
 * File:   
 * Author: Franz Kaschner
 * Comments:
 * Revision history: 
 */

// This is a guard condition so that contents of this file are not included
// more than once.  
#ifndef __TIMER1_H__
#define	__TIMER1_H__

#include <xc.h> // include processor files - each processor file is guarded.  

// Initialize timer1.
void initTimer1(const float timeInMs);

// Start timer1.
void startTimer1(); 

#endif	/* TIMER1_H */

