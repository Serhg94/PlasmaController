/*
 * custom_symbols.h
 *
 * Created: 06.11.2016 16:51:54
 *  Author: Serhg
 */ 


#ifndef CUSTOM_SYMBOLS_H_
#define CUSTOM_SYMBOLS_H_

#define CHAR_THUNDER_CODE 10
const char char_thunder[] PROGMEM = {
	0b00000001,
	0b00000110,
	0b00001100,
	0b00011000,
	0b00001110,
	0b00000011,
	0b00000110,
	0b00001000
};
#define UP_ARROW_CODE 11
const char up_arrow[] PROGMEM = {
	0b00000100,
	0b00001110,
	0b00010101,
	0b00000100,
	0b00000100,
	0b00000100,
	0b00000100,
	0b00000100
};
#define DOWN_ARROW_CODE 12
const char down_arrow[] PROGMEM = {
	0b00000100,
	0b00000100,
	0b00000100,
	0b00000100,
	0b00000100,
	0b00010101,
	0b00001110,
	0b00000100
};
#define TORCH_CHAR_CODE 13
const char torch_char[] PROGMEM = {
	0b00010001,
	0b00010001,
	0b00010001,
	0b00011111,
	0b00000100,
	0b00000100,
	0b00000100,
	0b00000100
};
#define LOCK_CHAR_CODE 14
const char lock_char[] PROGMEM = {
	0b00001110,
	0b00010001,
	0b00010001,
	0b00010001,
	0b00011111,
	0b00011001,
	0b00011011,
	0b00011111
};
#define E_CHAR_CODE 15
const char e_char[] PROGMEM = {
	0b00000110,
	0b00001000,
	0b00010000,
	0b00010000,
	0b00011111,
	0b00010000,
	0b00001000,
	0b00000111
};

#endif /* CUSTOM_SYMBOLS_H_ */