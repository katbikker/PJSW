#define F_CPU 8000000

#include <avr/io.h>
#include <util/delay.h>

int main(void)
{
    DDRC |= (1<<2) | (1<<3);//zet DDR van de te gebruiken poorten.
    DDRD |= (1<<5) | (1<<4);//zet DDR van de te gebruiken poorten.
    PORTC |= (0<<2) | (0<<3);//zet voor de zekerheid de te gebruiken poorten op 0.
    PORTD |= (0<<5) | (0<<4);//zet voor de zekerheid de te gebruiken poorten op 0.

    void vooruit(uint8_t);//zet de rijrichting op vooruit, zet de pwm klok aan en stelt de compare registers gelijk aan de ingevoerde waarde.
    void achteruit(uint8_t);//zet de rijrichting op achteruit, zet de pwm klok aan en stelt de compare registers gelijk aan de ingevoerde waarde.
    void stop();//zet de pwm klok uit en zet de compare registers op 0.

    vooruit(128);
    _delay_ms(1000);
    achteruit(128);
    _delay_ms(1000);
    stop();

    return 0;
}

void vooruit(uint8_t snelheid)
{
    PORTC |= (0<<2) | (0<<3);//rijrichting vooruit
    TCCR1A = (1<<COM1A1) | (0<<COM1A0) | (1<<COM1B1) | (0<<COM1B0) | (0<<FOC1A) | (0<<FOC1B) | (0<<WGM11) | (1<<WGM10);//pwm klok aanzetten.
    TCCR1B = (0<<ICNC1) | (0<<ICES1) | (0<<5) | (0<<WGM13) | (0<<WGM12) | (0<<CS12) | (0<<CS11) | (1<<CS10);//pwm klok aanzetten.
    OCR1AH = 0;//we gebruiken maar 8 bits dus het hoge register is niet nodig. voor de zekerheid op 0.
    OCR1AL = snelheid;//stelt het compare register gelijk aan de ingevoerde waarde. dit bepaalt de snelheid. ("snelheid" / 255 * 100%)
    OCR1BH = 0;//we gebruiken maar 8 bits dus het hoge register is niet nodig. voor de zekerheid op 0.
    OCR1BL = snelheid;//stelt het compare register gelijk aan de ingevoerde waarde. dit bepaalt de snelheid. ("snelheid" / 255 * 100%)
}

void achteruit(uint8_t snelheid);
{
    PORTC |= (1<<2) | (1<<3);//rijrichting achteruit
    TCCR1A = (1<<COM1A1) | (0<<COM1A0) | (1<<COM1B1) | (0<<COM1B0) | (0<<FOC1A) | (0<<FOC1B) | (0<<WGM11) | (1<<WGM10);//pwm klok aanzetten.
    TCCR1B = (0<<ICNC1) | (0<<ICES1) | (0<<5) | (0<<WGM13) | (0<<WGM12) | (0<<CS12) | (0<<CS11) | (1<<CS10);//pwm klok aanzetten.
    OCR1AH = 0;//we gebruiken maar 8 bits dus het hoge register is niet nodig. voor de zekerheid op 0.
    OCR1AL = snelheid;//stelt het compare register gelijk aan de ingevoerde waarde. dit bepaalt de snelheid. ("snelheid" / 255 * 100%)
    OCR1BH = 0;//we gebruiken maar 8 bits dus het hoge register is niet nodig. voor de zekerheid op 0.
    OCR1BL = snelheid;//stelt het compare register gelijk aan de ingevoerde waarde. dit bepaalt de snelheid. ("snelheid" / 255 * 100%)
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
