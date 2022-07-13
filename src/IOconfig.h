#ifndef IOCONFIG_H
#define	IOCONFIG_H

/// OUTPUTS 

// use latch for writing to outputs, pin numbers from board description
#define LED0 LATBbits.LATB10
#define LED1 LATBbits.LATB11
#define LED2 LATBbits.LATB12
#define M1_PLUS_EN LATAbits.LATA10
#define M1_MINUS_EN LATAbits.LATA7
#define M2_PLUS_EN LATAbits.LATA8
#define M2_MINUS_EN LATBbits.LATB4

// directly define constants for tristate to not forget them
#define LED0_TRIS TRISBbits.TRISB10
#define LED1_TRIS TRISBbits.TRISB11
#define LED2_TRIS TRISBbits.TRISB12
#define M1_PLUS_EN_TRIS TRISAbits.TRISA10
#define M1_MINUS_EN_TRIS TRISAbits.TRISA7
#define M2_PLUS_EN_TRIS TRISAbits.TRISA8
#define M2_MINUS_EN_TRIS TRISBbits.TRISB4

// motors pwm duty cycles
#define M1_DC P1DC1 // PWM1L1, PWM1H1
#define M2_DC P1DC2 // PWM1L2, PWM1H2

/// INPUTS

// use port for reading from inputs
#define BTN1 PORTBbits.RB7 
#define BTN2 PORTBbits.RB6

/// VALUES

// since active low LED
#define LEDON 0
#define LEDOFF 1

// buttons
#define BTN_PRESSED 1

void setupIO();

#endif	/* IOCONFIG_H */

