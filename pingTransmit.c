#define F_CPU 16000000

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

volatile unsigned long milliseconden; //global variable

double ping();
unsigned long currentTime();
unsigned int returnUsAndPingClockDisable();

void usartInit(); //enable usart
void pingClockEnable(); //enable timer
void currentTimeInit();
void transmitChar(unsigned char c);
void transmitStr(char *pointer)

/* isr */
ISR(TIMER0_COMPA_vect) //timer0 compare vector
{
    milliseconden++;
}

/* main */
int main(void)
{
    sei(); //set global interrupt flag
    currentTimeInit();
    usartInit();

    unsigned long prevTime;
    double distanceInCm;

    while(1)
    {
        prevTime = currentTime();

        while((prevTime + 1000) >= currentTime())
        {
            distanceInCm = ping();

            if(distanceInCm < 7)
            {
                transmitChar('s');
                transmitChar('t');
                transmitChar('o');
                transmitChar('p');
            }
            else
            {
                transmitChar('g');
                transmitChar('o');
            }

            transmitChar('\n');
            transmitStr("testStr"); //test string
            transmitChar('\n');
        }

        return 0;
}

unsigned long currentTime()
{
    return milliseconden;
}

void currentTimeInit()
{
    TCCR0A |= (1<<WGM01); //enable ctc mode
    TCCR0B |= (1<<CS01) | (1<<CS00); //prescaler 64.
    OCR0A = 249;//16000000HZ dus 16000 pulsen per milliseconden. 16000 / 64 = 250. -1 want de stap van 249 naar 0 is ook een puls
    TIMSK0 = (1<<OCIE0A);//enable compare interrupt
    sei(); //set global interrupt flag
}

/* enable timer */
void pingClockEnable()
{
    TCCR1B = (1<<CS11); //prescaler is 8
}

/* return microseconds and disable ping clock */
unsigned int returnUsAndPingClockDisable()
{
    unsigned int usec;

    cli(); //disable interrupts
    usec = TCNT1; //stel usec gelijk aan aantal klokpulsen.
    sei(); //enable interrupts

    TCCR1B = 0; //disable clock

    cli(); //16bit register schrijven dus liever geen interrupt
    TCNT1 = 0; //reset clock
    sei(); //enable interrupts

    usec = usec / 2; //prescaler is 8, cpu frequency is 16mhz, 16*10^6(16mhz) / 10^6(microseconden?) / 8(prescaler) = 2 pulses per microsecond

    return usec;
}

/* enable ping 28015 */
double ping()
{
    unsigned int echoTime;
    double distance;

    /* send ultrasonic signal */
    DDRL = 255; //d alles output
    PORTL &= ~(1 << 1);
    _delay_us(2);
    PORTL |= (1 << 1); //ping high. pin l1 is verbonden met sensor
    _delay_us(5);
    PORTL &= ~(1 << 1); 

    /*wait for input */
    DDRL = 0; //ddrl input

    /* start timer */
    pingClockEnable(); //start 16-bit timer

    /*ultrasoon signaal wordt ontvangen */
    while ((PINL & (1 << 1)) == 1);//VERANDERING!!!!!!!!OMG!!!!!!!!!!!!!!!
    echoTime = returnUsAndPingClockDisable()
    distance = (double)echoTime / 29 / 2; //reken afstand uit in cm
    return distance;
}

/* enable usart */
void usartInit()
{
    UBRR0L = 103; //baud rate 9600, F_CPU 16000000)

    UCSR0B = (1<<RXEN0) | (1<<TXEN0); //enable receive data and transmit data
    UCSR0C = (1<<UCSZ00) | (1<<UCSZ01); //8 data bits and 1 stop bit

    DDRB = 0xFF; //Zet port B als Output
    PORTB = 0x00; //PortB op LOW(input) zetten
}

/* transmit character */
void transmitChar(unsigned char c)
{
    while((UCSR0A & (1<<UDRE0)) == 0); //Wacht tot het register leeg is
    UDR0 = c; // char weer opsturen (UDR = USART data register)
    PORTB = 0b01000000; //PortB6 aan (pin 12)
}

/* transmit string */
void transmitStr(char *pointer)//VERANDERING!!!!!!!!OMG!!!!!!!!!!!!!!!
{
   while (*pointer != 0x00)
   {
      transmitChar(*pointer);
      pointer++;
   }
}
