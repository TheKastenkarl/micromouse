#ifndef IOCONFIGDEVBOARD_H
#define	IOCONFIGDEVBOARD_H

// use latch for writing to outputs, pin numbers from board description
#define LED4 LATBbits.LATB15
#define LED5 LATBbits.LATB14
#define LED6 LATBbits.LATB13
#define LED7 LATBbits.LATB12

#define M1_PLUS_EN LATAbits.LATA0 
#define M1_MINUS_EN LATAbits.LATA1
// #define M2_PLUS_EN;
// #define M2_MINUS_EN;

// use port for reading from inputs
#define SW1 !PORTBbits.RB5 

// since active low LED
#define LEDON 0
#define LEDOFF 1

void setupIODev();

#endif	/* IOCONFIGDEVBOARD_H */

