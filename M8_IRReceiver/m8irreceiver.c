/*
 * m8_irreceiver.c
 *
 *  Created on: 24.11.2013
 *      Author: sieb
 */



#include <avr/io.h>
#include <avr/interrupt.h>

volatile unsigned char dataReceived = 0;
volatile unsigned char allReceived = 0;

int main(void)
{
	// Init PB0 (Pin 14) : Output, Low
	DDRB |= 1 << PB0;
	PORTB &= ~(1 << PB0);
	// Init PD7 (Pin 13) : Output, Low
	DDRD |= 1 << PD7;
	PORTD &= ~(1 << PD7);
	// Init PC2, PC5 (Pin 25, 28): Output, Low
	DDRC |= 1 << PC2;
	DDRC |= 1 << PC5;
	PORTC &= ~(1 << PC2);
	PORTC &= ~(1 << PC5);
	// Init PB1 (Pin 15) : Input, Pullup aktiv
	DDRB &= ~(1 << PB1);
	PORTB |= 1 << PB1;
	// Init PB0 (Pin 2) : Input, Pullup aktiv
	DDRD &= ~(1 << PD0);
	PORTD |= 1 << PD0;

	TCCR0 = 0 << CS02 | 1 << CS01 | 1 << CS00;
	TIMSK |= 1 << TOIE0;

	sei();

	char output = 0;
	while( 1 )
	{
		while( !allReceived )
		{

		}
		allReceived = 0;
		output = (dataReceived - 3) >> 2;
		PORTD &= ~(1 << PD7);
		PORTB &= ~(1 << PB0);
		PORTC &= ~(1 << PC2 | 1 << PC5);
		PORTD |= (output & (1 << 3)) >> 3 << PD7;
		PORTB |= (output & (1 << 2)) >> 2 << PB0;
		PORTC |= (output & (1 << 1)) >> 1 << PC5;
		PORTC |= (output & (1 << 0)) >> 0 << PC2;
	}

}

ISR(TIMER0_OVF_vect)
{
	static char lastInput = 0;
	static char lastInputSet = 0;
	static char silenceInput = 0;
	static char equalCount = 0;
	static char currentBit = 100;
	static char lastBitHigh = 0;
	char input = PINB & (1 << PB1);

	if( !lastInputSet )
	{
		lastInput = input;
		lastInputSet = 1;
		silenceInput = input;
	}

//		PORTC &= ~(1 << PC2);
//		PORTC |= ((PINB & (1 << PB1)) >> PB1) << PC2;

	if( currentBit > 7 && currentBit != 100 )
	{
		allReceived = 1;
		currentBit = 100;
		lastBitHigh = 0;
	}
	else if( input == lastInput )
	{
		//		PORTD ^= 1 << PD7;
		equalCount++;
		if( input == silenceInput && equalCount >= 9 && currentBit != 100 )
		{
			currentBit++;
			//equalCount = 0;
		}
	}
	else
	{
		//		PORTB |= 1 << PB0;
		if( !lastBitHigh )
		{
			if( currentBit == 100 )
			{
				// Init
				allReceived = 0;
				currentBit = -1;
				dataReceived = 0;
			}
			else if( equalCount < 3 )
			{
				// Fehler
//				PORTB ^= 1 << PB0;
			}
			else if( equalCount <= 5 )
			{
				dataReceived |= 1 << currentBit;
				lastBitHigh = 1;
			}
			else if( equalCount > 9)
			{
				// Fehler
//				PORTD ^= 1 << PD7;
			}
			currentBit++;
		}
		else
		{
			lastBitHigh = 0;
		}
		equalCount = 1;
		lastInput = input;
	}
}
