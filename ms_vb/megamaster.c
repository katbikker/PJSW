#define F_CPU 16000000

#include <avr/io.h>
#include <util/delay.h>
#include "i2c_mst.h"

#define DEVICE_ADRES   8

#define KOMPAS 0x60 //1100000 = 0x60, 1100000 + R/W bit = 11000000 (0xC0 = 192, bit0 = 0 write) or 11000001 (0xC1 = 193, bit0 = 1 read)

#define KOMPAS_REG2 0x02 //register 2 is 16 bits (register 2,3)
#define KOMPAS_COMREG 0x0F //command register (8 bits) = 15

void calKompas(void);
void kompas(void);

int main(void)
{
	PORTD = 0x03; //pullup SDA en SCL
	initUSART();
    uint8_t data[10];
    init_master();

	writeString("Een testje\n\r");
    uint8_t teller=1;
    
    calKompas(void);
}

//KOMPAS (zie http://www.robot-electronics.co.uk/htm/cmps3tech.htm)

void kompas(void)
{
    verzenden(KOMPAS, KOMPAS_REG2);
    for(uint8_t i = 0; i < 8; i++) _delay_ms(250);
    
    ontvangen(KOMPAS, data, 2); //master ontvangt 2 bytes van slave (register 2,3)
    
    uint8_t highByte = data[0]; //register 2 (0-255)
    uint8_t lowByte = data[1];  //register 3
    double bearing = ((highByte << 8) + lowByte) / 10; //0-3599 / 10 = 0-359.9
    
    writeString("\n\rBearing: "); writeInteger(bearing,10); //print bearing
}

void calKompas(void)
{
    verzenden(KOMPAS, KOMPAS_COMREG);
    for(uint8_t i = 0; i < 8; i++) _delay_ms(250);
    
    for(uint8_t i = 0; i < 3; i++)
    {
    verzenden(KOMPAS, 0xFF);
    _delay_ms(10000)
    }
}

/*
1. Set the compass module flat, pointing North. Write 255 to register 15, Calibrating pin (pin5) goes low.
2. Set the compass module flat, pointing East. Write 255 to register 15,
3. Set the compass module flat, pointing South. Write 255 to register 15,
4. Set the compass module flat, pointing West. Write 255 to register 15, Calibrating pin (pin5) goes high.
*/


