/*
 * i2cw.c
 *
 * Created: 3/19/2016 2:14:44 PM
 * Author : john
 */

#define F_CPU 8000000

#include "motor.h"

#include <avr/io.h>
#include <util/twi.h>
#include "i2c.h"
#include "rp6aansluitingen.h"

#define TRUE 0xFF;
#define FALSE 0;

/*de interrupt routine van de i2c
de functie slaaftwi() staat in de library
*/

ISR(TWI_vect)
{

    slaaftwi();

}
uint8_t bumper_sturen();
void opdracht(uint8_t);
uint8_t ontvangen;
uint8_t snelheid;

uint8_t data_ont[20]; //max 20
volatile uint8_t data_flag = FALSE;
volatile uint8_t databyte=0x33;

void ontvangData(uint8_t [],uint8_t);
uint8_t verzendByte();

int main(void)
{

    DDRC=0xFF;
    initUSART();
    init_i2c_slave(8);

    /*ontvangData is de functie die uitgevoerd wordt
    wanneer een byte via de i2c bus ontvangen wordt
    */
    init_i2c_ontvang(ontvangData);

    /*verzendByte is de functie die aangeroepen wordt
    wanneer de slave een byte naar de master verzend*/
    init_i2c_verzend(verzendByte);

    sei(); //De slave van i2c werkt met interrupt

    motor_init();

    /* Replace with your application code */
    while (1)
    {
        if(data_flag)
        {

            opdracht(data_ont[0]);
            data_flag = FALSE;
        }
    }

}
/*slave heeft data ontvangen van de master
data[] een array waarin de ontvangen data staat
tel het aantal bytes dat ontvangen is*/

void ontvangData(uint8_t data[],uint8_t tel)
{
    for(int i=0; i<tel; ++i)
        data_ont[i]=data[i];
    data_flag = TRUE;
    writeString("o\n\r");
}

/* het byte dat de slave verzend naar de master
in dit voorbeeld een eenvoudige teller
*/

uint8_t verzendByte()
{
    return bumper_sturen();
}

//////////////////////////////////////////////////////////////////////////
uint8_t bumper_sturen()
{
    uint8_t i;

    if((PINB & 0b00000001 | PINC & 0b01000000)
    {
        i = 255;
    }
    else
    {
        i = 0;
    }
    return i;
}
///////////////////////////////////////////////////////////////////////////////////////
void opdracht(uint8_t ontvangen[])
{
    if(ontvangen[0] & 0b00000011 == 0)//snelheid
    {
        snelheid = 0;
    }
    else if(ontvangen[0] & 0b00000011 == 1)
    {
        snelheid = 63;
    }
    else if(ontvangen[0] & 0b00000011 == 2)
    {
        snelheid = 127;
    }
    else
    {
        snelheid = 191;
    }

    if((snelheid > 0) || ((ontvangen[0] & (1<<3)) != 0))//niet stilstaan
    {
        pwmklok();
    }
    else//stilstaan
    {
        stop();
    }

    if(ontvangen[0] & (1<<2) == 0)
    {
        vooruit();
    }
    else
    {
        achteruit();
    }

    if(ontvangen[0] & (1<<3) == 0)
    {
        rechtdoor(snelheid);
    }
    else
    {
        if(ontvangen[0] & (1<<4) == 0)
        {
            rechts(snelheid);
        }
        else
        {
            links(snelheid);
        }
    }
}
///////////////////////////////////////////////////////////////////////////////////////

void motor_init()//zet DDR van motor en bumper.
{
    DDRB &= ~(1<<0);//zet DDR van de linker bumper.
    DDRC &= ~(1<<6);//zet DDR van de rechter bumper.
    DDRC |= (1<<2) | (1<<3) | ;//zet DDR van de rijrichting.
    DDRD |= (1<<5) | (1<<4);//zet DDR van de motor.
}

void pwmklok()
{
    TCCR1A = (1<<COM1A1) | (0<<COM1A0) | (1<<COM1B1) | (0<<COM1B0) | (0<<FOC1A) | (0<<FOC1B) | (0<<WGM11) | (1<<WGM10);//pwm klok aanzetten.
    TCCR1B = (0<<ICNC1) | (0<<ICES1) | (0<<5) | (0<<WGM13) | (0<<WGM12) | (0<<CS12) | (0<<CS11) | (1<<CS10);//pwm klok aanzetten.
}

void stop();
{
    TCCR1A = 0;//pwm klok uitzetten.
    TCCR1B = 0;//pwm klok uitzetten.
    OCR1AH = 0;//we gebruiken maar 8 bits dus het hoge register is niet nodig. voor de zekerheid op 0.
    OCR1AL = 0;//zet compare register op 0.
    OCR1BH = 0;//we gebruiken maar 8 bits dus het hoge register is niet nodig. voor de zekerheid op 0.
    OCR1BL = 0;//zet compare register op 0.
}

void vooruit()
{
    PORTC &= ~(1<<2) & ~(1<<3);//rijrichting vooruit.
    //buzzer uit
}

void achteruit()
{
    PORTC |= (1<<2) | (1<<3);//rijrichting achteruit.
    //buzzer aan
}

void rechtdoor(uint8_t snelheid)
{
    OCR1AH = 0;//we gebruiken maar 8 bits dus het hoge register is niet nodig. voor de zekerheid op 0.
    OCR1AL = snelheid;//stelt het compare register gelijk aan de ingevoerde waarde. dit bepaalt de snelheid.
    OCR1BH = 0;//we gebruiken maar 8 bits dus het hoge register is niet nodig. voor de zekerheid op 0.
    OCR1BL = snelheid;//stelt het compare register gelijk aan de ingevoerde waarde. dit bepaalt de snelheid.
}

void links(uint8_t snelheid)
{
    if(snelheid == 0)//draaien.
    {
        PORTC |= (1<<2)//links achteruit, rechts vooruit.
                 PORTC &= ~(1<<3);

        OCR1AH = 0;//we gebruiken maar 8 bits dus het hoge register is niet nodig. voor de zekerheid op 0.
        OCR1AL = 128;//stelt het compare register gelijk aan de ingevoerde waarde.
        OCR1BH = 0;//we gebruiken maar 8 bits dus het hoge register is niet nodig. voor de zekerheid op 0.
        OCR1BL = 128;//stelt het compare register gelijk aan de ingevoerde waarde.
    }
    else//sturen.
    {
        OCR1AH = 0;//we gebruiken maar 8 bits dus het hoge register is niet nodig. voor de zekerheid op 0.
        OCR1AL = snelheid;//stelt het compare register gelijk aan de ingevoerde waarde. dit bepaalt de snelheid.
        OCR1BH = 0;//we gebruiken maar 8 bits dus het hoge register is niet nodig. voor de zekerheid op 0.
        OCR1BL = snelheid / 2;//hij draait langzamer dan de andere kant dus hij stuurt.
    }
}

void rechts(uint8_t snelheid)
{
    if(snelheid == 0)//draaien.
    {
        PORTC |= (1<<3)//rechts achteruit, links vooruit.
                 PORTC &= ~(1<<2);

        OCR1AH = 0;//we gebruiken maar 8 bits dus het hoge register is niet nodig. voor de zekerheid op 0.
        OCR1AL = 128;//stelt het compare register gelijk aan de ingevoerde waarde.
        OCR1BH = 0;//we gebruiken maar 8 bits dus het hoge register is niet nodig. voor de zekerheid op 0.
        OCR1BL = 128;//stelt het compare register gelijk aan de ingevoerde waarde.

    }
    else//sturen.
    {
        OCR1AH = 0;//we gebruiken maar 8 bits dus het hoge register is niet nodig. voor de zekerheid op 0.
        OCR1AL = snelheid / 2;//hij draait langzamer dan de andere kant dus hij stuurt.
        OCR1BH = 0;//we gebruiken maar 8 bits dus het hoge register is niet nodig. voor de zekerheid op 0.
        OCR1BL = snelheid;//stelt het compare register gelijk aan de ingevoerde waarde. dit bepaalt de snelheid.
    }
}
