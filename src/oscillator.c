#include "xc.h"
#include "osciallator.h"

// Oscialltor module.

/**
 * Setup the osciallator.
 */
void setupOscillator() {
#if (SEVEN_MEG_OSC == 0) 
    /*** oscillator setup --------------------------------------------------
     * The external oscillator runs at 16MHz
     * PLL is used to generate 53.3 MHz clock (FOSC)
     * The relationship between oscillator and cycle frequency: FCY = FOSC/2
     * Have a look at "PLL Configuration" paragraph in the mcu manual

     * Result: FCY = 0.5 * (16MHz*20/(3*2)) = 26.666 MIPS, Tcycle=37.5nsec
   ---------------------------------------------------------------------***/
    PLLFBDbits.PLLDIV = 18; // set PPL to M=20 (18+2)
    CLKDIVbits.PLLPRE = 1; // N1 = input/3
    CLKDIVbits.PLLPOST = 0; // N2 = output/2

#else //Below the 7.3728 Setup 
    /*** oscillator setup --------------------------------------------------
     * The external oscillator runs at 7.3728 MHz
     * PLL is used to generate 53.3 MHz clock (FOSC)
     * The relationship between oscillator and cycle frequency: FCY = FOSC/2
     * Have a look at "PLL Configuration" paragraph in the mcu manual

     * Result: FCY = 0.5 * (7.3728 MHz*29/(2*2)) = 26.73 MIPS, which is 
     * not exactl Tcycle=37.5nsec, but close enough for our purposes
   ---------------------------------------------------------------------***/
    PLLFBDbits.PLLDIV = 27; // set PPL to M=29 (27+2)
    CLKDIVbits.PLLPRE = 0; // N1 = input/2
    CLKDIVbits.PLLPOST = 0; // N2 = output/2
#endif //SEVEN_MEG_OSC == 0


    /* Clock switch to incorporate PLL*/
    __builtin_write_OSCCONH(0x03); // Initiate Clock Switch to Primary

    // Oscillator with PLL (NOSC=0b011)
    __builtin_write_OSCCONL(OSCCON || 0x01); // Start clock switching

    while (OSCCONbits.COSC != 0b011);

    // In reality, give some time to the PLL to lock
    while (OSCCONbits.LOCK != 1); // Wait for PPL to lock
}
