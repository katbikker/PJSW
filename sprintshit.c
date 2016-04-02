#define F_CPU 16000000

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

volatile unsigned long milliseconden = 0.0; //global variable

double ping();
double currenttime();
double return_microseconden_en_pingklok_uit();

void USART_Init(); //enable usart
void pingklok_aan(); //enable timer
void initcurrenttime();
void trns(unsigned char c);

/* isr */
ISR(TIMER0_COMPA_vect) //timer0 compare.
{
    milliseconden++;
}

/* main */
int main(void)
{
    initcurrenttime();
  	USART_Init();
    double prevTime, distanceInCm;

    while(1)
    {
        prevTime = currenttime();

        while((prevTime + 1000.0) >= currenttime())
        {
            distanceInCm = ping();
            if(distanceInCm < 7.0)
            {
                trns('s');
                trns('t');
                trns('o');
                trns('p');
            }
            else
            {
                trns('r');
                trns('i');
                trns('j');
                trns('d');
                trns('e');
                trns('n');
            }

            trns('\n');
        }

        return 0;
}

double currenttime()
{
    return milliseconden;
}

/* enable timer */
void pingklok_aan()
{
    TCCR1B = (1<<CS11); //prescaler is 8
}

void trns(unsigned char c)
{
    while((UCSR0A & (1<<UDRE0)) == 0); //Wacht tot het register leeg is
    UDR0 = c; // char weer opsturen (UDR = USART data register)
    PORTB = 0b01000000; //PortB6 aan (pin 12)
}

/* enable usart */
void USART_Init()
{
    UBRR0L = 103; //baud rate 9600, F_CPU 16000000)

    UCSR0B = (1<<RXEN0) | (1<<TXEN0); //enable receive data and transmit data
    UCSR0C = (1<<UCSZ00) | (1<<UCSZ01); //8 data bits and 1 stop bit

    DDRB = 0xFF; //Zet port B als Output
    PORTB = 0x00; //PortB op LOW(input) zetten
}


void initcurrenttime()
{
    TCCR0A |= (1<<WGM01); //enable ctc mode
    TCCR0B |= (1<<CS01) | (1<<CS00); //prescaler 64.
    OCR0A = 249;//16000000HZ dus 16000 pulsen per milliseconden. 16000 / 64 = 250. -1 want de stap van 249 naar 0 is ook een puls
    TIMSK0 = (1<<OCIE0A);//enable compare interrupt
    sei(); //set global interrupt flag
}

double return_microseconden_en_pingklok_uit()
{
    double usec;

    cli(); //disable interrupts
    usec = TCNT1; //stel usec gelijk aan aantal klokpulsen.
    sei(); //enable interrupts

    TCCR1B = 0; //disable clock

    cli(); //16bit register schrijven dus liever geen interrupt
    TCNT1 = 0; //reset clock
    sei(); //enable interrupts

    usec = usec / 2.0; //prescaler is 8, cpu frequency is 16mhz, 16*10^6(16mhz) / 10^6(microseconden?) / 8(prescaler) = 2 pulses per microsecond

    return usec;
}

/* enable ping 28015 */
double ping()
{
    double echoTime;
    double distance;

    /* delay before next measurement */
    _delay_us(200);

    /* send ultrasonic signal and start timer */
    DDRL = 255; //ddrl output
    PORTL |= (1 << 1); //enable ping (pin l1)

    _delay_us(5);

	PORTL &= ~(1 << 1); //disable ping (pin l1)
    pingklok_aan(); //start 16-bit timer

    /* holdoff */
    _delay_us(750);

    /*wacht op input */
    DDRL = 0; //ddrl input
    PORTL |= (1 << 1); //enable ping (pin l1)

    while((PINL & (1 << 1)) == 0)
    {
        if()
    }

    /*ultrasoon signaal wordt ontvangen */
    while((PINL & (1 << 1)) == 1)
    {
        echoTime = return_microseconden_en_pingklok_uit();
        PORTL &= ~(1 << 1); //disable ping
        distance = (echoTime) * 0.017175) ; //reken afstand uit in cm

        return distance;
    }
}

