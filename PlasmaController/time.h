/*
 * time.h
 *
 * Created: 06.11.2016 17:58:10
 *  Author: Serhg
 */ 


#ifndef TIME_H_
#define TIME_H_


#include <avr/io.h>
#include <avr/interrupt.h>
#include "conf.h"
#include "binary.h"

#define clockCyclesPerMicrosecond() ( F_CPU / 1000000L )
#define clockCyclesToMicroseconds(a) ( (a) / clockCyclesPerMicrosecond() )
#define MICROSECONDS_PER_TIMER0_OVERFLOW (clockCyclesToMicroseconds(64 * 256))

#define MILLIS_INC (MICROSECONDS_PER_TIMER0_OVERFLOW / 1000)

#define FRACT_INC ((MICROSECONDS_PER_TIMER0_OVERFLOW % 1000) >> 3)
#define FRACT_MAX (1000 >> 3)

volatile unsigned long timer0_millis = 0;
static unsigned char timer0_fract = 0;


ISR(TIMER0_OVF_vect)
{
	unsigned long m = timer0_millis;
	unsigned char f = timer0_fract;
	m += MILLIS_INC;
	f += FRACT_INC;
	if (f >= FRACT_MAX) {
		f -= FRACT_MAX;
		m += 1;
	}
	timer0_fract = f;
	timer0_millis = m;
}

unsigned long millis()
{
	unsigned long m;
	uint8_t oldSREG = SREG;
	
	cli();
	m = timer0_millis;
	SREG = oldSREG;

	return m;
}

void initTime()
{
	sei();
	SetBit(TCCR0A, WGM01);
	SetBit(TCCR0A, WGM00);

	SetBit(TCCR0B, CS01);
	SetBit(TCCR0B, CS00);

	SetBit(TIMSK0, TOIE0);
}

#endif /* TIME_H_ */