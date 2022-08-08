#include <xc.h>
#include "IOconfig.h"
// #include "IOconfigDevBoard.h"
#include <stdbool.h>
#include "serialComms.h"
#include <math.h>
#include "utils.h"


char* g_sendBuffer; // global variable that points to message to be sent
bool g_isBusy = false; // global variable to indicate that we are waiting to send

/**
 * Calculate value for BRG given the desired baudrate and the cycle frequency
 * 
 * @param baudrate: desired baudrate, typical values: 9600, 28800, 57600 bit/s
 * @param fCycle: cycle frequency of board
 * @return value for BRG register
 */
unsigned char getBRG(unsigned short baudrate, float fCycle) {
    unsigned char r = round(fCycle / (16. * baudrate) - 1);
    return r;
}

/**
 * Setup UART1 to send and receive serial messages.
 * 
 * @param baudrate: Desired baudrate, typical values: 9600, 28800, 57600 bit/s
 */
void setupUART1(unsigned short baudrate) {
    // switch off
    U1MODEbits.UARTEN = 0; // switch the uart off during set-up

    // baudrate related
    U1MODEbits.ABAUD = 0; // no auto baud rate detection
    U1BRG = getBRG(9600, INTERNAL_OSC_FREQ_HZ); // 173 for 9600
    U1MODEbits.BRGH = 0; // No High Speed Mode

    // others
    U1MODEbits.LPBACK = 0; // in loopback mode for test! TODO: set to no loop-back (=0) after test 

    // message protocol
    U1MODEbits.PDSEL = 0; // select 8 bits date, no parity
    U1MODEbits.STSEL = 0; // one stop bit

    // interrupts
    IFS0bits.U1RXIF = 0; // reset the receive interrupt flag
    IFS0bits.U1TXIF = 0; // reset the transmission interrupt flag
    IPC2bits.U1RXIP = 3; // set the RX interrupt priority
    IPC3bits.U1TXIP = 5; // set the TX interrupt priority
    U1STAbits.UTXISEL1 = 0;
    U1STAbits.UTXISEL0 = 0;
    U1STAbits.URXISEL = 0; // generate a receive interrupt as soon as a character has arrived
    IEC0bits.U1RXIE = 1; // enable the receive interrupt
    IEC0bits.U1TXIE = 1; // enable the transmit interrupt

    // turn on 
    U1MODEbits.UARTEN = 1; // switch the uart on
    U1STAbits.UTXEN = 1; // enable transmission
}


// Receive interrupt as soon as one character has arrived

void __attribute__((interrupt, no_auto_psv)) _U1RXInterrupt(void) {
    unsigned int rxData; // a local buffer to copy the data into

    IFS0bits.U1RXIF = 0; // reset receiving interrupt

    // actions
    rxData = U1RXREG; // read first byte in queue
    // U1TXREG = rxData; // and copy it back out to UART
    //wait until the character is gone...

    //we should also clear the overflow bit if it has been set (i.e. if we were to slow to read out the fifo)
    U1STAbits.OERR = 0; // we reset it all the time
    //some notes on this from the data sheet
    /*
    If the FIFO is full (four characters) and a fifth character is fully received into the UxRSR register,
    the overrun error bit, OERR (UxSTA<1>), will be set. The word in UxRSR will be kept, but further
    transfers to the receive FIFO are inhibited as long as the OERR bit is set. The user must clear
    the OERR bit in software to allow further data to be received.
    If it is desired to keep the data received prior to the overrun, the user should first read all five
    characters, then clear the OERR bit. If the five characters can be discarded, the user can simply
    clear the OERR bit. This effectively resets the receive FIFO and all prior received data is lost.

    The data in the receive FIFO should be read prior to clearing the OERR bit. The
    FIFO is reset when OERR is cleared, which causes all data in the buffer to be lost.
     */
}

// Transmit Interrupt

void __attribute__((interrupt, no_auto_psv)) _U1TXInterrupt(void) {
    static char del[] = DELIMITER;
    static int delIndex = 0;

    IFS0bits.U1TXIF = 0; // reset the transmitted interrupt flag

    // after final char has been sent
    if (!g_isBusy) {
        return;
    }

    while (U1STAbits.UTXBF); // wait if transmit buffer is full

    if (*g_sendBuffer != '\0') {
        // transfer next character
        U1TXREG = *g_sendBuffer;
        g_sendBuffer++;
    } else if (del[delIndex] != '\0') {
        // at end of string: send delimiters next
        U1TXREG = del[delIndex];
        delIndex++;
    } else {
        // at end of delimiters
        delIndex = 0; // reset for next run
        g_isBusy = false;
    }
}

void sendUART1(char* buffer, int waitIfBusy) {
    if (waitIfBusy) {
        while (g_isBusy);
    }

    g_sendBuffer = buffer; // set global variable to point to string to be sent
    g_isBusy = true;

    // need to disable interrupts to be able to increment the pointer before
    // jumping into the transmitt ISF
    while (U1STAbits.UTXBF); // wait if transmit buffer is full
    IEC0bits.U1TXIE = 0;
    U1TXREG = *g_sendBuffer; // transfer first character
    g_sendBuffer++;
    IEC0bits.U1TXIE = 1;
}