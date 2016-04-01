#include <avr/io.h>
#include <util/delay.h>

int pingPin = 5 //pin d5 is verbonden met sensor
int echoTime
long distance

int ping(distance)
{
    /*stuur ultrasoon signaal*/
    DDRD = 255; //d alles output
    PORTD |= (1 << pingPin); //ping high
    _delay_us(4); //delay 4 microseconden
    PORTD &= ~(1 << pingPin); //ping low
    
    /*wacht op echo*/
    DDRD = 0; //d alles input
    TCCR1A = 1; //start timer1 (16-bit)
    
    /*ultrasoon signaal wordt ontvangen */
    while ((PIND & (1 << pingPin)) == 1)
    {
        TCCR1A = 0; //stop timer1 (16-bit)
        
        distance = (echoTime * 0,003447) //reken afstand uit in cm
    }
}
        
        
        
        
        
