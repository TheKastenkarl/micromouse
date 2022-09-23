/// Configuration Bits

// FBS
#pragma config BWRP = WRPROTECT_OFF     // Boot Segment Write Protect (Boot Segment may be written)
#pragma config BSS = NO_FLASH           // Boot Segment Program Flash Code Protection (No Boot program Flash segment)

// FGS
#pragma config GWRP = OFF               // General Code Segment Write Protect (User program memory is not write-protected)
#pragma config GSS = OFF                // General Segment Code Protection (User program memory is not code-protected)

// FOSCSEL
#pragma config FNOSC = FRC                 // Start with Internal RC Oscillator
#pragma config IESO = OFF                // Internal External Switch Over Mode (Start-up device with FRC, then automatically switch to user-selected oscillator source when ready)

// FOSC
#pragma config POSCMD = HS             // Primary Oscillator Source (HS Oscillator Mode)
#pragma config OSCIOFNC = OFF           // OSC2 Pin Function (OSC2 pin has clock out function)
#pragma config IOL1WAY = OFF            // Peripheral Pin Select Configuration (Allow Multiple Re-configurations)
#pragma config FCKSM = CSECMD           // Clock Switching and Monitor (Both Clock Switching and Fail-Safe Clock Monitor are enabled)

// FWDT
#pragma config WDTPOST = PS1            // Watchdog Timer Postscaler (1:1)
#pragma config WDTPRE = PR128           // WDT Prescaler (1:128)
#pragma config WINDIS = OFF             // Watchdog Timer Window (Watchdog Timer in Non-Window mode)
#pragma config FWDTEN = OFF             // Watchdog Timer Enable (Watchdog timer enabled/disabled by user software)

// FPOR
#pragma config FPWRT = PWR128           // POR Timer Value (128ms)
#pragma config ALTI2C = ON             // Alternate I2C  pins (I2C mapped to SDA1/SCL1 pins)
#pragma config LPOL = ON                // Motor Control PWM Low Side Polarity bit (PWM module low side output pins have active-high output polarity)
#pragma config HPOL = ON                // Motor Control PWM High Side Polarity bit (PWM module high side output pins have active-high output polarity)
#pragma config PWMPIN = ON              // Motor Control PWM Module Pin Mode bit (PWM module pins controlled by PORT register at device Reset)

// FICD
#pragma config ICS = PGD1               // Comm Channel Select (Communicate on PGC1/EMUC1 and PGD1/EMUD1)
#pragma config JTAGEN = OFF             // JTAG Port Enable (JTAG is Disabled)

/// Include headers

#include "xc.h"
#include "IOconfig.h"
// #include "IOconfigDevBoard.h"
#include "osciallator.h"
#include "pwm.h"
#include "timer.h"
#include "serialComms.h"
#include "motorEncoders.h"
#include "adc.h"
#include "dma.h"
#include "motors.h"
#include "tests.h"
#include "utils.h"
#include "robot.h"
#include "maze_solver.h"

void defaultState() {
    runMotor(0.0, 0, 0);
    runMotor(0.0, 1, 0);
    
    LED0 = LEDOFF;
    LED1 = LEDOFF;
    LED2 = LEDOFF;
}

void setupMotors() {
    initQEI1(0); // left motor encoder
    initQEI2(0); // right motor encoder
    setupPWM(0.05); // 0.05 ms = 20 kHz
}

void setupIRSensors() {
    setupADC1();
    initDmaChannel4();
    startADC1();
}

void setupBluetooth() {
    setupUART1(BAUDRATE);
}

void setupTimer() {
    initTimer1(50); // 50 ms timer
    startTimer1();
}

void setup() {
    setupOscillator();
    setupIO();
    setupTimer();
    setupBluetooth();
    setupIRSensors();
    setupMotors();
    setupUART1(BAUDRATE);
    
    defaultState();
}

void loop() {
    while (1) {
        //testButtonAndLed(1, 0);
        //testBluetoothUART();
        //testIR(1, 1.0F);
        //testIR(0, 1.0F);
        //testMotor(1);
        //testMotor(0);
        //testEncoder(0);
        //sleep(2000000);
    }
}

int main() {
#if !SIMULATION
    sendUART1("START MAIN", 1);
#endif
    setup();
    //loop();
    sleep(5 * 4000000); // around 10 sec
    explore_and_exploit();
    //wall_follow();
    return 0;
}