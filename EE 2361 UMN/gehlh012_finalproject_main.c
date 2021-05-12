/*
 * Author: gehlh
 *
 * Created on March 14, 2021, 3:56 PM
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



void setup(void)
{
    CLKDIVbits.RCDIV = 0; 
    AD1PCFG = 0x9FFF;
    LATA = 0xFFFF;
    TRISA = 0xFFFF;
    CNPU1bits.CN2PUE =1;
    CNPU1bits.CN3PUE = 1;
    CNPU2bits.CN30PUE =1;
    CNPU2bits.CN29PUE  =1;
    
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

void right(void) {
    lcd_cmd(0b00011100);
}

void left(void){
    lcd_cmd(0b00011000);
}

volatile double number;
volatile double num1;
volatile double num2;
char displayNumber[20];
char action; // + - * /
char key;
int result =0; //boolean 1 or 0

char readKeyPadRaw(void){
    ///////////////////////////////////////////////////////////////////////////
    LATBbits.LATB12 = 0; //first row is sent a high signal
    LATBbits.LATB13 = 1;
    LATBbits.LATB14 = 1;
    LATBbits.LATB15 = 1;
    
    delay(1); //Now we will wait in order to stop de-bounce
    
    if(PORTAbits.RA3 != 1)
        {
        return '1';
         }
    
    if(PORTAbits.RA2 != 1)
        {
        return '2';
        }
    
    if(PORTAbits.RA1 != 1)
        {
        return '3';
        }
    if(PORTAbits.RA0 != 1)
        {
        return 'A';
        }
    ///////////////////////////////////////////////////////////////////////////
    LATBbits.LATB12 = 1;
    LATBbits.LATB13 = 0; //second row is sent a high signal 
    LATBbits.LATB14 = 1;
    LATBbits.LATB15 = 1;
    
    delay(1); //Now we will wait in order to stop de-bounce
    
    if(PORTAbits.RA3 != 1){
        return '4';
    }
    
    if(PORTAbits.RA2 != 1){
        return '5';
    }
    
    if(PORTAbits.RA1 != 1){
        return '6';
    }
    if(PORTAbits.RA0 != 1){
        return 'B';
    }
    
    ////////////////////////////////////////////////////////////////////////////
    LATBbits.LATB12 = 1;
    LATBbits.LATB13 = 1;
    LATBbits.LATB14 = 0; //Third row is sent a high signal
    LATBbits.LATB15 = 1;
    
    delay(1); //Now we will wait in order to stop de-bounce
    
    if(PORTAbits.RA3 != 1){
        return '7';
    }
    
    if(PORTAbits.RA2 != 1){
        return '8';
    }
    
    if(PORTAbits.RA1 != 1){
        return '9';
    }
    if(PORTAbits.RA0 != 1){
        return 'C';
    }
    
    ////////////////////////////////////////////////////////////////////////////
    LATBbits.LATB12 = 1;
    LATBbits.LATB13 = 1;
    LATBbits.LATB14 = 1;
    LATBbits.LATB15 = 0; //forth row is sent a high signal
    
    delay(1); //Now we will wait in order to stop de-bounce
    
    if(PORTAbits.RA3 != 1)
        {
        return 'E';
        }
    
    if(PORTAbits.RA2 != 1)
        {
        return '0';
        }
    
    if(PORTAbits.RA1 != 1)
        {
        return 'F';
        }
    if(PORTAbits.RA0 != 1)
        {
        return 'D';
        }
    return 'x'; //This character is returned if no button is pressed 
} 

void covert_to_int()
{
    lcd_cmd(0b00000001);
    if(key == 'F'){
           lcd_cmd(0b00000001);
           number = num1; //this is the clear button and will clear the inputs
           num2 = 0;
           result = 0;    
    }
    if(key == '1'){ //this is the main part of the code that will turn the individual code into a int
       lcd_printChar(0b00110001);
       if(number==0){ //is this was the first button pressed, then set this as number
          number = 1;
       } 
       else{ //if this was not the first button pressed, 
       number = (number*10)+1; //we shift to the next tens place and add the input (input of 1 then another 1 will result in 11)
       } 
    }
    if(key == '2'){
       lcd_printChar(0b00110010);
       if(number==0){
          number = 2;
       } 
       else{
       number = (number*10)+2;
       } 
    }
    if(key == '3'){
       lcd_printChar(0b00110011);
       if(number==0){
          number = 3;
       } 
       else{
       number = (number*10)+3;
       } 
    }
    if(key == '4'){
       lcd_printChar(0b00110100);
       if(number==0){
          number = 4;
       } 
       else{
       number = (number*10)+4;
       } 
    }
    if(key == '5'){
       lcd_printChar(0b00110101);
       if(number==0){
          number = 5;
       } 
       else{
       number = (number*10)+5;
       } 
    }
    if(key == '6'){
       lcd_printChar(0b00110110);
       if(number==0){
          number = 6;
       } 
       else{
       number = (number*10)+6;
       } 
    }
    if(key == '7'){
       lcd_printChar(0b00110111);
       if(number==0){
          number = 7;
       } 
       else{
       number = (number*10)+7;
       } 
    }
    if(key == '8'){
       lcd_printChar(0b00111000);
       if(number==0){
          number = 8;
       } 
       else{
       number = (number*10)+8;
       } 
    }
    if(key == '9'){
       lcd_printChar(0b00111001);
       if(number==0){
          number = 9;
       } 
       else{
       number = (number*10)+9;
       } 
    }
    if(key == '0'){
       lcd_printChar(0b00110000);
       if(number==0){
          number = 0;
       } 
       else{
       number = (number*10)+0;
       } 
    }
    if(key == 'E'){ //equals sign was pressed
        lcd_printChar(0b00111101);
        num2=number; 
        result = 1;  //boolean is set
    }
    if(key == 'A'||key =='B'||key=='C'||key =='D'){ //one of the action keys was pressed, now figure out which one
        num1=number;
        number=0;
        if(key =='A'){
            lcd_printChar(0b00101011);
            action = '+';
        }
        if(key =='B'){
            lcd_printChar(0b00101101);
            action = '-';
        }
        if(key =='C'){
            lcd_printChar(0b00101010);
            action = '*';
        }
        if(key =='D'){
            lcd_printChar(0b00101111);
            action = '/';
        }
    delay(1);
    } 
}

void calculateResult(){ //math functions 
    if(action =='+'){
        number=num1+num2;
    }
    if(action =='-'){
        number=num1-num2;
    }
    if(action =='*'){
        number=num1*num2;
    }
    if(action =='/'){
        number=num1/num2;
    }
}

void displayAnswer(){
    lcd_cmd(0b00000001);
    lcd_setCursor(0,0);
    sprintf(displayNumber, "%f", number); //these are simular to lab 6 where we take a char array and turn them into a string
    lcd_printChar(0b00111101);
    if(result==1){ //if the equals sign was input, display answer 
        lcd_printStr(displayNumber); 
    }
}


int main(void) {
    setup();
    lcd_cmd(0b00000001);//clear display
    lcd_setCursor(0,0); 
    
    while(1){ //these steps will repeat forever
    key=readKeyPadRaw(); //find what key was pressed
    if(key != 'x') //if no key was detected (readkeypadraw returned an x) then we will not decode the values
    {
     covert_to_int(); //since we have found what key was pressed, we will now decode those values into integer values
     if(result == 1) //if the equal button was pressed
        {
         calculateResult(); //preform math functions
         displayAnswer(); //display answer on LCD (NOTE: i belive this part will take debugging, check if we need to left shift each string after displaying)
        }
    }
    }
    return 0;//never reached
}