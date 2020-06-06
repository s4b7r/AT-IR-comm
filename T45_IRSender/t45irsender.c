/*
 * t45irsender.c
 *
 *  Created on: 24.11.2013
 *      Author: sieb
 */



#include <avr/io.h>
#include <avr/interrupt.h>

volatile unsigned char dataToSend = 0;
volatile char allSend = 0;

int main(void)
{
	DDRB |= (1 << PB2); // PB2 = Output
	PORTB &= ~(1 << PB2); // PB2 = Low

	DDRB &= ~27; // PB0, PB1, PB3, PB4 = Input
	PORTB |= 27; // PB0, PB1, PB3, PB4 PullUp ein

	TCCR0B |= 1 << CS02 | 0 << CS01 | 0 << CS00;
	// TCCR0B: Timer/Counter Control Register B
	// CS: Clock Select

	TIMSK = 1 << TOIE0;
	// TIMSK: Timer/Counter Interrupt Mask
	// TOIE0: Timer/Counter0 Overflow Interrupt Enable

	//sei();

	char input = 0;
	char lastInput = input;

	while( 1 )
	{
		//input = (((~PINB & (1 << PB4)) >> PB4) << 3) + (((~PINB & (1 << PB3)) >> PB3) << 2) +
				//(((~PINB & (1 << PB0)) >> PB0) << 1) + (((~PINB & (1 << PB1)) >> PB1) << 0);
		//input = input < 15 ? input+1 : 0;
		switch (input) {
			case 0:
				input=1;
				break;
			case 1:
				input=2;
				break;
			case 2:
				input=4;
				break;
			case 4:
				input=8;
				break;
			case 8:
				input=15;
				break;
			case 15:
				input=7;
				break;
			case 7:
				input=3;
				break;
			case 3:
				input=5;
				break;
			case 5:
				input=10;
				break;
			case 10:
				input=12;
				break;
			case 12:
				input=6;
				break;
			default:
				break;
		}
		if( input != lastInput )
		{
			//dataToSend = 0b00000000;
			dataToSend = (input << 2) + 3;
			GTCCR |= 1 << PSR0;
			// GTCCR: General Timer/Counter Control Register
			// PSR0: Prescaler Reset Timer/Counter0
			TCNT0 = 0;
			// TCNT0: Timer/Counter Register
			sei();
			while( !allSend )
			{

			}
			cli();
			allSend = 0;
			lastInput = input;
		}
	}
}

ISR(TIMER0_OVF_vect)
{
	static char lastState = 0;
	static char currentBit = 0;
	if( currentBit > 7 )
	{
		lastState = 0;
		currentBit = 0;
		PORTB &= ~(1 << PB2);
		allSend = 1;
	}
	else
	{
		char toggleMask = ((~lastState | dataToSend) & (1 << currentBit)) >> currentBit << PB2;
		PORTB ^= toggleMask;
		currentBit = lastState ? currentBit + 1 : currentBit;
		lastState = ~lastState;
	}
}
