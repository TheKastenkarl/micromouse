#include "dma.h"
#include "IOconfig.h"

unsigned int adcData[32]__attribute__((space(dma)));

/***************************************************************************
* Function Name     : initDmaChannel4                                      *
* Description       : Configure channel 4 of the DMA for usage with the    *
*                     ADC1 peripheral.                                          * 
* Parameters        : None                                                 * 
* Return Value      : None                                                 *
***************************************************************************/
void initDmaChannel4()
{
	//DMA4CON = 0x;
	DMA4CONbits.CHEN = 0;       // Disable channel
	DMA4CONbits.SIZE = 0;       // Data transfer size (1=byte,0=word)
	DMA4CONbits.DIR	= 0;        // Transfer direction (1=read RAM,write to Periph. 0= read from periph, write to RAM)
	DMA4CONbits.HALF = 0;       // Early block transfer complete interrupt (1=interrupt on half block transfer,0=int on full block transfer)
	DMA4CONbits.NULLW = 1;      // Null Data write to peripheral mode (Null data write to peripheral as well as write ro RAM, 0=normal)
	DMA4CONbits.AMODE = 0b10;	// DMA channel operating mode	2,3=Peripheral Indirect Addressing mode, 1=Register Indirect without Post-Increment mode, 0=Register Indirect with Post-Increment mode
	DMA4CONbits.MODE = 0;       // DMA channel operating mode select 0=Continuous, Ping-Pong modes disabled, 2=continuous, ping-pong

	DMA4REQbits.FORCE = 0;      // Force DMA Transfer (1=single DMA transfer,0=automatic initiated by DMA request)
	DMA4REQbits.IRQSEL = 13;    // DMA Peripheral IRQ number select (ADC1)

	DMA4STA = (__builtin_dmaoffset(&(adcData[0]))); // start address of DMA RAM
	DMA4PAD = (volatile unsigned int) &ADC1BUF0;	// address of peripheral sfr (0x0300)
	DMA4CNT = 1; // we have 2 a2d  s/h channels for  measurement !!!CHANGE HERE!!! (franz: set "number of selected channels in AD1CSSL - 1")

    // Interrupt settings
	IFS2bits.DMA4IF	= 0;	// Clear DMA interrupt
	IEC2bits.DMA4IE	= 0;	// disable interrupt
	IPC11bits.DMA4IP = 5;	// set priority of interrupt

	DMA4CONbits.CHEN = 1;	// enable channel
}

/***************************************************************************
* Function Name     : _DMA4Interrupt                                       *
* Description       : ISR of DMA4. Gets triggered by ADC1 interrupt bit.   *
*                     Interrupt is sent after each channel is sampled,     *
*                     not after all channels were sampled. (really true?   *
*                     see page 281 of datasheet: "If more than one conver- *
*                     sion result needs to be buffered before triggering   *
*                     an interrupt, DMA data transfers can be used." ???   *
* Parameters        : None                                                 *
* Return Value      : None                                                 *
***************************************************************************/
void __attribute__((interrupt, auto_psv)) _DMA4Interrupt()
{
	IFS2bits.DMA4IF = 0; // Clear DMA interrupt
}


/*
 Questions:
 * - How often does the DMA send an interrupt?
 * -> Can be configured (after every conversion or after conversions of all channels), but we do not use the interrupts
 * - What is the advantage of using DMA over using the ADC buffers (as described in the tutorial pdf)? 
 * -> our uC has only a one word buffer which would mean that we would need to store the values manually somewhere else via the ISR
 * -> Difference to microcontroller in "dsPICForMicromouseOverview.pdf": There no DMA exists but more ADC buffers exist.
 */