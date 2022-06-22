/* 
 * File:   
 * Author: 
 * Comments:
 * Revision history: 
 */

// This is a guard condition so that contents of this file are not included
// more than once.  
#ifndef SERIALCOMMS_H
#define	SERIALCOMMS_H

#include <xc.h> // include processor files - each processor file is guarded.  

// Set up UART1 (serial port) with data transfer rate of 57.6kbit/s.
void setupUART1();

// This function puts the data string to be transmitted into the transmit buffer.
void putsUART1(char* dataStr, int waitIfBusy);

#endif	/* SERIALCOMMS_H */
