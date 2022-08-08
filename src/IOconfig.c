#include "xc.h"
#include "IOconfig.h"

// Configure IO.

/**
 * Map remappable pins to peripherals.
 */
void setupRemappable() {
    __builtin_write_OSCCONL(OSCCON & 0xbf); // clear bit 6 (unlock, they are usually write protected)

    // INPUT: Adress Peripheral, write input pin
    // see table 11-1 in datasheet to check peripheral codes 
    RPINR18bits.U1RXR = 9; // RP9 -> UART1 RX
    RPINR14bits.QEA1R = 24; // RP24 -> QEA Encoder 1
    RPINR14bits.QEB1R = 25; // RP25 -> QEB Encoder 1
    RPINR16bits.QEA2R = 24; // RP24 -> QEA Encoder 1
    RPINR16bits.QEB2R = 25; // RP25 -> QEB Encoder 1

    // OUTPUT: Adress Pin, write peripheral
    // see table 11-2 in datasheet to check peripheral codes 
    RPOR4bits.RP8R = 0b00011; // UART1 TX -> RP8

    __builtin_write_OSCCONL(OSCCON | 0x40); // Lock PPS registers (lock again!)

    // short dirty delay for changes to take effect
    int i;
    for (i = 0; i < 30000; i++);
}

/**
 * Setup analog pins.
 */
void setupAnalog() {
    AD1PCFGL = 0b1111111111101100; // AN0, AN1, AN4 are analog inputs
}

/**
 * Setup digital pins.
 */
void setupDigital() {
    // LEDs
    LED0_TRIS = 0;
    LED1_TRIS = 0;
    LED2_TRIS = 0;
    
    // H-Bridge logic
    M1_PLUS_EN_TRIS = 0;
    M1_MINUS_EN_TRIS = 0;
    M2_PLUS_EN_TRIS = 0;
    M2_MINUS_EN_TRIS = 0;

    // UART1 TX 
    TRISBbits.TRISB8 = 0;
}

void setupIO() {
    setupRemappable();
    setupAnalog();
    setupDigital();
}