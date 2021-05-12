/*
 * File:   gehlh_lab4_v002_main.c
 * Author: gehlh
 *
 * Created on March 8, 2021, 2:49 PM
 */

#include <xc.h>

// CONFIG2
#pragma config POSCMOD = NONE           // Primary Oscillator Select (Primary oscillator disabled)
#pragma config I2C1SEL = PRI            // I2C1 Pin Location Select (Use default SCL1/SDA1 pins)
#pragma config IOL1WAY = OFF            // IOLOCK Protection (IOLOCK may be changed via unlocking seq)
#pragma config OSCIOFNC = OFF           // Primary Oscillator Output Function (OSC2/CLKO/RC15 functions as CLKO (FOSC/2))
#pragma config FCKSM = CSECME           // Clock Switching and Monitor (Clock switching is enabled, Fail-Safe Clock Monitor is enabled)
#pragma config FNOSC = FRCPLL           // Oscillator Select (Fast RC Oscillator with PLL module (FRCPLL))
#pragma config SOSCSEL = SOSC           // Sec Oscillator Select (Default Secondary Oscillator (SOSC))
#pragma config WUTSEL = LEG             // Wake-up timer Select (Legacy Wake-up Timer)
#pragma config IESO = ON                // Internal External Switch Over Mode (IESO mode (Two-Speed Start-up) enabled)

#pragma config WDTPS = PS32768          // Watchdog Timer Postscaler (1:32,768)
#pragma config FWPSA = PR128            // WDT Prescaler (Prescaler ratio of 1:128)
#pragma config WINDIS = ON              // Watchdog Timer Window (Standard Watchdog Timer enabled,(Windowed-mode is disabled))
#pragma config FWDTEN = OFF             // Watchdog Timer Enable (Watchdog Timer is disabled)
#pragma config ICS = PGx1               // Comm Channel Select (Emulator EMUC1/EMUD1 pins are shared with PGC1/PGD1)
#pragma config GWRP = OFF               // General Code Segment Write Protect (Writes to program memory are allowed)
#pragma config GCP = OFF                // General Code Segment Code Protect (Code protection is disabled)
#pragma config JTAGEN = OFF             // JTAG Port Enable (JTAG port is disabled)

#define BUFFER_SIZE 2

volatile unsigned long int buffer[BUFFER_SIZE] = {0,0}; // The toggling array used as a buffer
volatile unsigned long int curEdge = 0, prevEdge = 0, edgeDiff = 0, curPeriod = 0; // Measurements of edges for button debouncing
volatile unsigned long int prevEdge2 = 0;
volatile unsigned long int count = 0; // Track button presses
int i = 0;

volatile unsigned int overflow1 = 0;

void setup(void) {
    CLKDIVbits.RCDIV = 0; //  set clock divisions to 8 MHz
    AD1PCFG = 0x9fff; // For digital I/O.  If you want to use analog, you'll
    // need to change this.
}

// Function: initPushButton
// Sets up Timer 2 and Input Capture 1
void initPushButton(void) {
    CLKDIVbits.RCDIV = 0;
    CNPU2bits.CN22PUE = 1; // Pull up resistor
    AD1PCFG = 0x9fff;
    TRISA = 0xFFFF;
    TRISBbits.TRISB8 = 1;
    
    __builtin_write_OSCCONL(OSCCON & 0xbf); // unlock PPS
    RPINR7bits.IC1R = 8; // Use Pin RP8 = "8", for Input Capture 1 (Table 10-2)
    __builtin_write_OSCCONL(OSCCON | 0x40); // lock   PPS
    
    IC1CON = 0; // Off and reset IC1
    IEC0bits.IC1IE = 1;
    IC1CONbits.ICTMR = 1; 
    IC1CONbits.ICM = 0b001; // Capture every edge
    
    // Timer2
    T2CONbits.TON = 0; // Don't turn on yet
    T2CONbits.TCKPS = 0b11; // 256 PRE
    PR2 = 0xffff;
    TMR2 = 0;
    T2CONbits.TON = 1; // Turn on
    
    _T2IF = 0;
    _T2IE = 1;
}

void __attribute__((__interrupt__, __auto_psv__)) _T2Interrupt(void){ //  if TMR2 value exceeds the set PR2 value
    _T2IF = 0; //  reset interrupt flag
    overflow1++; //  increment overflow count
}

// IC1 Interrupt
// Contains debouncing method where the difference between edges must exceed a certain amount to avoid false positive button presses
void __attribute__((__interrupt__, __auto_psv__)) _IC1Interrupt(void){ //  if selected edge is detected
    IFS0bits.IC1IF = 0; //  reset IC1 flag
    curEdge = IC1BUF + (65535 * overflow1);
    edgeDiff = curEdge - prevEdge;

    // Debouncing
    if (edgeDiff > 450){
        count++;
        
        if (count == 2){
            prevEdge2 = curEdge;
            count = 0;
        }
        
        if (count == 27){   // This statement may not be necessary?
            count = 1;
        }
        
        if ((count == 1) && (prevEdge2 != 0)){
            curPeriod = curEdge - prevEdge2;
            buffer[i] = curPeriod; // Add element to the buffer
            if (i == 0){ // Buffer has 2 slots, write to buffer[0] and buffer[1], then reset
                i++;
            } else{
                i = 0;
            }
            count = 1; // Reset the count
            prevEdge2 = 0;
        }
        prevEdge = curEdge; // Reset edges to find the next correct edge
    }
}

// Function: initServo
// Sets up Timer 3 and Output Compare 1
void initServo(void) {
    CLKDIVbits.RCDIV = 0;
    
    __builtin_write_OSCCONL(OSCCON & 0xbf); // unlock PPS
    RPOR3bits.RP6R = 18; // Use Pin RP6 for Output Compare 1 = "18" (Table 10-3)
    __builtin_write_OSCCONL(OSCCON | 0x40); // lock   PPS
    
    
    // Timer3
    T3CON = 0x0000;         // Timer off, PRE 8, Tcy as clock source
    T3CONbits.TCKPS = 0b01;
    TMR3 = 0;
    PR3 = 39999;            
    T3CONbits.TON = 1;      // Timer3 on
    
    // Timer 3 setup should happen before this line
    OC1CON = 0;
    OC1R = 0000; // servo start position. We won?t touch OC1R again
    OC1RS = 0000; // We will only change this once PWM is turned on
    
    OC1CONbits.OCM = 0b110; // Output compare PWM w/o faults
    OC1CONbits.OCTSEL = 1; // Use Timer 3 for compare source
}

void setServo(int Val) {
    OC1RS = Val;
}

void main(void) {
    // Set up servo and push button
    int count = 0;
    setup();
    initServo();
    initPushButton();
    setServo(1);
    
    unsigned long int difference = 0;
    
    while(1) {
        // Initial movement after 3 presses
        if (buffer[0] < 10000 && buffer[1] < 10000) {
            setServo(3500);
            count = 25;
            buffer [0] = 300000;
            buffer[1] = 300000;

            
            // Reset everything
            overflow1 = 0;
            difference = 0;
            TMR2 = 0;
            IC1BUF = 0;
        }
        
        // Reset after initial servo movement
        if (difference >= 2) {
            setServo(2000);
            difference = 0;
        }
    }
    
}