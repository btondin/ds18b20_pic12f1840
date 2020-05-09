#ifndef MEUDS18B20_H_   /* Include guard */
#define MEUDS18B20_H_

// OneWire commands
#define STARTCONVO      0x44  // Tells device to take a temperature reading and put it on the scratchpad
#define COPYSCRATCH     0x48  // Copy EEPROM
#define READSCRATCH     0xBE  // Read EEPROM
#define WRITESCRATCH    0x4E  // Write to EEPROM
#define RECALLSCRATCH   0xB8  // Reload from last known
#define READPOWERSUPPLY 0xB4  // Determine if device needs parasite power
#define ALARMSEARCH     0xEC  // Query bus for devices with an alarm condition

// Scratchpad locations
#define TEMP_LSB        0
#define TEMP_MSB        1
#define HIGH_ALARM_TEMP 2
#define LOW_ALARM_TEMP  3
#define CONFIGURATION   4
#define INTERNAL_BYTE   5
#define COUNT_REMAIN    6
#define COUNT_PER_C     7
#define SCRATCHPAD_CRC  8

// Device resolution
#define TEMP_9_BIT  0x1F //  9 bit
#define TEMP_10_BIT 0x3F // 10 bit
#define TEMP_11_BIT 0x5F // 11 bit
#define TEMP_12_BIT 0x7F // 12 bit


float ds1820_read()
{
 int8 busy=0, temp1, temp2;
 signed int16 temp3;
 float result;

 onewire_reset();
 onewire_write(0xCC);
 onewire_write(0x44);

 while (busy == 0)
  busy = onewire_read();

 onewire_reset();
 onewire_write(0xCC);
 onewire_write(0xBE);
 temp1 = onewire_read();
 temp2 = onewire_read();
 temp3 = make16(temp2, temp1);
 
 result = (float) temp3 / 2.0;   //Calculation for DS18S20 with 0.5 deg C resolution
// result = (float) temp3 / 16.0;  //Calculation for DS18B20 with 0.1 deg C resolution
 
 delay_ms(200);
 return(result);
}