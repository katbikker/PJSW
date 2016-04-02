#define F_CPU 16000000

#include <avr/io.h>

volatile uint8_t RP6Opdracht;//RP6 Opdracht Register
//bit7 noodsituatie   1 = noodsituatie
//bit6
//bit5
//bit4 stuurrichting. 1 = links         0 = rechts
//bit3 sturen         1 = sturen        0 = rechtdoor
//bit2 rijrichting    1 is achteruit    0 = vooruit
//bit1 snelheid       Bit 1 en 0 bepalen de snelheid
//bit0 snelheid       0, 64, 128 of 192 / 256 x 100%

ISR(USART0_RX_vect)//usart ontvangt iets
{
    char c;
    c = UDR0;

    if((RP6Opdracht & (1<<7) == 0)
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
            break;
            case 'd' :
            break;
            default :
            trnsmitChar('w');
            trnsmitChar('a');
            trnsmitChar('s');
            trnsmitChar('d');
            trnsmitChar('\n');
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
    void usartInit();
    void trnsmitChar(unsigned char c);
    RP6Opdracht = 0;
    sei();
    usartInit();
    while(1);
    return 0;
}

void usartInit()
{
    UBRR0L = 103; //baud rate 9600, F_CPU 16000000)

    UCSR0B = (1<<RXEN0) | (1<<TXEN0) | (1<<RXCIE0); //enable receive data and transmit data
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
