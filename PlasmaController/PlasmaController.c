/*
 * PlasmaController.cpp
 *
 * Created: 30.10.2016 18:03:13
 *  Author: Serhg
 */ 

#include "conf.h"
#include <stdio.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <avr/eeprom.h>
#include <avr/wdt.h>
#include <stdbool.h>
#include "lcd-library.h"
#include "encoder.h"
#include "custom_symbols.h"
#include "millis.h"

uint16_t set_voltage;
uint16_t real_voltage = 0;
float set_delay;
float post_delay;

bool emerg_stop = 0;
volatile bool torch_on = 0;
bool torch_on_out = 0;
volatile bool arc_on_control_by_input = 0;
volatile bool arc_on = 0;
volatile bool arc_on_out = 0;
bool up = 0;
bool down = 0;

//volatile unsigned long delay_start_time;
volatile bool delay_start_ena = 0;

char stringOne[17];
char stringTwo[17];
short display_mutex;
volatile short display_changed;

//??????? ?????????? ???????? ?????????? INT0- ARC_ON-????
ISR( INT0_vect )
{
	if (PIND & (1 << PIND2))
	{
		arc_on = 0;
		arc_on_control_by_input = 0;
		delay_start_ena = 0;
		display_changed|=4;
	}
	else
	if (torch_on)
	{
		arc_on = 1;
		arc_on_control_by_input = 1;
		delay_start_ena = 1;
		millis_reset();
		display_changed|=4;
	}
}
//??????? ?????????? ???????? ?????????? INT1- torch_ON-????
ISR( INT1_vect )
{
	if (PIND & (1 << PIND3))
	{
		torch_on = 1;
	}
	else
	{
		torch_on = 0;
	}
}

int analogRead(int An_pin)
{
	ADMUX=An_pin;   
	_delay_us(10);	  
	ADCSRA=B11000110;	//B11000111-125kHz B11000110-250kHz 
	while (ADCSRA & (1 << ADSC));
	An_pin = ADCL;
	int An = ADCH; 
	return (An<<8) + An_pin;
}

void customSumbolLoad()
{
	lcdLoadCharacterf(CHAR_THUNDER_CODE, char_thunder);
	lcdLoadCharacterf(UP_ARROW_CODE, up_arrow);
	lcdLoadCharacterf(DOWN_ARROW_CODE, down_arrow);
	lcdLoadCharacterf(TORCH_CHAR_CODE, torch_char);
	lcdLoadCharacterf(LOCK_CHAR_CODE, lock_char);
	lcdLoadCharacterf(Y_CHAR_CODE, y_char);
	lcdLoadCharacterf(D_CHAR_CODE, d_char);
	lcdLoadCharacterf(I_CHAR_CODE, i_char);
}

void displayRefrash()
{
	if (display_changed)
	if (!display_mutex)
	{
		//lcdClear();
		//stringOne[16]='\0';
		//stringTwo[16]='\0';
		//sprintf(stringOne, " E CT:%3d", real_voltage);
		//stringOne[0]=D_CHAR_CODE;
		//stringOne[2]=I_CHAR_CODE;
		//lcdPuts(stringOne);
		//lcdGotoXY(1,0);
		sprintf(stringTwo, " CTAH:%3d       ", set_voltage);
		stringTwo[0]=Y_CHAR_CODE;
		//lcdPuts(stringOne);
		
		//lcdGotoXY(0,12);
		int d1;            // Get the integer part (678).
		float f2;
		if(!emerg_stop)
		{
			d1 = set_delay;            // Get the integer part (678).
			f2 = set_delay - d1;     // Get fractional part (678.0123 - 678 = 0.0123).
			int d2 = trunc(f2 * 10);   // Turn into integer (123).
			sprintf(stringOne, " E CT:%3d   %d.%01d ", real_voltage, d1, d2);
		}
		else
		{
			d1 = post_delay;            // Get the integer part (678).
			f2 = post_delay - d1;     // Get fractional part (678.0123 - 678 = 0.0123).
			int d2 = trunc(f2 * 10);   // Turn into integer (123).
			sprintf(stringOne, " E CT:%3d  P%d.%01d ", real_voltage, d1, d2);
		}
		stringOne[0]=D_CHAR_CODE;
		stringOne[2]=I_CHAR_CODE;
		//sprintf(stringOne, "%d.%01d\0", d1, d2);
		//lcdPuts(stringOne);
		if(!emerg_stop){
			if (arc_on_out)
			{
				//lcdGotoXY(0,10);
				stringOne[10]=CHAR_THUNDER_CODE;
				//stringOne[1]=0;
				//lcdPuts(stringOne);
			}
			if (torch_on_out)
			{
				//lcdGotoXY(1,13);
				stringTwo[13]=TORCH_CHAR_CODE;
				//stringOne[1]=0;
				//lcdPuts(stringOne);
			}
			if (up)
			{
				//lcdGotoXY(1,10);
				stringTwo[10]=UP_ARROW_CODE;
				//stringOne[1]=0;
				//lcdPuts(stringOne);
			}
			if (down)
			{
				//lcdGotoXY(1,10);
				stringTwo[10]=DOWN_ARROW_CODE;
				//stringOne[1]=0;
				//lcdPuts(stringOne);
			}
			if (delay_start_ena)
			{
				//lcdGotoXY(1,10);
				stringTwo[10]=LOCK_CHAR_CODE;
				//stringOne[1]=0;
				//lcdPuts(stringOne);
			}
		}
		else
		{
			//lcdGotoXY(1,13);
			stringTwo[13]='E';
			//stringOne[1]=0;
			//lcdPuts(stringOne);
		}
		//lcdClear();
		stringOne[16]=0;
		stringTwo[16]=0;
		if (!lcdGotoXY(0,0))
		{
			lcdInit();
			customSumbolLoad();
			display_changed = 1;
		}
		lcdPuts(stringOne);
		if (!lcdGotoXY(1,0))
		{
			lcdInit();
			customSumbolLoad();
			display_changed = 1;
		}
		lcdPuts(stringTwo);
		display_changed=0;
	}
}

ISR (TIMER1_COMPA_vect)
{
	displayRefrash();
}

void init()
{
	wdt_enable(WDTO_500MS);
	wdt_reset();
	_delay_ms(50);
	lcdInit();
	wdt_reset();
	set_voltage=eeprom_read_word(0);
	set_delay=((float)eeprom_read_word(3))/10;
	post_delay=((float)eeprom_read_word(6))/10;
	if ((set_voltage>MAX_VOLTAGE)||(set_voltage<MIN_VOLTAGE))
	{
		set_voltage = 140;
		eeprom_write_word(0, (uint16_t)(set_voltage*10));
	}
	if ((set_delay>MAX_DELAY)||(set_delay<MIN_DELAY))
	{
		set_delay = 1.0;
		eeprom_write_word(3, (uint16_t)(set_delay*10));
	}
	if ((post_delay>MAX_DELAY)||(post_delay<MIN_DELAY))
	{
		post_delay = 1.0;
		eeprom_write_word(6, (uint16_t)(post_delay*10));
	}
	
	millis_init();
	millis_resume();
	//?????? ?????????? ???????
    OCR1A = 0xFA0;
    TCCR1B |= (1 << WGM12);
    // Mode 4, CTC on OCR1A
    TIMSK1 |= (1 << OCIE1A);
    //Set interrupt on compare match
    TCCR1B |= (1 << CS12) | (1 << CS10);
    // set prescaler to 1024 and start the timer

	
	//????? ?????????? ????????
	SetBit(TORCH_ON_OUT_DDR, TORCH_ON_OUT);
	ClearBit(TORCH_ON_OUT_PORT, TORCH_ON_OUT);
	//????? ???-??
	SetBit(ARC_ON_OUT_DDR, ARC_ON_OUT);
	SetBit(ARC_ON_OUT_PORT, ARC_ON_OUT);
	//????? E_STOP
	SetBit(E_STOP_OUT_DDR, E_STOP_OUT);
	SetBit(E_STOP_OUT_PORT, E_STOP_OUT);
	//????? UP
	SetBit(UP_OUT_DDR, UP_OUT);
	SetBit(UP_OUT_PORT, UP_OUT);
	//????? DOWN
	SetBit(DOWN_OUT_DDR, DOWN_OUT);
	SetBit(DOWN_OUT_PORT, DOWN_OUT);
	
	//???? E_STOP
	ClearBit(E_STOP_IN_DDR, E_STOP_IN);
	SetBit(E_STOP_IN_PORT, E_STOP_IN);
	
	//???? INT0
	ClearBit(DDRD, 2);
	SetBit(PORTD, 2);
	//???? INT1
	ClearBit(DDRD, 3);
	SetBit(PORTD, 3);
	//?????????? ??? ???? ISCxx
	EICRA &= ~( (1<<ISC11)|(1<<ISC10)|(1<<ISC01)|(1<<ISC00) );
	//??????????? ?? ???????????? ?? ?????? ?????????
	EICRA |= (1<<ISC00)|(1<<ISC10);
	//????????? ??????? ??????????
	EIMSK |= (1<<INT0)|(1<<INT1);
	
	//????????????? ????????? ????????
	//if (PIND & (1 << PIND3))
	//	torch_on = 1;
	//else
	//	torch_on = 0;
	//if (PIND & (1 << PIND2))
	//	arc_on = 0;
	//else
	//	arc_on = 1;
	
	ENC_InitEncoder();

	customSumbolLoad();
	
	display_mutex = 0;
	display_changed = 3;
	sei();
}


void encoderProcess()
{
	unsigned char rotation = ENC_PollEncoder();
	if (rotation==RIGHT_SPIN)
	if (set_voltage>MIN_VOLTAGE) 
	{
		set_voltage--;
		display_changed|=1;
		eeprom_write_word(0, set_voltage);
	}
	if (rotation==LEFT_SPIN)
	if (set_voltage<MAX_VOLTAGE) 
	{
		set_voltage++;
		display_changed|=1;
		eeprom_write_word(0, set_voltage);
	}
		
	rotation = ENC_PollEncoderT();
	if (!emerg_stop)
	{
		if (rotation==RIGHT_SPIN)
		if (set_delay>MIN_DELAY)
		{
			set_delay-=0.1;
			display_changed|=2;
			eeprom_write_word(3, (uint16_t)(set_delay*10));
		}
		if (rotation==LEFT_SPIN)
		if (set_delay<MAX_DELAY)
		{
			set_delay+=0.1;
			display_changed|=2;
			eeprom_write_word(3, (uint16_t)(set_delay*10));
		}
	}
	else
	{
		if (rotation==RIGHT_SPIN)
		if (post_delay>MIN_DELAY)
		{
			post_delay-=0.1;
			display_changed|=2;
			eeprom_write_word(6, (uint16_t)(post_delay*10));
		}
		if (rotation==LEFT_SPIN)
		if (post_delay<MAX_DELAY)
		{
			post_delay+=0.1;
			display_changed|=2;
			eeprom_write_word(6, (uint16_t)(post_delay*10));
		}
	}
}

void logicProcess()
{
	if (!emerg_stop)
	{	
		if (torch_on_out!=torch_on) display_changed|=8;
		torch_on_out = torch_on;
		//???? ????????? ???? ?? ??????????? ?????? ????? ?
		//?????????? ? ????? ????????? - ??????? ??? ???? ???? ? ??? ????????? ????? ?????
		if ((!arc_on_control_by_input)&&(torch_on)){
		if ((real_voltage>=MIN_ARC_BY_ADC)&&(real_voltage<=MAX_ARC_BY_ADC))
		{
			if (!arc_on)
			{
				arc_on = 1;
				delay_start_ena = 1;
				display_changed|=4;
				millis_reset();
			}
		}
		else
		{
			if (arc_on)
			{
				arc_on = 0;
				display_changed|=4;
				delay_start_ena = 0;
			}
		}}
		//????? ???????? ????? ??????????? ???? - ?????? ?? ????
		if ((delay_start_ena)&&(millis()>set_delay*1000))
		{
			display_changed|=4;
			delay_start_ena = 0;
			arc_on_out = 1;
			//???????? ?????? ??? ??????? ???????????? - ???????? ????? ??????? ???????? ??????????? ? ??????? ??????????? ??????
			millis_reset();
		}
		//???? ???? ??????? ??? ? ????? ?????? ?????? ????????? ?????? - ?????? ?? ????
		if ((!arc_on)||(!torch_on_out)) arc_on_out = 0;
		//????? ??????? ?????? ???? ???? ????, ???????? ??????, ?????? ????????? ?????? ???? ? ???????????? ???? ??????
		if ((arc_on_out)&&(millis()>post_delay*1000)){
			if (real_voltage>set_voltage+VOLTAGE_GIST)
			{
				if ((up)||(!down)) display_changed|=64;
				up=0;
				down=1;
			}
			if (real_voltage<set_voltage-VOLTAGE_GIST)
			{
				if ((!up)||(down)) display_changed|=64;
				up=1;
				down=0;
			}
			if ((real_voltage>=set_voltage-VOLTAGE_GIST)&&(real_voltage<=set_voltage+VOLTAGE_GIST))
			{
				if ((up)||(down)) display_changed|=64;
				up=0;
				down=0;
			}
		}
		else
		{
			if ((up)||(down)) display_changed|=64;
			up=0;
			down=0;
		}
	}
	//? ?????? ????????? ????????? ???????? ???
	else
	{
		if ((torch_on_out)||(up)||(down)||(arc_on_out)) display_changed|=32;
		torch_on_out = 0;
		up = 0;
		down = 0;
		arc_on_out = 0;
	}
}

void setStates()
{
	if (torch_on_out)	SetBit(TORCH_ON_OUT_PORT, TORCH_ON_OUT);
	else 				ClearBit(TORCH_ON_OUT_PORT, TORCH_ON_OUT);
	if (!arc_on_out)			ClearBit(ARC_ON_OUT_PORT, ARC_ON_OUT);
	else 				SetBit(ARC_ON_OUT_PORT, ARC_ON_OUT);
	if (!emerg_stop)		ClearBit(E_STOP_OUT_PORT, E_STOP_OUT);
	else 				SetBit(E_STOP_OUT_PORT, E_STOP_OUT);
	if (!up)				ClearBit(UP_OUT_PORT, UP_OUT);
	else 				SetBit(UP_OUT_PORT, UP_OUT);
	if (!down)			ClearBit(DOWN_OUT_PORT, DOWN_OUT);
	else 				SetBit(DOWN_OUT_PORT, DOWN_OUT);
}

void readStates()
{
	//unsigned int new_v = (analogRead(64)/1023.0)*330;
	//new_v = -1.043*new_v+375.583;
	unsigned int new_v = analogRead(64);  
	if (new_v<403)		new_v = 673.3 - 1.07516*new_v;
	else if (new_v<830) new_v = 375.583-0.336452*new_v;
	else				new_v = 421.6 - 0.387097*new_v;

	if (new_v < 70) new_v=0;

	if (real_voltage - new_v>VOLTAGE_DIFF_TO_REFRASH)
	{
		real_voltage = new_v;
		display_changed|=16;
	}
	if (E_STOP_IN_PIN & (1 << E_STOP_IN))
	{
		if (emerg_stop) display_changed|=32;
		//{
			////???? ?? ?????? ?? ????????? ?????????? ????????? ???? ???? - ????? ??????? ???????? ? ????????? 
			//display_changed|=32;
			//if (arc_on){
				//delay_start_ena = 1;
				//delay_start_time = millis();
				//display_changed|=4;
			//}
		//}
		emerg_stop = 1;
	}
	else
	{
		if (!emerg_stop) display_changed|=32;
		emerg_stop = 0;
	}
}

int main(void)
{
	init();
	
	while(1)
	{
		wdt_reset();
		encoderProcess();
		readStates();
		logicProcess();
		setStates();
		//displayRefrash();
	}
	

}