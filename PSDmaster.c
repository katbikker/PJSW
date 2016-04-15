/*
 * test2.c
 *
 * Created: 3/20/2016 6:09:06 PM
 * Author : john
 */

#define F_CPU 16000000

#include <avr/io.h>
#include <util/delay.h>
#include "i2c_mst.h"


#define DEVICE_ADRES   8

#define KOMPAS 96 //1100000 = 0x60, 1100000 + R/W bit = 11000000 (0xC0 = 192, bit0 = 0 write) or 11000001 (0xC1 = 193, bit0 = 1 read)

#define KOMPAS_REG2 2 //register 2 is 16 bits (register 2,3)
#define KOMPAS_COMREG 15 //command register (8 bits) = 15

#include <avr/interrupt.h>

volatile char c;
volatile unsigned long milliseconden; //global variable
volatile uint8_t RP6Opdracht[5];//RP6 Opdracht Register
//bit7 noodsituatie   1 = noodsituatie of niet storen
//bit6 nieuwe opdracht ontvangen
//bit5 autonoom       1 = autonoom      0 = handmatig
//bit4 stuurrichting. 1 = links         0 = rechts
//bit3 sturen         1 = sturen        0 = rechtdoorw
//bit2 rijrichting    1 is achteruit    0 = vooruit
//bit1 snelheid       Bit 1 en 0 bepalen de snelheid
//bit0 snelheid       0, 64, 128 of 192 / 256 x 100%


unsigned long currentTime();
void currentTimeInit();
void pingClockEnable(); //enable timer
unsigned int returnUsAndPingClockDisable();
double ping();
void usartInit(); //enable usart
void trnsmitChar(unsigned char c);
void noodsituatie();
void autonoom();
void calKompas(void);
double kompas(void);
void opdrachtOntvangen();

ISR(TIMER0_COMPA_vect) //timer0 compare vector
{
    milliseconden++;
}

ISR(USART0_RX_vect)//usart ontvangt iets
{
    c = UDR0; //lees het register van de usart uit
    if((RP6Opdracht[0] & (1<<5)) != 0) // Als de robot autonoom rijd
    {
        RP6Opdracht[0] = 0b00000000; //robot stopt
        verzenden(DEVICE_ADRES,RP6Opdracht[0]);
    }
    else
    {
        RP6Opdracht[0] |= (1<<6); //Ontvang opdracht
    }
}

int main(void)
{
    unsigned long prevTime;
    double distanceInCm;

    RP6Opdracht[0] = 0;

    sei();
    usartInit();
    currentTimeInit();

    PORTD = 0x03; //pullup SDA en SCL
    uint8_t data[10];
    init_master();

    while(1)
    {
        prevTime = currentTime();

        if((RP6Opdracht[0] & (1<<6)) != 0) //als er een opdracht is ontvangen
        {
            opdrachtOntvangen();
        }

        if((RP6Opdracht[0] & (1<<5)) != 0) //opdracht autonoom ontvangen
        {
            autonoom();
        }

        while((prevTime + 50) >= currentTime());//kijk 20 keer per seconde of de robot in gevaar is
        distanceInCm = ping();

        if(distanceInCm < 7)
        {
            noodsituatie();
        }
        _delay_ms(5); //Visser's code heeft tijd nodig
        ontvangen(DEVICE_ADRES,data,1);
        if(data[0] == 255) //Bumper geraakt
        {
            noodsituatie();
        }
    }
    return 0;
}

void opdrachtOntvangen()
{
    cli();
    RP6Opdracht[0] &= ~(1<<6); //zet opdracht ontvangen op 0

    if((RP6Opdracht[0] & (1<<7)) == 0) //niet in noodsituatie
    {
            switch(c)
            {
            case 'w' :
                if((RP6Opdracht[0] & (1<<2)) == 0)//als de rijrichting op vooruit staat
                {
                    if((RP6Opdracht[0] & 0b00000011) == 0) //Als de snelheid 0 is
                    {
                        RP6Opdracht[0] |= (1<<0);//zet snelheid op 1
                        verzenden(DEVICE_ADRES,RP6Opdracht[0]);
                    }
                    else if((RP6Opdracht[0] & 0b00000011) == 1) //Als de snelheid 1 is
                    {
                        RP6Opdracht[0] &= ~(1<<0);//zet snelheid op 2
                        RP6Opdracht[0] |= (1<<1);
                        verzenden(DEVICE_ADRES,RP6Opdracht[0]);
                    }
                    else if((RP6Opdracht[0] & 0b00000011) == 2) //Als de snelheid 2 is
                    {
                        RP6Opdracht[0] |= (1<<0);//zet snelheid op 3
                        verzenden(DEVICE_ADRES,RP6Opdracht[0]);
                    }
                    else
                    {
                        trnsmitChar('m');
                        trnsmitChar('a');
                        trnsmitChar('x');
                        trnsmitChar('\n');
                        trnsmitChar('\r');
                    }
                }
                else//als de rijrichting op achteruit staat
                {
                    if((RP6Opdracht[0] & 0b00000011) == 1) //Als de snelheid 1 is
                    {
                        RP6Opdracht[0] &= ~(1<<2) & ~(1<<0);//zet snelheid op 0 en rijrichting op vooruit
                        verzenden(DEVICE_ADRES,RP6Opdracht[0]);
                    }
                    else if((RP6Opdracht[0] & 0b00000011) == 2) //Als de snelheid 2 is
                    {
                        RP6Opdracht[0] &= ~(1<<1);//zet snelheid op 1
                        RP6Opdracht[0] |= (1<<0);
                        verzenden(DEVICE_ADRES,RP6Opdracht[0]);
                    }
                    else
                    {
                        RP6Opdracht[0] &= ~(1<<0);//zet snelheid op 2
                        verzenden(DEVICE_ADRES,RP6Opdracht[0]);
                    }
                }
                break;
            case 's' :
                if((RP6Opdracht[0] & (1<<2)) == 0)//als de rijrichting op vooruit staat
                {
                    if((RP6Opdracht[0] & 0b00000011) == 0) //Als de snelheid 0 is
                    {
                        RP6Opdracht[0] |= (1<<0) | (1<<2);//zet snelheid op 1 en rijrichting op achteruit
                        verzenden(DEVICE_ADRES,RP6Opdracht[0]);
                    }
                    else if((RP6Opdracht[0] & 0b00000011) == 1) //Als de snelheid 1 is
                    {
                        RP6Opdracht[0] &= ~(1<<0);//zet snelheid op 0
                        verzenden(DEVICE_ADRES,RP6Opdracht[0]);
                    }
                    else if((RP6Opdracht[0] & 0b00000011) == 2) //Als de snelheid 2 is
                    {
                        RP6Opdracht[0] &= ~(1<<1);//zet snelheid op 1
                        RP6Opdracht[0] |= (1<<0);
                        verzenden(DEVICE_ADRES,RP6Opdracht[0]);
                    }
                    else
                    {
                        RP6Opdracht[0] &= ~(1<<0);//zet snelheid op 2
                        verzenden(DEVICE_ADRES,RP6Opdracht[0]);
                    }
                }
                else//als de rijrichting op achteruit staat
                {
                    if((RP6Opdracht[0] & 0b00000011) == 1) //Als de snelheid 1 is
                    {
                        RP6Opdracht[0] &= ~(1<<0);//zet snelheid op 2
                        RP6Opdracht[0] |= (1<<1);
                        verzenden(DEVICE_ADRES,RP6Opdracht[0]);
                    }
                    else if((RP6Opdracht[0] & 0b00000011) == 2) //Als de snelheid 2 is
                    {
                        RP6Opdracht[0] |= (1<<0);//zet snelheid op 3
                        verzenden(DEVICE_ADRES,RP6Opdracht[0]);
                    }
                    else
                    {
                        trnsmitChar('m');
                        trnsmitChar('a');
                        trnsmitChar('x');
                        trnsmitChar('\n');
                        trnsmitChar('\r');
                    }
                }
                break;
            case 'a' :
                if((RP6Opdracht[0] & (1<<3)) == 0)//als hij rechtdooor aan het rijden is
                {
                    RP6Opdracht[0] |= (1<<3) | (1<<4);//stuur naar links
                    verzenden(DEVICE_ADRES,RP6Opdracht[0]);
                }
                else if((RP6Opdracht[0] & (1<<4)) == 0)//als hij naar rechts aan het sturen is
                {
                    RP6Opdracht[0] &= ~(1<<3) & ~(1<<4);//stop met sturen
                    verzenden(DEVICE_ADRES,RP6Opdracht[0]);
                }
                break;
            case 'd' :
                if((RP6Opdracht[0] & (1<<3)) == 0)//als hij rechtdooor aan het rijden is
                {
                    RP6Opdracht[0] |= (1<<3);//stuur naar rechts
                    RP6Opdracht[0] &= ~(1<<4);
                    verzenden(DEVICE_ADRES,RP6Opdracht[0]);
                }
                else if((RP6Opdracht[0] & (1<<4)) != 0)//als hij naar links aan het sturen is
                {
                    RP6Opdracht[0] &= ~(1<<3) & ~(1<<4);//stop met sturen
                    verzenden(DEVICE_ADRES,RP6Opdracht[0]);
                }
                break;
            case 't' :
                RP6Opdracht[0] |= (1<<5); //Begin autonoom te rijden
                break;
            case 'r' :
                if((RP6Opdracht[0] & 0b00000011) == 0) //Als de snelheid 0 is
                {
				    trnsmitChar('0');
				    trnsmitChar(' ');
				    trnsmitChar('m');
				    trnsmitChar('/');
				    trnsmitChar('s');
				    trnsmitChar('\n');
				    trnsmitChar('\r');
                }
                else if((RP6Opdracht[0] & 0b00000011) == 1) //Als de snelheid 1 is
                {
					trnsmitChar('0');
					trnsmitChar(',');
					trnsmitChar('0');
					trnsmitChar('5');
					trnsmitChar(' ');
					trnsmitChar('m');
					trnsmitChar('/');
					trnsmitChar('s');
					trnsmitChar('\n');
					trnsmitChar('\r');
                }
                else if((RP6Opdracht[0] & 0b00000011) == 2) //Als de snelheid 2 is
                {
                    trnsmitChar('0');
                    trnsmitChar(',');
                    trnsmitChar('1');
                    trnsmitChar('2');
                    trnsmitChar('m');
                    trnsmitChar('/');
                    trnsmitChar('s');
                    trnsmitChar('\n');
                    trnsmitChar('\r');
                }
                else
                {
					trnsmitChar('0');
					trnsmitChar(',');
					trnsmitChar('2');
					trnsmitChar(' ');
					trnsmitChar('m');
					trnsmitChar('/');
					trnsmitChar('s');
					trnsmitChar('\n');
					trnsmitChar('\r');
                }
                break;
                case 'k' :
                calKompas(); //Begin kalibratie van het kompas
                break;
            default : //Beschikbare toetsen
                trnsmitChar('w');
                trnsmitChar('a');
                trnsmitChar('s');
                trnsmitChar('d');
                trnsmitChar('t');
                trnsmitChar('r');
                trnsmitChar('k');
                trnsmitChar('\n');
                trnsmitChar('\r');
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
        trnsmitChar('\r');
    }

    sei();
}

void autonoom()
{
    unsigned long prevTime;
    uint8_t data[10];

    double startBearing;
    double endBearing;
    double distanceInCm;
    int status;

    while((RP6Opdracht[0] & (1<<5)) != 0) //Zolang de rp6 op autonoom staat
    {
        status = 1;
        RP6Opdracht[0] = 0b00100010;//rechtdoor
        verzenden(DEVICE_ADRES,RP6Opdracht[0]);

        while((status) && ((RP6Opdracht[0] & (1<<5)) != 0)) //Zolang de rp6 op autonoom staat en er geen noodgeval is
        {
            prevTime = currentTime();

            while((prevTime + 50) >= currentTime());//kijk 20 keer per seconde of de robot in gevaar is
            distanceInCm = ping();

            if(distanceInCm < 7)
            {
                status = 0;
            }

            ontvangen(DEVICE_ADRES,data,1);
            if(data[0] == 255) //als de bumper geraakt is
            {
                status = 0;
            }
        }
        RP6Opdracht[0] |= (1<<2);//achteruit
        verzenden(DEVICE_ADRES,RP6Opdracht[0]);
        prevTime = currentTime();
        while((prevTime + 500) >= currentTime()); //wacht 500ms
        startBearing = kompas();
        RP6Opdracht[0] = 0b00101000;//draaien
        verzenden(DEVICE_ADRES,RP6Opdracht[0]);

        //wachten tot 90 graden gedraaid is
        if(startBearing >= 270) //Als de bearing hoger is als 270 dan is de nieuwe waarde 0-90 oftewel begin - 270
        {
            endBearing = startBearing - 270;
            while(!(kompas() >= endBearing) && !(kompas() < (endBearing + 50))); //Kijk of het kompas 90 graden heeft gedraaid of eventueel net iets meer.
        }
        else
        {
            endBearing = startBearing + 90;
            while(!(kompas() >= endBearing) && !(kompas() < startBearing)); //Kijk of het kompas groter is dan de endbearing of kleiner is dan de beginbearing
        }


    }
    RP6Opdracht[0] = 0b00000000; //stop rp6
    verzenden(DEVICE_ADRES,RP6Opdracht[0]);

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

    while ((PINL & (1 << 1)) == 0); //wacht op signaal

    /* start timer */
    pingClockEnable(); //start 16-bit timer

    /*ultrasoon signaal wordt ontvangen */
    while ((PINL & (1 << 1)) != 0); //wacht tot signaal stopt
    echoTime = returnUsAndPingClockDisable();
    distance = (double)echoTime / 29 / 2; //reken afstand uit in cm
    return distance;
}

void noodsituatie()
{
    unsigned long prevTime;
    double startBearing;
    double endBearing;
    RP6Opdracht[0] = 0b10000000;//robot staat stil en accepteert geen input meer
    verzenden(DEVICE_ADRES,RP6Opdracht[0]);
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
    trnsmitChar('\r');

    prevTime = currentTime();
    while((prevTime + 30000) >= currentTime());//wacht 30 seconden
    startBearing = kompas();
    RP6Opdracht[0] |= (1<<3) | (1<<4);//robot draait tegen de klok in
    verzenden(DEVICE_ADRES,RP6Opdracht[0]);
    if(startBearing < 180)
    {
        endBearing = startBearing + 180;
        while(!(kompas() >= endBearing)); //Kijk of kompas de draai gemaakt heeft
    }
    else
    {
        endBearing = startBearing - 180; // 180 of hoger + 180 = 0 of hoger dus startbearing -180
        while(!(kompas() <= endBearing)); //Kijk of kompas de draai gemaakt heeft
    }

    RP6Opdracht[0] = 0b10000000;//robot staat stil
    verzenden(DEVICE_ADRES,RP6Opdracht[0]);
    RP6Opdracht[0] = 0; //robot accepteert input
}

double kompas(void)
{
    uint8_t data[10];
    verzenden(KOMPAS, KOMPAS_REG2);
    for(uint8_t i = 0; i < 8; i++) _delay_ms(250); //wacht 2 sec

    ontvangen(KOMPAS, data, 2); //master ontvangt 2 bytes van slave (register 2,3)

    uint8_t highByte = data[0]; //register 2 (0-255)
    uint8_t lowByte = data[1];  //register 3
    double bearing = ((highByte << 8) + lowByte) / 10; //0-3599 / 10 = 0-359.9

    return bearing; //print bearing
}

void calKompas(void)
{
    writeString("\n\rKalibratie: ");
    verzenden(KOMPAS, KOMPAS_COMREG);
    writeString("\n\rVerzonden ");
    for(uint8_t i = 0; i < 8; i++) _delay_ms(250); //Wacht 2 sec

    for(uint8_t i = 0; i < 4; i++) //Verstuurr 4x een waarde van 255 voor het kalibreren
    {
        verzenden(KOMPAS, 0xFF);
        writeString("\n\rDraai");
        _delay_ms(10000);
    }
}
