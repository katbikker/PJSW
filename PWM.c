#define F_CPU 8000000

#include <avr/io.h>
#include <util/delay.h>

int main(void)
{
    DDRB |= (0<<0); //zet DDR van de linker bumper.
    DDRC |= (1<<2) | (1<<3) | (0<<6);;//zet DDR van de rijrichting en de rechter bumper.
    DDRD |= (1<<5) | (1<<4);//zet DDR van de motor.
    PORTC |= (0<<2) | (0<<3);//zet voor de zekerheid de te gebruiken poorten op 0.
    PORTD |= (0<<5) | (0<<4);//zet voor de zekerheid de te gebruiken poorten op 0.

    void vooruit(uint8_t);//zet de rijrichting op vooruit. snelheid = ("ingevoerde waarde" / 255 * 100%)
    void achteruit(uint8_t);//zet de rijrichting op achteruit. snelheid = ("ingevoerde waarde" / 255 * 100%)
    void stop();//zet de pwm klok uit en zet de compare registers op 0.
    void stuur(char, char, uint8_t);//(V/A, L/R, snelheid) stelt de rijrichting, stuurrichting en snelheid in. snelheid = ("ingevoerde waarde" / 255 * 100%)
    void draai(char, uint8_t);//(L/R, snelheid) stelt de draairichting in. draaisnelheid = ("ingevoerde waarde" / 255 * 100%)
    
    char v = 'v';
    char a = 'a';
    char l = 'l';
    char r = 'r';

    
    while(!(PINB & 0b00000001 | PINC & 0b01000000)) // Kijk of de bumper iets raakt.
    {
    vooruit(128);
    _delay_ms(1000);
    achteruit(128);
    _delay_ms(1000);
    stuur(v, l, 128);
    _delay_ms(1000);
    draai(l, 128);
    _delay_ms(1000);
    stop();
    }
    achteruit(64);
    _delay_ms(500);
    stop();

    return 0;
}

void vooruit(uint8_t snelheid)
{
    PORTC |= (0<<2) | (0<<3);//rijrichting vooruit

    TCCR1A = (1<<COM1A1) | (0<<COM1A0) | (1<<COM1B1) | (0<<COM1B0) | (0<<FOC1A) | (0<<FOC1B) | (0<<WGM11) | (1<<WGM10);//pwm klok aanzetten.
    TCCR1B = (0<<ICNC1) | (0<<ICES1) | (0<<5) | (0<<WGM13) | (0<<WGM12) | (0<<CS12) | (0<<CS11) | (1<<CS10);//pwm klok aanzetten.

    OCR1AH = 0;//we gebruiken maar 8 bits dus het hoge register is niet nodig. voor de zekerheid op 0.
    OCR1AL = snelheid;//stelt het compare register gelijk aan de ingevoerde waarde. dit bepaalt de snelheid.
    OCR1BH = 0;//we gebruiken maar 8 bits dus het hoge register is niet nodig. voor de zekerheid op 0.
    OCR1BL = snelheid;//stelt het compare register gelijk aan de ingevoerde waarde. dit bepaalt de snelheid.
}

void achteruit(uint8_t snelheid);
{
    PORTC |= (1<<2) | (1<<3);//rijrichting achteruit

    TCCR1A = (1<<COM1A1) | (0<<COM1A0) | (1<<COM1B1) | (0<<COM1B0) | (0<<FOC1A) | (0<<FOC1B) | (0<<WGM11) | (1<<WGM10);//pwm klok aanzetten.
    TCCR1B = (0<<ICNC1) | (0<<ICES1) | (0<<5) | (0<<WGM13) | (0<<WGM12) | (0<<CS12) | (0<<CS11) | (1<<CS10);//pwm klok aanzetten.

    OCR1AH = 0;//we gebruiken maar 8 bits dus het hoge register is niet nodig. voor de zekerheid op 0.
    OCR1AL = snelheid;//stelt het compare register gelijk aan de ingevoerde waarde. dit bepaalt de snelheid.
    OCR1BH = 0;//we gebruiken maar 8 bits dus het hoge register is niet nodig. voor de zekerheid op 0.
    OCR1BL = snelheid;//stelt het compare register gelijk aan de ingevoerde waarde. dit bepaalt de snelheid.
}

void stop();
{
    PORTC |= (0<<2) | (0<<3);//rijrichting vooruit.
    TCCR1A = 0;//pwm klok uitzetten.
    TCCR1B = 0;//pwm klok uitzetten.
    OCR1AH = 0;//we gebruiken maar 8 bits dus het hoge register is niet nodig. voor de zekerheid op 0.
    OCR1AL = 0;//zet compare register op 0.
    OCR1BH = 0;//we gebruiken maar 8 bits dus het hoge register is niet nodig. voor de zekerheid op 0.
    OCR1BL = 0;//zet compare register op 0.
}

void stuur(char rijrichting, char stuurrichting, uint8_t snelheid)
{
    if(rijrichting == 'a' || rijrichting == 'A')
    {
        PORTC |= (1<<2) | (1<<3);//rijrichting achteruit
    }
    if(rijrichting == 'v' || rijrichting == 'V')
    {
        PORTC |= (0<<2) | (0<<3);//rijrichting vooruit
    }

    TCCR1A = (1<<COM1A1) | (0<<COM1A0) | (1<<COM1B1) | (0<<COM1B0) | (0<<FOC1A) | (0<<FOC1B) | (0<<WGM11) | (1<<WGM10);//pwm klok aanzetten.
    TCCR1B = (0<<ICNC1) | (0<<ICES1) | (0<<5) | (0<<WGM13) | (0<<WGM12) | (0<<CS12) | (0<<CS11) | (1<<CS10);//pwm klok aanzetten.

    if(stuurrichting == 'l' || stuurrichting == 'L')
    {
        OCR1AH = 0;//we gebruiken maar 8 bits dus het hoge register is niet nodig. voor de zekerheid op 0.
        OCR1AL = snelheid;//stelt het compare register gelijk aan de ingevoerde waarde. dit bepaalt de snelheid.
        OCR1BH = 0;//we gebruiken maar 8 bits dus het hoge register is niet nodig. voor de zekerheid op 0.
        OCR1BL = snelheid / 2;//hij draait langzamer dan de andere kant dus hij stuurt.
    }
    if(stuurrichting == 'r' || stuurrichting == 'R')
    {
        OCR1AH = 0;//we gebruiken maar 8 bits dus het hoge register is niet nodig. voor de zekerheid op 0.
        OCR1AL = snelheid / 2;//hij draait langzamer dan de andere kant dus hij stuurt.
        OCR1BH = 0;//we gebruiken maar 8 bits dus het hoge register is niet nodig. voor de zekerheid op 0.
        OCR1BL = snelheid;//stelt het compare register gelijk aan de ingevoerde waarde. dit bepaalt de snelheid.
    }
}

void draai(char richting, uint8_t snelheid)
{
    if(richting == 'l' || richting == 'L')//tegen de klok in.
    {
        PORTC |= (1<<2) | (0<<3);//links achteruit, rechts vooruit.
    }
    if(richting == 'r' || richting == 'R')//met de klok mee.
    {
        PORTC |= (1<<2) | (0<<3);//links vooruit, rechts achteruit.
    }

    TCCR1A = (1<<COM1A1) | (0<<COM1A0) | (1<<COM1B1) | (0<<COM1B0) | (0<<FOC1A) | (0<<FOC1B) | (0<<WGM11) | (1<<WGM10);//pwm klok aanzetten.
    TCCR1B = (0<<ICNC1) | (0<<ICES1) | (0<<5) | (0<<WGM13) | (0<<WGM12) | (0<<CS12) | (0<<CS11) | (1<<CS10);//pwm klok aanzetten.

    OCR1AH = 0;//we gebruiken maar 8 bits dus het hoge register is niet nodig. voor de zekerheid op 0.
    OCR1AL = snelheid;//stelt het compare register gelijk aan de ingevoerde waarde. dit bepaalt de snelheid.
    OCR1BH = 0;//we gebruiken maar 8 bits dus het hoge register is niet nodig. voor de zekerheid op 0.
    OCR1BL = snelheid;//stelt het compare register gelijk aan de ingevoerde waarde. dit bepaalt de snelheid.
}
