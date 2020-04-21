void delay_us(int useconds);
void delay_ms(int j);
unsigned char ow_reset(void);
unsigned char read_bit(void);
unsigned char read_byte(void);
void write_bit(char bitval);
void write_byte(char val);
unsigned char * ReadTemp();

//Sourav || Date:- 16.02.2016, 6.44PM Rev:-1 Source:- https://datasheets.maximintegrated.com/en/ds/DS18B20.pdf
//18b20.c required.