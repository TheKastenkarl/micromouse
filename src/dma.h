/* 
 * File:   dma.h
 * Author: 
 *
 * Created on June 13, 2019, 11:41 AM
 */

#ifndef DMA_H
#define	DMA_H

#include <xc.h>
    
extern unsigned int adcData[32]__attribute__((space(dma)));

// Configure channel 4 of the DMA for usage with the ADC1 peripheral.
void initDmaChannel4();

//add some defines to make accessing data more readable
#define IO_1 adcData[0] // AN0
#define TEST_SENSOR adcData[1] // AN5

#endif	/* DMA_H */
