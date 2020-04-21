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

// set Config bits
#pragma config FOSC=INTOSC, PLLEN=OFF, WDTE=OFF, MCLRE=ON,
#pragma config CLKOUTEN=OFF, IESO=OFF, FCMEN=OFF,CP=OFF, CPD=OFF,BOREN=OFF
#pragma config WRT=OFF,STVREN=ON,BORV=LO,LVP=OFF

// Definitions
#define _XTAL_FREQ  16000000        // this is used by the __delay_ms(xx) and __delay_us(xx) functions


//**********************************************************************************
// This subroutine takes in a 10 bit number and sets the duty cycle register
// for the PWM accordingly
//**********************************************************************************
void SetPWMDutyCyle(unsigned int duty_cycle_value)
{
    CCP1CONbits.DC1B = duty_cycle_value & 0x03; //first set the 2 lsb bits
    CCPR1L =  (duty_cycle_value >> 2);           //now set upper 8 msb bits
}

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
    TRISAbits.TRISA2 = 0;	// RA2 = PWM Output (CCP1) connected to LED
    TRISAbits.TRISA3 = 0;	// RA3 = nc (MCLR)
    TRISAbits.TRISA4 = 0;	// RA4 = nc
    TRISAbits.TRISA5 = 0;	// RA5 = nc
    
    APFCONbits.CCP1SEL=0;       // The CCP1SEL bit selects which pin the PWM output is on.
                                // The default value for this bit is 0 which sets the
                                // PWM output on RA2.  If you want the PWM output on
                                // RA5 instead then set this bit to a 1.

    

    //******************************************************************************************
    // PWM Period = (1/Fosc) * 4 * (TMR2 Prescaler)* (PR2+1)
    //******************************************************************************************
    // Here are sample PWM periods for different TMR2 Prescalar values for Fosc=16Mhz and PR2=255
    //******************************************************************************************
    // TMR2 Prescalar=1: PWM Period = (1/16000000)*4*1*256 = 64 us or 15.63 khz
    // TMR2 Prescalar=4: PWM Period = (1/16000000)*4*4*256 = 256 us or 3.91 khz
    // TMR2 Prescalar=16: PWM Period = (1/16000000)*4*16*256= 1.024 ms or .976 khz
    // TMR2 Prescalar=64: PWM Period = (1/16000000)*4*64*256= 4.096 ms or .244 khz
    //
    // For this example we will choose the PWM period of 64us (15.63 kHz) so most people
    // will not be able to hear it.

    // ***** Setup PWM output ******************************************************

    TRISAbits.TRISA2 = 1;       // disable pwm pin output for the moment

    CCP1CONbits.CCP1M=0x0C;     // select PWM mode for CCP module
    CCP1CONbits.P1M=0x00;	// select single output on CCP1 pin (RA5)
    
    PR2 = 0xff;                 // set PWM period as 255 per our example above

    CCPR1L =  0x00;             // clear high 8 bits of PWM duty cycle
    CCP1CONbits.DC1B=0x00;	// clear low 2 bits of PWM Duty cycle

                                // Note: PWM uses TMR2 so we need to configure it
    PIR1bits.TMR2IF=0;		// clear TMR2 interrupt flag
    T2CONbits.T2CKPS=0x00;      // select TMR2 prescalar as divide by 1 as per our example above
    T2CONbits.TMR2ON=1;		// turn TMR2 on

    TRISAbits.TRISA2 = 0;	// turn PWM output back on

    //***********************************************************************************************************
    // Now lets look at adjusting the brightness of the LED by changing the PWM duty cycle.
    // The PWM duty cycle is a 10bit value.  The lower 8 bits are set using the CCPR1L register
    // and the 2 msb bits are set using the DC1B<1:0> bits in the CCP1CONregisters.
    //***********************************************************************************************************
    // duty cycle register value = (PWM Period)*(fraction of full pulse desired)/[(TMR2 Prescaler)*(1/Fosc)]
    //***********************************************************************************************************
    // Here are example duty duty cycles and corresponding duty cycle register values
    // based on Fosc=16Mhz, TMR2 Prescaler=divide by 1 and PWM Period set to 64us (15.63 khz)
    //*********************************************************************************
    // for 100% duty: Duty Cycle Value = (64us)(1.0)/[1*(1/16000000)] =  1024
    // for 90% duty: Duty Cycle Value  = (64us)(0.9)/[1*(1/16000000)] =  921.6
    // for 50% duty: Duty Cycle Value  = (64us)(0.5)/[1*(1/16000000)] =  512
    // for 10% duty: Duty Cycle Value  = (64us)(0.1)/[1*(1/16000000]) =  102.4
    // for 1% duty: Duty Cycle Value   = (64us)(0.01)/[1*(1/16000000]) = 10.24


    // in this example, we will ramp the PWM duty cycle from 0% duty (duty cycle register =0)
    // up to 100% duty (duty cycle register = 1024) in steps of 32 with a delay of 500ms
    // in between each step. After the ramp is up to 100%, the loop starts over again at 0%

    do{

       DutyCycle=0;
       while (DutyCycle <= 1023)
       {
            SetPWMDutyCyle(DutyCycle);
            DutyCycle = DutyCycle + 32;
            __delay_ms(10);
       }

    } while (1);


}




