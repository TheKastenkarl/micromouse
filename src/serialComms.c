#include <xc.h>
#include <stdio.h>
#include "IOconfig.h"

// File shared variables
// Stores the pointer to the character which should be sent next via UART
char* uartData = NULL;
// Indicates if the UART is currently used
int uartBusy = 0;

/***************************************************************************
* Function Name     : setupUART1                                           *
* Description       : Set up UART1 (serial port). We aim to achieve a data *
*                     transfer rate of 57.6kbit/s based on Fcycle=26.67Mhz.*
*                     BaudRate=Fcycle/(16*(BRG+1))                         *
*                     => BRG = Fcy/(16*BaudRate) - 1 =                     *
*                            = 26.666Mhz/(16*57600) - 1 = 28.23            *
*                     => choose 28 => BaudRate=57.474kbit/s,               *
*                        which is ~ 1% off.                                *
*                     => for standard communication speed of 9600bit/s:   *
*                        choose 173 (factor 6)                             *
* Parameters        : None                                                 *
* Return Value      : None                                                 *
***************************************************************************/
void setupUART1()
{
	U1MODEbits.UARTEN=0; // switch the uart off during set-up
	U1BRG=173;//28; // baud rate register
	U1MODEbits.LPBACK=0; // in loopback mode for test! TODO: set to no loop-back (=0) after test
                         // good test characters: 0xAA or 0x55 (ones and zeros after each other)
	
	U1MODEbits.WAKE=0; //do not wake up on serial port activity

	U1MODEbits.ABAUD=0; //no auto baud rate detection
	U1MODEbits.PDSEL=0; //select 8 bits date, no parity
	U1MODEbits.STSEL=0; //one stop bit
    U1MODEbits.BRGH=0;  // No High Speed Mode

	IFS0bits.U1RXIF=0; //reset the receive interrupt flag
	IFS0bits.U1TXIF=0; //reset the transmission interrupt flag
    
	IPC2bits.U1RXIP=3; //set the RX interrupt priority
	IPC3bits.U1TXIP=5; //set the TX interrupt priority

	U1STAbits.URXISEL=0; //generate a receive interrupt as soon as a character has arrived
	//U1STAbits.UTXEN=1; //enable the transmission of data

	IEC0bits.U1RXIE=1; //enable the receive interrupt
	IEC0bits.U1TXIE=1; //enable the transmit interrupt

	//FINALLY, 
	U1MODEbits.UARTEN=1; //switch the uart on
  	U1STAbits.UTXEN=1; //enable transmission
	
    //U1MODE = 0x8000; /* Reset UART to 8-n-1, alt pins, and enable */
	//U1STA  = 0x0440; /* Reset status register and enable TX & RX*/
}

/***************************************************************************
* Function Name     : _U1RXInterrupt                                       *
* Description       : ISR of UART1 when data is received.                  *
* Parameters        : None                                                 *
* Return Value      : None                                                 *
***************************************************************************/
void __attribute__((interrupt, no_auto_psv)) _U1RXInterrupt(void)
{	
	unsigned int rxData; // a local buffer to copy the data into

	// Set the UART1 receiving interrupt flag to zero
 	IFS0bits.U1RXIF=0;

    LED7 = LEDOFF;
	
	// Read out the data
	rxData=U1RXREG; // Franz: reads first byte of 4byte-FIFO register and automatically removes it from the FIFO
    
    // Copy it back out to UART (for testing))
    U1TXREG=rxData;
    //wait until the character is gone...

	//we should also clear the overflow bit if it has been set (i.e. if we were to slow to read out the fifo)
	U1STAbits.OERR=0; //we reset it all the time
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

/***************************************************************************
* Function Name     : _U1TXInterrupt                                       *
* Description       : ISR of UART1 when data is transmitted. Is used in    *
*                     combination with putsUART1 to send data.             *
* Parameters        : None                                                 *
* Return Value      : None                                                 *
***************************************************************************/
void __attribute__((interrupt, no_auto_psv)) _U1TXInterrupt(void)
{	
    // Set the UART1 transmitting interrupt flag to zero
 	IFS0bits.U1TXIF = 0;
    // transfer data to buffer until buffer if end of char string has not been
    // reached (U1STAbits.UTXBF = 1 if buffer is full))
    if (*uartData != '\0')
    {
        U1TXREG = *uartData++;    /* transfer data word to TX reg */
    }
    // add a linebreak after every data string 
    else if (uartBusy)
    {
        U1TXREG = '\n';
        uartBusy = 0;
    }
}

/***************************************************************************
* Function Name     : putsUART1                                            *
* Description       : This function puts the data string to be transmitted *
*                     into the transmit buffer (till NULL character). If   *
*                     the data string is longer than the transmit buffer,  *
*                     the TX interrupt and the file global variable        *
*                     uartData are used to send the remaining characters.  *
* Parameters        : dataStr - address of the string buffer to be         *  
*                     transmitted                                          *
*                     waitIfBusy - If true the function waits until the    *
*                     previous data is completely sent, else not.          *
* Return Value      : None                                                 *
***************************************************************************/
void putsUART1(char* dataStr, int waitIfBusy)
{
    uartData = dataStr;
    if (waitIfBusy)
    {
        while(uartBusy);
    }
    uartBusy = 1;
    // transfer data to buffer until buffer if full or until end of char string
    // is reached (U1STAbits.UTXBF = 1 if buffer is full))
    while(*uartData != '\0' && !U1STAbits.UTXBF)
    {
        U1TXREG = *uartData++;    /* transfer data word to TX reg */
    }
}