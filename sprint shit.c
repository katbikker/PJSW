#define F_CPU 16000000

#include <avr/io.h>
#include <util/delay.h>

volatile unsigned long milliseconden;//globale variabele.

ISR(TIMER0_COMPA_vect)//timer0 compare.
{
    milliseconden++;
}

unsigned long currenttime();
void initcurrenttime();
unsigned int return_microseconden_en_pingklok_uit();
void pingklok_aan();
unsigned double ping();

int main(void)
{
    sei();
    initcurrenttime();
    unsigned long prevTime;
    unsigned double afstand;



    while(1)
    {
        prevTime = currenttime();
        while((prevTime + 1000) >= currenttime());
        afstand = ping();
        //shit versturen
    }




    return 0;
}

unsigned double ping()
{
    unsigned int echoTime;
    unsigned double distance;
    /*stuur ultrasoon signaal*/
    DDRD = 255; //d alles output
    PORTD |= (1 << 5); //ping high. pin d5 is verbonden met sensor
    _delay_us(4); //delay 4 microseconden
    PORTD &= ~(1 << 5); //ping low. pin d5 is verbonden met sensor

    /*wacht op echo*/
    DDRD = 0; //d alles input
    pingklok_aan(); //start timer1 (16-bit)

    /*ultrasoon signaal wordt ontvangen */
    while ((PIND & (1 << 5)) == 0);
    echoTime = return_microseconden_en_pingklok_uit();
    distance = echoTime * 0,03435; //reken afstand uit in cm
    return distance;
}

void pingklok_aan()
{
    //TCCR1A is 0.
    TCCR1B = (1<<CS11);//prescaler is 8.
    //TCCR1C is 0.
}

unsigned int return_microseconden_en_pingklok_uit()
{
    unsigned int i;

    cli();//16bit register uitlezen dus liever geen interrupt.
    i = TCNT1;//stel i gelijk aan aantal klokpulsen.
    sei();//zet interrupts weer aan.

    TCCR1B = 0;//zet klok uit.

    cli();//16bit register schrijven dus liever geen interrupt.
    TCNT1 = 0;// zet klok weer op 0.
    sei();//zet interrupts weer aan.

    i = i / 2;//prescaler is 8. cpu frequentie is 16mhz. 16*10^6 / 10^6 / 8 = 2 dus 2 pulsen per microseconde.
    return i;
}

void initcurrenttime()
{
    TCCR0A |= (1<<WGM01);//ctc modus.
    TCCR0B |= (1<<CS01) | (1<<CS00);//prescaler 64.
    OCR0A = 249;//16000000HZ dus 16000 pulsen per milliseconden. 16000 / 64 = 250. -1 want de stap van 249 naar 0 is ook een puls.
    TIMSK0 = (1<<OCIE0A);//enable compare interrupt.
}

unsigned long currenttime()
{
    return milliseconden;
}
