#define F_CPU 16000000

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>


volatile unsigned long milliseconden; //global variable
volatile uint8_t RP6Opdracht;//RP6 Opdracht Register
//bit7 noodsituatie   1 = noodsituatie of niet storen
//bit6 bumper informatie opvragen
//bit5 autonoom       1 = autonoom      0 = handmatig
//bit4 stuurrichting. 1 = links         0 = rechts
//bit3 sturen         1 = sturen        0 = rechtdoor
//bit2 rijrichting    1 is achteruit    0 = vooruit
//bit1 snelheid       Bit 1 en 0 bepalen de snelheid
//bit0 snelheid       0, 64, 128 of 192 / 256 x 100%


unsigned long currentTime();
(1<<6)
void currentTimeInit();
void pingClockEnable(); //enable timer
unsigned int returnUsAndPingClockDisable();
double ping();
void usartInit(); //enable usart
void trnsmitChar(unsigned char c);
void noodsituatie();
void autonoom();

ISR(TIMER0_COMPA_vect) //timer0 compare vector
{
    milliseconden++;
}

ISR(USART0_RX_vect)//usart ontvangt iets
{
    char c;
    c = UDR0;

    if((RP6Opdracht & (1<<7) == 0)
    {
        if((RP6Opdracht & (1<<5) == 0)
       {
            switch(c)
            {
            case 'w' :
                if((RP6Opdracht & (1<<2)) == 0)//als de rijrichting op vooruit staat
                {
                    if(RP6Opdracht & 0b00000011 == 0)
                    {
                        RP6Opdracht |= (1<<0);//zet snelheid op 1
                        //stuur shit naar rp6
                    }
                    else if(RP6Opdracht & 0b00000011 == 1)
                    {
                        RP6Opdracht &= ~(1<<0);//zet snelheid op 2
                        RP6Opdracht |= (1<<1);
                        //stuur shit naar rp6
                    }
                    else if(RP6Opdracht & 0b00000011 == 2)
                    {
                        RP6Opdracht |= (1<<0);//zet snelheid op 3
                        //stuur shit naar rp6
                    }
                    else
                    {
                        trnsmitChar('m');
                        trnsmitChar('a');
                        trnsmitChar('x');
                        trnsmitChar('\n');
                    }
                }
                else//als de rijrichting op achteruit staat
                {
                    if(RP6Opdracht & 0b00000011 == 1)
                    {
                        RP6Opdracht &= ~(1<<2) & ~(1<<0);//zet snelheid op 0 en rijrichting op vooruit
                        //stuur shit naar rp6
                    }
                    else if(RP6Opdracht & 0b00000011 == 2)
                    {
                        RP6Opdracht &= ~(1<<1)//zet snelheid op 1
                                       RP6Opdracht |= (1<<0)
                                                      //stuur shit naar rp6
                    }
                    else
                    {
                        RP6Opdracht &= ~(1<<0)//zet snelheid op 2
                                       //stuur shit naar rp6
                    }
                }
                break;
            case 's' :
                if((RP6Opdracht & (1<<2)) == 0)//als de rijrichting op vooruit staat
                {
                    if(RP6Opdracht & 0b00000011 == 0)
                    {
                        RP6Opdracht |= (1<<0) | (1<<2);//zet snelheid op 1 en rijrichting op achteruit
                        //stuur shit naar rp6
                    }
                    else if(RP6Opdracht & 0b00000011 == 1)
                    {
                        RP6Opdracht &= ~(1<<0);//zet snelheid op 0
                        //stuur shit naar rp6
                    }
                    else if(RP6Opdracht & 0b00000011 == 2)
                    {
                        RP6Opdracht &= ~(1<<1);//zet snelheid op 1
                        RP6Opdracht |= (1<<0);
                        //stuur shit naar rp6
                    }
                    else
                    {
                        RP6Opdracht &= ~(1<<0);//zet snelheid op 2
                        //stuur shit naar rp6
                    }
                }
                else//als de rijrichting op achteruit staat
                {
                    if(RP6Opdracht & 0b00000011 == 1)
                    {
                        RP6Opdracht &= ~(1<<0);//zet snelheid op 2
                        RP6Opdracht |= (1<<1);
                        //stuur shit naar rp6
                    }
                    else if(RP6Opdracht & 0b00000011 == 2)
                    {
                        RP6Opdracht |= (1<<0);//zet snelheid op 3
                        //stuur shit naar rp6
                    }
                    else
                    {
                        trnsmitChar('m');
                        trnsmitChar('a');
                        trnsmitChar('x');
                        trnsmitChar('\n');
                    }
                }
                break;
            case 'a' :
                if((RP6Opdracht & (1<<3)) == 0)//als hij rechtdooor aan het rijden is
                {
                    RP6Opdracht |= (1<<3) | (1<<4);//stuur naar links
                }
                else if((RP6Opdracht & (1<<4)) == 0)//als hij naar rechts aan het sturen is
                {
                    RP6Opdracht |= (1<<4);//stuur naar links
                }
                else//als hij naar links aan het sturen is
                {
                    RP6Opdracht &= ~(1<<3) & ~(1<<4);//stop met sturen
                }
                break;
            case 'd' :
                if((RP6Opdracht & (1<<3)) == 0)//als hij rechtdooor aan het rijden is
                {
                    RP6Opdracht |= (1<<3);//stuur naar rechts
                    RP6Opdracht &= ~(1<<4);
                }
                else if((RP6Opdracht & (1<<4)) != 0)//als hij naar links aan het sturen is
                {
                    RP6Opdracht &= ~(1<<4);//stuur naar rechts
                }
                else//als hij naar rechts aan het sturen is
                {
                    RP6Opdracht &= ~(1<<3) & ~(1<<4);//stop met sturen
                }
                break;
            case 't' :
                autonoom();
                break;
            case 'r' :
                if((RP6Opdracht & 0b00000011) == 0)
                {
                    trnsmitChar('0');
                    trnsmitChar(' ');
                    trnsmitChar('m');
                    trnsmitChar('/');
                    trnsmitChar('s');
                    trnsmitChar('\n');
                }
                else if((RP6Opdracht & 0b00000011) == 1)
                {
                    trnsmitChar('1');// niet correct
                    trnsmitChar(' ');
                    trnsmitChar('m');
                    trnsmitChar('/');
                    trnsmitChar('s');
                    trnsmitChar('\n');
                }
                else if((RP6Opdracht & 0b00000011) == 2)
                {
                    trnsmitChar('2');// niet correct
                    trnsmitChar(' ');
                    trnsmitChar('m');
                    trnsmitChar('/');
                    trnsmitChar('s');
                    trnsmitChar('\n');
                }
                else
                {
                    trnsmitChar('3');// niet correct
                    trnsmitChar(' ');
                    trnsmitChar('m');
                    trnsmitChar('/');
                    trnsmitChar('s');
                    trnsmitChar('\n');
                }
                break;
            default :
                trnsmitChar('w');
                trnsmitChar('a');
                trnsmitChar('s');
                trnsmitChar('d');
                trnsmitChar('t');
                trnsmitChar('r');
                trnsmitChar('\n');
            }
        }
        else//uit de autonome modus
        {
            RP6Opdracht = 0b00000000;
            //stuur shit
        }
    }
    else
    {
        trnsmitChar('w');
        trnsmitChar('a');
        trnsmitChar('c');
        trnsmitChar('h');
        trnsmitChar('t');
        trnsmitChar('\n');
    }
}

int main(void)
{
    unsigned long prevTime;
    double distanceInCm;

    RP6Opdracht = 0;

    sei();
    usartInit();
    currentTimeInit();

    while(1);
    {
        prevTime = currentTime();

        while((prevTime + 50) >= currentTime());//kijk 20 keer per seconde of de robot in gevaar is
        distanceInCm = ping();

        if(distanceInCm < 7)
        {
            noodsituatie();
        }

        RP6Opdracht |= (1<<6) | (1<<7);

        //stuur shit naar rp6

        RP6Opdracht &= ~(1<<6) & ~(1<<7);

        //als bumper geraakt is
        //{
        //noodsituatie();
        //}
    }
    return 0;
}


void autonoom()
{
    unsigned long prevTime;
    RP6Opdracht = 0b00100000;
    //stuur shit

    while(RP6Opdracht & (1<<5) != 0)
    {
        RP6Opdracht = 0b00100010;//rechtdoor
        //stuur shit

        while(1);
        {
            prevTime = currentTime();

            while((prevTime + 50) >= currentTime());//kijk 20 keer per seconde of de robot in gevaar is
            distanceInCm = ping();

            if(distanceInCm < 7)
            {
                break;
            }

            RP6Opdracht |= (1<<6) | (1<<7);

            //stuur shit naar rp6

            RP6Opdracht &= ~(1<<6) & ~(1<<7);

            //als bumper geraakt is
            //{
            //break;
            //}
        }
        RP6Opdracht |= (1<<2);//achteruit
        //stuur shit

        prevTime = currentTime();

        while((prevTime + 500) >= currentTime());

        RP6Opdracht = 0b0010100;//draaien
        //stuur shit

        //wachten tot 90 graden gedraaid is


    }


}

void usartInit()
{
    UBRR0L = 103; //baud rate 9600, F_CPU 16000000

    UCSR0B = (1<<RXEN0) | (1<<TXEN0) | (1<<RXCIE0); //enable receive data and transmit data en receive interrupt
    UCSR0C = (1<<UCSZ00) | (1<<UCSZ01); //8 data bits and 1 stop bit

    DDRB = 0xFF; //Zet port B als Output
    PORTB = 0x00; //PortB op LOW(input) zetten
}

/* transmit character */
void trnsmitChar(unsigned char c)
{
    while((UCSR0A & (1<<UDRE0)) == 0); //Wacht tot het register leeg is
    UDR0 = c; // char weer opsturen (UDR = USART data register)
    PORTB = 0b01000000; //PortB6 aan (pin 12)
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
    PORTL &= ~(1 << 1); //pin l1 low, clean signal
    _delay_us(1);
    PORTL |= (1 << 1); //pin l1 high, send signal
    _delay_us(4);
    PORTL &= ~(1 << 1); //pin l1 low

    /*wait for input */
    DDRL = 0; //ddrl input

    while ((PINL & (1 << 1)) == 0); //VERANDERING!!!!!!!!OMG!!!!!!!!!!!!!!!WACHTEN OP SIGNAAL

    /* start timer */
    pingClockEnable(); //start 16-bit timer

    /*ultrasoon signaal wordt ontvangen */
    while ((PINL & (1 << 1)) != 0); //VERANDERING!!!!!!!!OMG!!!!!!!!!!!!!!!
    echoTime = returnUsAndPingClockDisable();
    distance = (double)echoTime / 29 / 2; //reken afstand uit in cm
    return distance;
}

void noodsituatie()
{
    unsigned long prevTime;

    RP6Opdracht = 0b10000000;//robot staat stil en accepteert geen input meer
    //stuur shit
    trnsmitChar('n');
    trnsmitChar('o');
    trnsmitChar('o');
    trnsmitChar('d');
    trnsmitChar('s');
    trnsmitChar('i');
    trnsmitChar('t');
    trnsmitChar('u');
    trnsmitChar('a');
    trnsmitChar('t');
    trnsmitChar('i');
    trnsmitChar('e');
    trnsmitChar('\n');

    prevTime = currentTime();
    while((prevTime + 30000) >= currentTime());//wacht 30 seconden

    RP6Opdracht |= (1<<3) | (1<<4);//robot draait tegen de klok in
    //stuur shit

    //wachten tot hij ver genoeg gedraait is

    RP6Opdracht = 0b10000000;//robot staat stil
    //stuur shit
    RP6Opdracht = 0;//robot accepteert input
}
