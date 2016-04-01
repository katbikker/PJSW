#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>
#include <stdio.h>


void USART_Init(){

UBRR0H=00;  //
UBRR0L=103; //baud rate 9600 bij (16.0000MHz)

UCSR0B = (1<<RXEN0) | (1<<TXEN0); 		// Enable de recieve and transmit data (status register B)
UCSR0C = (1<<UCSZ00) | (1<<UCSZ01);		// 8 data bits and 1 stop bit (in status register C)

DDRB = 0xFF;		//Zet al port B als Output
PORTB = 0x00; 		//PortB output op LOW(uit) zetten
}

int main(void) {
  USART_Init ();

  unsigned char c;

  while (1) {
      c = rcv();       //ontvang char a
      trns(c);          //    transmit received character
      for(uint8_t i=0;i<5;++i)
         _delay_ms(500); //delay 0,5sec
         PORTB = 0; // portb weer uitzetten (LOW)

  }
  return (0);
}

void trns(unsigned char c){
   while((UCSR0A & (1<<UDRE0)) == 0); //Wacht tot de register leeg is
   UDR0 = c; // char weer opsturen (UDR = USART data register)
   PORTB = 0b01000000; //PortB6 aan (pin 12)
}

unsigned char rcv(){
      while((UCSR0A & (1<<RXC0)) == 0); //wacht tot het vol is
      return UDR0;
}

