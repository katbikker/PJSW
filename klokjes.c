#define F_CPU 16000000

#include <avr/io.h>

volatile unsigned long milliseconden;//globale variabele.

ISR(TIMER0_OVF_vect)//timer0 overflow.
{
    milliseconden++;
}

int main(void)
{
    //prototypes
    void initcurrenttime();
    unsigned long currenttime();
    void pingklok_aan();
    unsigned int return_microseconden_en_pingklok_uit();

    sei();//ergens in code moet je zetten.

    return 0;
}

void initcurrenttime()
{
    TCCR0A |= (1<<WGM1);//ctc modus.
    TCCR0B |= (1<<CS01) | (1<<CS00);//prescaler 64.
    OCR0A = 249;//16000000HZ dus 16000 pulsen per milliseconden. 16000 / 64 = 250. -1 want de overflow van 249 naar 0 is ook een puls.
    TIMSK0 = (1<< TOIE0);//enable overflow interrupt.
}

unsigned long currenttime()
{
    return milliseconden;
}

void pingklok_aan()
{
    //TCCR1A is 0.
    TCCR1B = (1<<CS10)//prescaler is 8.
    //TCCR1C is 0.
}

unsigned int return_microseconden_en_pingklok_uit()
{
    unsigned int i;
    cli();//16bit register uitlezen dus liever geen interrupt.
    i = TCNT1;//stel i gelijk aan aantal klokpulsen.
    sei();//zet interrupts weer aan.
    TCCR1B = 0;//zet klok uit.
    TCNT1 = 0;// zet klok weer op 0.
    i = i / 2;//prescaler is 8. cpu frequentie is 16mhz. 16*10^6 / 10^6 / 8 = 2 dus 2 pulsen per microseconde.
    return i;
}
