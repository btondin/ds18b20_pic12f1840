///////////////////////////////////////////////////////////////////////////
////                                                                   ////
////                            ds18b20.c                              ////
////                                                                   ////
//// Driver for Dallas DS18B20 digital thermometer, with Dallas one    ////
//// wire interface.                                                   ////
////                                                                   ////
////  ds18b20_init()                                                   ////
////     Init the module and the driver.                               ////
////                                                                   ////
////  __bit = ds18b20_read(signed short *pTemp)                        ////
////     Reads temperature from device and saves read value to pTemp   ////
////     pointer.  Returns TRUE if we succesfully talked to the device ////
////     and pTemp was updated; returns FALSE if there was an error    ////
////     in communication and pTemp is not updated.                    ////
////     This will deadlock the PICmicro while waiting for the         ////
////     DS18B20 to return a conversion, which can take over 750ms.    ////
////     Also this routine may get stuck in an infinite loop and not   ////
////     return if there is a problem with the bus.                    ////
////     To limit the deadlocking, use ds18b20_start_conversion() and  ////
////     ds18b20_get_conversion() instead.                             ////
////     The temperature returned is degrees C, but fractional;        ////
////     divide by 16 to get the integer numerator.                    ////
////                                                                   ////
//// __bit ds18b20_start_conversion(void)                              ////
////     This starts a temperature conversion on the DS18B20.          ////
////     Returns TRUE if communication was successful, FALSE if there  ////
////     was something wrong with the bus or the device.               ////
////                                                                   ////
//// __bit ds18b20_get_conversion(signed short *pTemp)                 ////
////     After a conversion is started, you can either wait for Tconv  ////
////     (which is 750ms for max resolution conversion) or poll        ////
////     the bus with ds18b20_read_bit() (if it returns non-zero       ////
////     then conversion is finished).  Once conversion is finished    ////
////     then this function reads the conversion stored on the         ////
////     DS18B20 device.                                               ////
////                                                                   ////
////                            CONNECTIONS                            ////
////                                                                   ////
//// PIN_DS18B20_DATA                                                  ////
////     #define this to the PIC GPIO pin connected to the DS18B20's   ////
////     data pin.                                                     ////
////     Example:  #define PIN_DS18B20_DATA PIN_D0                     ////
////                                                                   ////
///////////////////////////////////////////////////////////////////////////
////        (C) Copyright 1996,2016 Custom Computer Services           ////
//// This source code may only be used by licensed users of the CCS C  ////
//// compiler.  This source code may only be distributed to other      ////
//// licensed users of the CCS C compiler.  No other use, reproduction ////
//// or distribution is permitted without written permission.          ////
//// Derivative programs created using this software in object code    ////
//// form are not restricted in any way.                               ////
////                                                                   ////
//// http://www.ccsinfo.com                                            ////
///////////////////////////////////////////////////////////////////////////

/// begin config //////////////////////////////////////////////////////////

#include <stdint.h>
#include "CCS_to_XC8.c"


#ifndef PIN_DS18B20_DATA
#define PIN_DS18B20_DATA PORTAbits.RA4
#endif

#ifndef PIN_DS18B20_TRIS
#define PIN_DS18B20_TRIS TRISAbits.RA4
#endif

/// end config ////////////////////////////////////////////////////////////


void ds18b20_init(void)
{
   //output_float(PIN_DS18B20_DATA);
   PIN_DS18B20_TRIS = 1; //Set as input
}

/*
This method will initialize the DS18B20 for transactions to occur
PARAMS: none
RETURNS: TRUE if start pulse response read from unit
*/
__bit ds18b20_start(void)
{
   __bit ret = FALSE;
   
   //output_low(PIN_DS18B20_DATA);
   PIN_DS18B20_TRIS = 0; //Set as output
   PIN_DS18B20_DATA = 0;
   
   __delay_us(500);
   //output_float(PIN_DS18B20_DATA);
   PIN_DS18B20_TRIS = 1; //Set as input
   
   
   __delay_us(70);//wait to read the SLAVES response
   //if (!input(PIN_DS18B20_DATA))
   if (!PIN_DS18B20_DATA)
       
       
   {
      ret = TRUE;
      __delay_us(430);
   }
   return(ret);
}
/*
This method will write a bit to the DS18B20 following write time slots
PARAMS: __bit value to write
RETURNS: none
*/
void ds18b20_write(__bit value)
{
   //output_low(PIN_DS18B20_DATA);//drives output low for master
   PIN_DS18B20_TRIS = 0; //Set as output
   PIN_DS18B20_DATA = 0;
   
   
   __delay_us(2);//delays for 2us
   //output_bit(PIN_DS18B20_DATA,value);//sets MASTER to input mode to release pin
   PIN_DS18B20_TRIS = 0; //Set as output
   PIN_DS18B20_DATA = value;
   
   
   __delay_us(60);
   //output_float(PIN_DS18B20_DATA);//sets MASTER to input mode to release pin
   PIN_DS18B20_TRIS = 1; //Set as input
   
   __delay_us(2);//delays for 2 microseconds which is within 15us max
}
/*
This method will call write() for each bit of a byte to be sent
PARAMS: BYTE value to write
RETURNS: none
*/
void ds18b20_write_byte(unsigned char value)
{
   unsigned char i;
   for(i=1;i<=8;++i)
   {
      ds18b20_write(shift_right(&value,1,0));
   }
}
/*
This method will read a value from the DS18B20 following the read time slots
PARAMS: none
RETURNS: A bit from the DS1993
*/
__bit ds18b20_read_bit(void)
{
   __bit value;
   //output_low(PIN_DS18B20_DATA);//drives output low for master
   PIN_DS18B20_TRIS = 0; //Set as output
   PIN_DS18B20_DATA = 0;
   
   
   __delay_us(2);
   //output_float(PIN_DS18B20_DATA);//sets MASTER to input mode to release pin
   PIN_DS18B20_TRIS = 1; //Set as input
   
   
   __delay_us(8);//waits to read
   //value=input(PIN_DS18B20_DATA);
   value = PIN_DS18B20_DATA;
   
   __delay_us(120);
   return value;
}
/*
This method will call read() and update a byte bit by bit
PARAMS: none
RETURNS: A byte from the DS1993
*/
unsigned char ds18b20_read_byte()
{
   unsigned char i, value;
   
   for(i=0; i<8; i++)
   {
      shift_right(&value,1,ds18b20_read_bit());
   }
   return value;
}

__bit ds18b20_start_conversion(void)
{
   if (!ds18b20_start())
      return(FALSE);
   ds18b20_write_byte(0xCC);
   ds18b20_write_byte(0x44);
   return(TRUE);
}

__bit ds18b20_get_conversion(signed short *pTemp)
{
   union 
   {
      unsigned char b[2];
      signed short w;
   } ret;
   
   if (!ds18b20_start())
      return(FALSE);
      
   ds18b20_write_byte(0xCC);
   ds18b20_write_byte(0xBE);
   ret.b[0] = ds18b20_read_byte();
   ret.b[1] = ds18b20_read_byte();
   
   *pTemp = ret.w;
   
   return(TRUE);
}

__bit ds18b20_read(signed short *pTemp)
{
   if (!ds18b20_start_conversion())
      return(FALSE);
      
   while(!ds18b20_read_bit());
      
   return(ds18b20_get_conversion(pTemp));
}

/*
This method will initiate a temperature read (12 bit resolution) and convert 
of the module by skipping the ROM of the module and reading from 
the scratch pad
PARAMS: pTemp - where temperature is stored if valid read
RETURNS: TRUE if ds18b20 is on bus and acting properly
*/
/*
__bit ds18b20_read(signed short *pTemp)
{
   unsigned char busy = 0;
   union 
   {
      unsigned char b[2];
      signed short w;
   } ret;
   
   if (!ds18b20_start())
      return(FALSE);
   ds18b20_write_byte(0xCC);
   ds18b20_write_byte(0x44);
   while(busy==0)
      busy=ds18b20_read_byte();
      
   if (!ds18b20_start())
      return(FALSE);
   ds18b20_write_byte(0xCC);
   ds18b20_write_byte(0xBE);
   ret.b[0] = ds18b20_read_byte();
   ret.b[1] = ds18b20_read_byte();
   
   return(ret.w);
}
*/

