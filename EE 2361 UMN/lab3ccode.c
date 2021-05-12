/*
 * File:   lab3code.c
 * Author: Joseph Gehlhoff
 *
 * Created on February 19, 2021, 2:34 PM
 */




#include "xc.h"
#include "delayheader.h"



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

void setup(void)
{
    CLKDIVbits.RCDIV = 0;  
}

 
void init7seg(void) {
    AD1PCFG = 0x9FFF;
    TRISB = 0x0000;
    LATB = 0xFFFF;
    LATA = 0xFFFF;
}
void showChar7seg(char myChar, int myDigit){
   LATB = 0xFFFF ;
    if(myDigit == 1){
        LATBbits.LATB10 =1;
        LATBbits.LATB11 = 0;
        
    }
    else if(myDigit == 0){
        LATBbits.LATB10 =0;
        LATBbits.LATB11 = 1;
        
    }
    
    if(myChar == '1'){
        LATBbits.LATB8 = 0; 
        LATBbits.LATB7 = 0; 
    }
    if (myChar == '2'){
        LATBbits.LATB9 = 0;
         LATBbits.LATB8 = 0;
          LATBbits.LATB3 = 0;
          LATBbits.LATB5 = 0;
         LATBbits.LATB6 = 0;
    }
    
    if(myChar == '3'){
        LATBbits.LATB9 = 0; 
     LATBbits.LATB8 = 0; 
     LATBbits.LATB7 = 0; 
     LATBbits.LATB6 = 0;
     LATBbits.LATB3 = 0;
    }
    if(myChar == '4'){
        LATBbits.LATB8 = 0; 
        LATBbits.LATB7 = 0; 
        LATBbits.LATB4 = 0;
        LATBbits.LATB3 = 0;
    }
    
    if(myChar =='5'){
        LATBbits.LATB9 = 0;
        LATBbits.LATB4 = 0;
        LATBbits.LATB3 = 0;
        LATBbits.LATB7 = 0; 
        LATBbits.LATB6 = 0;
    }
    
    if(myChar =='6'){
     LATBbits.LATB9 = 0; 
     LATBbits.LATB7 = 0; 
     LATBbits.LATB6 = 0;
     LATBbits.LATB5 = 0;
     LATBbits.LATB4 = 0;
     LATBbits.LATB3 = 0;
       
    }
    
    if(myChar =='7'){
     LATBbits.LATB9 = 0; 
     LATBbits.LATB8 = 0; 
     LATBbits.LATB7 = 0; 
     
        
    }
    
    if(myChar=='8'){
     LATBbits.LATB9 = 0; 
     LATBbits.LATB8 = 0; 
     LATBbits.LATB7 = 0; 
     LATBbits.LATB6 = 0;
     LATBbits.LATB5 = 0;
     LATBbits.LATB4 = 0;
     LATBbits.LATB3 = 0;
    }
    
    if(myChar=='9'){
     LATBbits.LATB9 = 0; 
     LATBbits.LATB8 = 0; 
     LATBbits.LATB7 = 0; 
     LATBbits.LATB6 = 0;
     LATBbits.LATB4 = 0;
     LATBbits.LATB3 = 0;
    }
    
    if(myChar=='0'){
     LATBbits.LATB9 = 0; 
     LATBbits.LATB8 = 0; 
     LATBbits.LATB7 = 0; 
     LATBbits.LATB6 = 0;
     LATBbits.LATB5 = 0;
     LATBbits.LATB4 = 0;
    
    }
    
    if(myChar=='A'){
       
     LATBbits.LATB9 = 0;
     LATBbits.LATB8 = 0; 
     LATBbits.LATB7 = 0; 
    LATBbits.LATB3 = 0;
     LATBbits.LATB5 = 0;
     LATBbits.LATB4 = 0;
     
    
    }
    
    if(myChar=='b'){
     LATBbits.LATB7 = 0; 
     LATBbits.LATB6 = 0;
     LATBbits.LATB5 = 0;
     LATBbits.LATB4 = 0;
     LATBbits.LATB3 = 0;
    }
    
     if(myChar=='C'){
     LATBbits.LATB9 = 0; 
     LATBbits.LATB6 = 0;
     LATBbits.LATB5 = 0;
     LATBbits.LATB4 = 0;
 
    }
    
    if(myChar=='d'){
    
     LATBbits.LATB8 = 0; 
     LATBbits.LATB7 = 0; 
     LATBbits.LATB6 = 0;
     LATBbits.LATB5 = 0;
     LATBbits.LATB3 = 0;
    }
    
     if(myChar=='E'){
     LATBbits.LATB9 = 0; 
     LATBbits.LATB6 = 0;
     LATBbits.LATB5 = 0;
     LATBbits.LATB4 = 0;
     LATBbits.LATB3 = 0;
    }
     if(myChar=='F'){
     LATBbits.LATB9 = 0; 
     LATBbits.LATB5 = 0;
     LATBbits.LATB4 = 0;
     LATBbits.LATB3 = 0;
    }
}
void initkeypad(void){
    AD1PCFG = 0x9FFF;
    LATA = 0xFFFF;
    TRISA = 0xFFFF;
    CNPU1bits.CN2PUE =1;
    CNPU1bits.CN3PUE = 1;
    CNPU2bits.CN30PUE =1;
    CNPU2bits.CN29PUE  =1;
}
char readKeyPadRaw(void){
    ///////////////////////////////////////////////////////////////////////////
    LATBbits.LATB12 = 0; //first row is sent a high signal
    LATBbits.LATB13 = 1;
    LATBbits.LATB14 = 1;
    LATBbits.LATB15 = 1;
    
    ms_wait(); //Now we will wait in order to stop de-bounce
    
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
    
    ms_wait(); //Now we will wait in order to stop de-bounce
    
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
        return 'b';
    }
    
    ////////////////////////////////////////////////////////////////////////////
    LATBbits.LATB12 = 1;
    LATBbits.LATB13 = 1;
    LATBbits.LATB14 = 0; //Third row is sent a high signal
    LATBbits.LATB15 = 1;
    
    ms_wait(); //Now we will wait in order to stop de-bounce
    
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
    
    ms_wait(); //Now we will wait in order to stop de-bounce
    
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
        return 'd';
        }
    return 'x'; //This character is returned if no button is pressed 
} 

void delay(int delay_in_ms){            
    int d;
    for(d=0;d<delay_in_ms;d++){
        ms_wait();                  // call the millisecond delay function delay_in_ms times
    }
}

int main(void) {
    setup();
    init7seg(); 
    initkeypad();
    
    char Value[2] = {'x','x'};
    char Value2[2] = {'x','x'}; 
    //both value and nextDig are set to a void char
    
    while(1) 
    {
        
        char newVal = readKeyPadRaw(); 
        Value2[0] = Value2[1]; 
        Value2[1] = newVal; 
        
        
        if (Value2[0] == 'x' && Value2[1] != 'x') 
            {                                            //handles very first inputs 
                Value[0] = Value[1];
                Value[1] = newVal;
            }
         
        showChar7seg(Value[1],1); 
        delay(5);
        showChar7seg(Value[0],0); 
        delay(5); 
    }
    return 0;
}
