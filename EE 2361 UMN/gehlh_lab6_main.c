/*
 * File:   gehlh_lab6_main.c
 * Author: gehlh
 *
 * Created on March 27, 2021, 4:57 PM
 */

#include "xc.h"
#include "string.h"
#include "stdint.h"
#include <stdio.h>

#pragma config ICS = PGx1          // Comm Channel Select (Emulator EMUC1/EMUD1 pins are shared with PGC1/PGD1)
#pragma config FWDTEN = OFF        // Watchdog Timer Enable (Watchdog Timer is disabled)
#pragma config GWRP = OFF          // General Code Segment Write Protect (Writes to program memory are allowed)
#pragma config GCP = OFF           // General Code Segment Code Protect (Code protection is disabled)
#pragma config JTAGEN = OFF        // JTAG Port Enable (JTAG port is disabled)


// CW2: FLASH CONFIGURATION WORD 2 (see PIC24 Family Reference Manual 24.1)
#pragma config I2C1SEL = PRI       // I2C1 Pin Location Select (Use default SCL1/SDA1 pins)
#pragma config IOL1WAY = OFF       // IOLOCK Protection (IOLOCK may be changed via unlocking seq)
#pragma config OSCIOFNC = ON       // Primary Oscillator I/O Function (CLKO/RC15 functions as I/O pin)
#pragma config FCKSM = CSECME      // Clock Switching and Monitor (Clock switching is enabled, 
                                       // Fail-Safe Clock Monitor is enabled)
#pragma config FNOSC = FRCPLL      // Oscillator Select (Fast RC Oscillator with PLL module (FRCPLL))

#define BUFFER_SIZE 128

void delay(unsigned int ms) { //delay function
    int i;
    for (i = 0; i < ms; i++) {
        asm("repeat #15993");
        asm("nop");
    }
    return;
}

void lcd_cmd(char command) {
    I2C2CONbits.SEN = 1;    // Begin start sequence 
    while(I2C2CONbits.SEN);
    IFS3bits.MI2C2IF = 0;   //reset interrupt
    I2C2TRN = 0b01111100;   // Slave address 
    while(!IFS3bits.MI2C2IF); 
    IFS3bits.MI2C2IF = 0;   //reset interrupt
    I2C2TRN = 0b00000000;   // Control bytes
    while(!IFS3bits.MI2C2IF);
    IFS3bits.MI2C2IF = 0;    //reset interrupt
    I2C2TRN = command;      //data byte
    while(!IFS3bits.MI2C2IF);
    IFS3bits.MI2C2IF = 0;    //reset interrupt
    I2C2CONbits.PEN = 1;
    while(I2C2CONbits.PEN); 
}

void lcd_init(void)
{   delay(50);
    lcd_cmd(0b00111000); // function set, normal instruction mode
    lcd_cmd(0b00111001); // function set, extended instruction mode
    lcd_cmd(0b00010100); // interval osc
    lcd_cmd(0b01110000); // contrast C3-C0 
    lcd_cmd(0b01011110); // Ion, Bon, C5-C4 
    lcd_cmd(0b01101100); // follower control
    delay(200);
    lcd_cmd(0b00111000); // function set, normal instruction mode
    lcd_cmd(0b00001100); // Display On
    lcd_cmd(0b00000001); // Clear Display
    delay(2);
}



void lcd_setCursor(char x, char y)
{
    char location = (x+(y*0x40));
    char cmd = (location + (1<<7));
    lcd_cmd(cmd);
}

void lcd_printChar(char myChar)
{
    I2C2CONbits.SEN = 1;    //start
    while(I2C2CONbits.SEN);
    IFS3bits.MI2C2IF = 0;   //reset interrupt
    I2C2TRN = 0b01111100;   // Slave address and R/nW bit
    while(!IFS3bits.MI2C2IF); 
    IFS3bits.MI2C2IF = 0;
    I2C2TRN = 0b01000000;   // Control bytes
    while(!IFS3bits.MI2C2IF);
    IFS3bits.MI2C2IF = 0;  //reset interrupt
    I2C2TRN = myChar;      //data byte
    while(!IFS3bits.MI2C2IF);
    IFS3bits.MI2C2IF = 0;   //reset interrupt
    I2C2CONbits.PEN = 1;
    while(I2C2CONbits.PEN);
}

void lcd_printStr(const char s[]) {
    int i = 0;
    int stringLength = strlen(s);
    
    I2C2CONbits.SEN = 1; // Start
    while(I2C2CONbits.SEN);
    IFS3bits.MI2C2IF = 0;
    I2C2TRN = 0b01111100; // Slave address bits and r/w bit
    while(!IFS3bits.MI2C2IF);//wait for interrupt flag
    IFS3bits.MI2C2IF = 0;
    
    
    for (i = 0; i < (stringLength); i++) 
    {
        I2C2TRN = 0b11000000; //control byte and RS set to 1
        while(!IFS3bits.MI2C2IF);//wait for interrupt flag
        IFS3bits.MI2C2IF = 0;
        I2C2TRN = s[i]; //data for char
        while(!IFS3bits.MI2C2IF); //wait for interrupt flag
        IFS3bits.MI2C2IF = 0;   
    }
    
    I2C2TRN = 0b01000000; // Final byte, Co = 0 & Rs = 1;
    while(!IFS3bits.MI2C2IF);//wait for interrupt flag
    IFS3bits.MI2C2IF = 0;
    I2C2CONbits.PEN = 1; // Stop
    while(I2C2CONbits.PEN);
}

void right(void) {
    lcd_cmd(0b00011100);
}

void left(void){
    lcd_cmd(0b00011000);
}
/////////////////////////////////////////////////////////////////////////////////
//lab 6 functions
//////////////////////////////////////////////////////////////////////////////////
long buffer[BUFFER_SIZE];    
int buffIdx = 0;
int j;
void putVal(int newValue) {
    buffer[buffIdx++] = newValue;
    if (buffIdx >= BUFFER_SIZE) { // Reset if buffer size exceeded
        buffIdx = 0;
    }
}

int getAvg() {
    double sum = 0;
    int i;
    for (i = 0; i < BUFFER_SIZE; i++) {
        sum = sum + buffer[i]; // Add up all the values in the buffer
    }
    
    return sum/BUFFER_SIZE; // Return avg
}

void initBuffer() {
    int i = 0;
    for (i = 0; i < BUFFER_SIZE; i++) {
        buffer[i] = 0;
    }
}

void setup() {
    //analog setup
    CLKDIVbits.RCDIV = 0; //freq
    AD1PCFG = 0xFFFE; //set AN0 as analog
    
    TRISA |=1; //set input pin
    
    //LCD setup 
    I2C2CONbits.I2CEN = 0;
    I2C2BRG = 0x9D; // 100kHz
    I2C2CONbits.I2CEN = 1; //enable
    _I2CSIDL = 0;
    _MI2C2IF = 0; 
    lcd_init();
    initBuffer();
    lcd_setCursor(0,0);
    
    // adc setup
    AD1CON2bits.VCFG = 0b000; //voltage setup
    AD1CON3bits.ADCS = 0b00000001; //sample rate set up
    AD1CON1bits.SSRC = 0b010; //source select
    AD1CON3bits.SAMC = 0b00;
    AD1CON1bits.FORM = 0b00; //integer 
    
    AD1CON1bits.ASAM = 0b1;
    AD1CON2bits.SMPI = 0b1; //16 samples/s
    AD1CON1bits.ADON = 0b1; //A/D operating mode bit
    
    _AD1IF = 0; //reset interrupt flag
    _AD1IE = 1; //enable interrupt flag
   
    //adc timer setup  
    TMR3 = 0;
    T3CON = 0;
    T3CONbits.TCKPS = 0b01; //64
    PR3 = 15624;
    T3CONbits.TON = 1;
 
    //LCD timer setup
    TMR2 = 0;
    T2CON = 0;
    T2CONbits.TCKPS = 0b10;
    PR2 = 24999;
    T2CONbits.TON = 1; 
    _T2IE =1;
}

void __attribute__((interrupt,auto_psv)) _ADC1Interrupt()
{
 _AD1IF = 0;
 unsigned long int value;
 value = ADC1BUF0;
 putVal(value);
}

void __attribute__((interrupt, auto_psv)) _T2Interrupt()
{
 j=1;
 _T2IF = 0;
 TMR2=0;

}

    int main(void) {
    setup();
    lcd_cmd(0b00000001);
    lcd_setCursor(0,5);
    lcd_printStr("abcd");
    while(1)
    { 
        
    if(j==1)
    {
        j=0;
        lcd_cmd(0b00000001);
        lcd_setCursor(0,0);
        int adValue;
        char adStr[20];
        adValue = getAvg();
        sprintf(adStr, "%6.4f V", (3.3 / 1024) * adValue);
        lcd_printStr(adStr);
    }
   
    }
    return 1;
}
