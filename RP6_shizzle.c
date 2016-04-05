#define F_CPU 8000000

#include <avr/io.h>
#include "motor.h"

uint8_t ontvangen;
uint8_t snelheid;
motor_init();
///////////////////////////////////////////////////
//              shit ontvangen                   //
///////////////////////////////////////////////////

if((ontvangen & (1<<6)) != 0)//bumper check
{
    if((PINB & 0b00000001 | PINC & 0b01000000)
    {
        //stuur hoog signaal terug
    }
    else
    {
        //stuur laag signaal terug
    }
}
else
{
    if(ontvangen & 0b00000011 == 0)//snelheid
    {
        snelheid = 0;
    }
    else if(ontvangen & 0b00000011 == 1)
    {
        snelheid = 63;
    }
    else if(ontvangen & 0b00000011 == 2)
    {
        snelheid = 127;
    }
    else
    {
        snelheid = 191;
    }

    if((snelheid > 0) || ((ontvangen & (1<<3)) != 0))//niet stilstaan
    {
        pwmklok();
    }
    else//stilstaan
    {
        stop();
    }

    if(ontvangen & (1<<2) == 0)
    {
        vooruit();
    }
    else
    {
        achteruit();
    }

    if(ontvangen & (1<<3) == 0)
    {
        rechtdoor(snelheid);
    }
    else
    {
        if(ontvangen & (1<<4) == 0)
        {
            rechts(snelheid);
        }
        else
        {
            links(snelheid);
        }
    }
}



///////////////////////////////////////////////////

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
