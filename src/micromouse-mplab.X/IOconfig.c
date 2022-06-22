#include <xc.h>
#include "IOconfig.h"

/***************************************************************************
* Function Name     : setupIO                                              *
* Description       : Set up the input and output ports.                   *
* Parameters        : None                                                 *
* Return Value      : None                                                 *
***************************************************************************/
void setupIO()
{
    int i;
    AD1PCFGL=0xFFFF; // all pins are now digital, by default they are analogue -> change if you want to use them as analogue input pins for ADC
    
    // Set AN5 (=IO6=RB3=potentiometer) as analogue input
    AD1PCFGLbits.PCFG5 = 0;
    
    // Set AN0 as analogue input
    AD1PCFGLbits.PCFG0 = 0;
    
    // set LEDs as output (by default all port pins are inputs)
    TRISBbits.TRISB15 = 0;
    TRISBbits.TRISB14 = 0;
    TRISBbits.TRISB13 = 0;
    TRISBbits.TRISB12 = 0;
    
    TRISBbits.TRISB8 = 0; // UART1 TX

    // PIN MAPPING
    // before we map, we need to unlock
    __builtin_write_OSCCONL(OSCCON & 0xbf); // clear bit 6 (unlock, they are usually write protected)
    
    // PERIPHERAL receives data from which INPUT  
    RPINR18bits.U1RXR = 9; // mapped to RP9 is U1 RX

    // PERIPHERAL QEA Encoder 1, receives data from RP10
    RPINR14bits.QEA1R = 10; 
    // PERIPHERAL QEB Encoder 1, receives data from RP11
    RPINR14bits.QEB1R = 11;
    
    // OUTPUT PIN receives data from which PERIPHERAL, 
    // see table 11-2 in datasheet to check peripheral codes 
    RPOR4bits.RP8R = 0b00011; // output bin RP8 gets data from peripheral U1 TX 

    // after mapping we lock again
     __builtin_write_OSCCONL(OSCCON | 0x40); // Lock PPS registers (lock again!)
     
    for (i = 0; i < 30000; i++); // short dirty delay for changes to take effect    
}