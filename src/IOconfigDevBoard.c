#include "xc.h"
#include "IOconfigDevBoard.h"

void setupIO() {
    // ---- PIN MAPPING ---- //
    // before we map, we need to unlock
    __builtin_write_OSCCONL(OSCCON & 0xbf); // clear bit 6 (unlock, they are usually write protected)

    // INPUT: Adress Peripheral, write input pin
    RPINR18bits.U1RXR = 9; // RP9 -> UART1 RX
    RPINR14bits.QEA1R = 10; // RP10 -> QEA Encoder 1
    RPINR14bits.QEB1R = 11; // RP11 -> QEB Encoder 1

    // OUTPUT: Adress Pin, write peripheral
    //see table 11-2 in datasheet to check peripheral codes 
    RPOR4bits.RP8R = 0b00011; // UART1 TX -> RP8 

    // END PIN MAPPING
    __builtin_write_OSCCONL(OSCCON | 0x40); // Lock PPS registers (lock again!)
    int i;
    for (i = 0; i < 30000; i++); // short dirty delay for changes to take effect,
    // ---- PIN MAPPING ---- //

    AD1PCFGL = 0b1111111111111111; // all pins are now digital, by default they are analogue, the ANx pins!

    // set LEDs as Output
    // TODO: use defines here instead of raw pins
    TRISBbits.TRISB15 = 0;
    TRISBbits.TRISB14 = 0;
    TRISBbits.TRISB13 = 0;
    TRISBbits.TRISB12 = 0;

    TRISBbits.TRISB8 = 0; // UART1 TX 

    TRISAbits.TRISA0 = 0;
    TRISAbits.TRISA1 = 0;

    // all LEDs off by default
    LED4 = LEDOFF;
    LED5 = LEDOFF;
    LED6 = LEDOFF;
    LED7 = LEDOFF;
}
