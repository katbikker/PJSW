#define F_CPU 8000000

#include <avr/io.h>
#include <util/delay.h>


int main(void)
{
    DDRC |= (1<<2) | (1<<3);
    DDRD |= (1<<5) | (1<<4);

    TCCR1A = (1<<COM1A1) | (0<<COM1A0) | (1<<COM1B1) | (0<<COM1B0) | (0<<FOC1A) | (0<<FOC1B) | (0<<WGM11) | (1<<WGM10);
    TCCR1B = (0<<ICNC1) | (0<<ICES1) | (0<<5) | (0<<WGM13) | (0<<WGM12) | (0<<CS12) | (0<<CS11) | (1<<CS10);
    OCR1AH = 0;
    OCR1AL = 64;

    OCR1BH = 0;
    OCR1BL = 64;

    PORTC |= (0<<2) | (0<<3);

    _delay_ms(1000);
    PORTD = (0<<5) | (0<<4);

    TCCR1A = 0;
    TCCR1B = 0;

    return 0;
}
