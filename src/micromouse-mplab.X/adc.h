/* 
 * File:   adc.h
 * Author: a2-lenz
 *
 * Created on December 10, 2018, 4:59 PM
 */

#ifndef ADC_H
#define	ADC_H

// Configure ADC1 in 12 bit mode for usage of ADC CH0 in channel scan mode. 
void setupADC1(void);

// Start ADC1.
void startADC1(void);

#endif	/* ADC_H */

