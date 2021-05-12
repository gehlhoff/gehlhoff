/*
 * File:   gehlh_lab5_v001_main.c
 * Author: gehlh
 *
 * Created on March 14, 2021, 3:56 PM
 */

#include "xc.h"

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


void delay(unsigned int ms) { //delay function using assembly 
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

void setup(void)
{
    CLKDIVbits.RCDIV = 0; 
    AD1PCFG = 0xFFFF; 
    I2C2CONbits.I2CEN = 0;
    I2C2BRG = 0x9D; // 100kHz
    I2C2CONbits.I2CEN = 1; //enable
    _I2CSIDL = 0;
    _MI2C2IF = 0; 
    lcd_init();
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
    
    
    for (i = 0; i < (stringLength-1); i++) 
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

void right() {
    lcd_cmd(0b00011100);
}

void left(void){
    lcd_cmd(0b00011000);
}

int main(void) {
    setup();
    lcd_setCursor(0,0);
    //lcd_printChar(0b01000001);
    lcd_printStr("go gophers ");
    while(1)
    {
        delay(350);
        left();  
    }
    return 0;
}


