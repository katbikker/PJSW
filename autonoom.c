#define F_CPU 8000000

#include <avr/io.h>
#include <util/delay.h>

int main(void)
{
    void motor_init();//zet DDR van motor en bumper.
    void pwmklok();//zet pwm klok aan.
    void stop();//zet de pwm klok uit en zet de compare registers op 0.
    void vooruit();//zet de rijrichting op vooruit.
    void achteruit();//zet de rijrichting op achteruit.
    void rechtdoor(uint8_t);//snelheid = ("ingevoerde waarde" / 255 * 100%).
    void links(uint8_t);//stuurt naar links. snelheid = ("ingevoerde waarde" / 255 * 100%).
    void rechts(uint8_t);//stuurt naar rechts. snelheid = ("ingevoerde waarde" / 255 * 100%).
    
    void autonoom();
    
    autonoom();
    
    return 0;
}

//-------------autonoom---------------

void autonoom
{
    motor_init();
    pwmklok();
    while(!(PINB & 0b00000001 | PINC & 0b01000000)) // Kijk of de bumper iets raakt.
    {
        vooruit();
        rechtdoor(128);
        _delay_ms(500);
        achteruit();
        _delay_ms(500);
        vooruit();
        links(128);
        _delay_ms(500);
        achteruit();
        rechts(128);
        _delay_ms(500);
        links(0);
        _delay_ms(500);
        rechts(0);
        _delay_ms(500);
        
    }
    if(PINB & 0b00000001 | PINC & 0b01000000)
    {
        achteruit();
        rechtdoor(128);
        _delay_ms(500);
        rechts(0)
        _delay_ms(1500);
        stop();
    }
}

//--------------------functies-----------------

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
    PORTC &= ~(1<<2) & ~(1<<3);//rijrichting vooruit
}

void achteruit();
{
    PORTC |= (1<<2) | (1<<3);//rijrichting achteruit
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
    if(snelheid == 0)
    {
        PORTC |= (1<<2)//links achteruit, rechts vooruit.
        PORTC &= ~(1<<3);

        OCR1AH = 0;//we gebruiken maar 8 bits dus het hoge register is niet nodig. voor de zekerheid op 0.
        OCR1AL = snelheid;//stelt het compare register gelijk aan de ingevoerde waarde. dit bepaalt de snelheid.
        OCR1BH = 0;//we gebruiken maar 8 bits dus het hoge register is niet nodig. voor de zekerheid op 0.
        OCR1BL = snelheid;//stelt het compare register gelijk aan de ingevoerde waarde. dit bepaalt de snelheid.
    }
    else
    {
        OCR1AH = 0;//we gebruiken maar 8 bits dus het hoge register is niet nodig. voor de zekerheid op 0.
        OCR1AL = snelheid;//stelt het compare register gelijk aan de ingevoerde waarde. dit bepaalt de snelheid.
        OCR1BH = 0;//we gebruiken maar 8 bits dus het hoge register is niet nodig. voor de zekerheid op 0.
        OCR1BL = snelheid / 2;//hij draait langzamer dan de andere kant dus hij stuurt.
    }
}

void rechts(uint8_t snelheid)
{
    if(snelheid == 0)
    {
        PORTC |= (1<<3)//rechts achteruit, links vooruit.
        PORTC &= ~(1<<2);

        OCR1AH = 0;//we gebruiken maar 8 bits dus het hoge register is niet nodig. voor de zekerheid op 0.
        OCR1AL = snelheid;//stelt het compare register gelijk aan de ingevoerde waarde. dit bepaalt de snelheid.
        OCR1BH = 0;//we gebruiken maar 8 bits dus het hoge register is niet nodig. voor de zekerheid op 0.
        OCR1BL = snelheid;//stelt het compare register gelijk aan de ingevoerde waarde. dit bepaalt de snelheid.

    }
    else
    {
        OCR1AH = 0;//we gebruiken maar 8 bits dus het hoge register is niet nodig. voor de zekerheid op 0.
        OCR1AL = snelheid / 2;//hij draait langzamer dan de andere kant dus hij stuurt.
        OCR1BH = 0;//we gebruiken maar 8 bits dus het hoge register is niet nodig. voor de zekerheid op 0.
        OCR1BL = snelheid;//stelt het compare register gelijk aan de ingevoerde waarde. dit bepaalt de snelheid.
    }
}
