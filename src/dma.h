#ifndef DMA_H
#define	DMA_H

#include <xc.h>
    
extern unsigned int adcData[32]__attribute__((space(dma)));

void initDmaChannel4();

#define IR_LEFT adcData[0] // AN0
#define IR_RIGHT adcData[1] // AN1
#define IR_FRONT adcData[2] // AN4

#endif	/* DMA_H */
