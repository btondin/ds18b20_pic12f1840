//**********************************************************************************
// Interfacing DS18B20 temperature sensor with PIC12F1840 MCU
//
// Device: PIC12F1840
// Compiler: Microchip XC8 v2.10
// IDE: MPLAB X v5.35
// Created: April 2020
//
//**********************************************************************************
//          Pinout for this example
//          ----------
//      Vdd |1      8| GND
//      RA5 |2      7| RA0
//      RA4 |3      6| RA1
//      RA3 |4      5| RA2 
//          ----------
//
//***********************************************************************************

#include <xc.h> // include standard header file

#include "ds18b20.h"

// set Config bits
#pragma config FOSC=INTOSC, PLLEN=OFF, WDTE=OFF, MCLRE=ON,
#pragma config CLKOUTEN=OFF, IESO=OFF, FCMEN=OFF,CP=OFF, CPD=OFF,BOREN=OFF
#pragma config WRT=OFF,STVREN=ON,BORV=LO,LVP=OFF

// Definitions
#define _XTAL_FREQ  16000000        // this is used by the __delay_ms(xx) and __delay_us(xx) functions

#define LED PORTAbits.RA2


//**********************************************************************************
//*****************   main routine   ***********************************************
//**********************************************************************************
void main ( ) 
{
    unsigned int DutyCycle;
    
    // set up oscillator control register
    OSCCONbits.SPLLEN=0;    // PLL is disabled
    OSCCONbits.IRCF=0x0F;   //set OSCCON IRCF bits to select OSC frequency=16Mhz
    OSCCONbits.SCS=0x02;    //set the SCS bits to select internal oscillator block
    // OSCON should be 0x7Ah now.

    // Set up I/O pins
    ANSELAbits.ANSELA=0;    // set all analog pins to digital I/O
    ADCON0bits.ADON=0;      // turn ADC off
    DACCON0bits.DACEN=0;    // turn DAC off

    // PORT A Assignments
    TRISAbits.TRISA0 = 0;	// RA0 = nc
    TRISAbits.TRISA1 = 0;	// RA1 = nc
    TRISAbits.TRISA2 = 0;	// RA2 = nc
    TRISAbits.TRISA3 = 0;	// RA3 = nc (MCLR)
    TRISAbits.TRISA4 = 0;	// RA4 = nc
    TRISAbits.TRISA5 = 0;	// RA5 = nc
    
        

    
    while(0)
    {
        __delay_ms(100);
        LED = 1;
        __delay_ms(100);
        LED = 0;
        
    }
    
    int temp;

	while ( 1 )
	{
		//Start conversion (without ROM matching)
		//ds18b20convert( &PORTA, &TRISA, &PORTA, ( 1 << 0 ), NULL );

		//Delay (sensor needs time to perform conversion)
		__delay_ms( 1000 );
        LED = !LED;

		//Read temperature (without ROM matching)
		//ds18b20read( &PORTB, &DDRB, &PINB, ( 1 << 0 ), NULL, &temp );

		//Somehow use data stored in `temp` variable
	}
    


}




