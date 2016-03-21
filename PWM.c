#include <avr/io.h>
#include <util/delay.h>

int main(void)
{
    DDRC |= (1<<6);
    DDRB |= (1<<0);
    DDRD |= (1<<5) | (1<<4);
    
    TCCR0 = (0<<FOC0) | (0<<WGM01) | (1<<WGM00) | (1<<COM01) | (0<<COM00) | (0<<CS02) | (0<<CS01) | (1<<CS00);
    OCR0 = 128;
    
    PORTC |= (1<<6);
    PORTB |= (1<<0);
    
    PORTD = (0<<5) | (0<<4);
    _delay_ms(1000);
    PORTD = (0<<5) | (0<<4);
    
    TCCR0 = 0;

    return 0;
}
