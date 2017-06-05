/*
 * IncFile1.h
 *
 * Created: 01.11.2016 0:27:42
 *  Author: Serhg
 */ 


#ifndef INCFILE1_H_
#define INCFILE1_H_


#define F_CPU 16000000UL

#define MAX_VOLTAGE 340
#define MIN_VOLTAGE 0
#define MAX_DELAY 5
#define MIN_DELAY 0
#define VOLTAGE_GIST 2
#define VOLTAGE_DIFF_TO_REFRASH 2
#define MIN_ARC_BY_ADC 80
#define MAX_ARC_BY_ADC 200

#define DELAY_SHOW_TIME 2000

#define TORCH_ON_OUT_DDR DDRB
#define TORCH_ON_OUT_PORT PORTB
#define TORCH_ON_OUT 5
#define ARC_ON_OUT_DDR DDRB
#define ARC_ON_OUT_PORT PORTB
#define ARC_ON_OUT 4
#define DOWN_OUT_DDR DDRD
#define DOWN_OUT_PORT PORTD
#define DOWN_OUT 0
#define E_STOP_OUT_DDR DDRD
#define E_STOP_OUT_PORT PORTD
#define E_STOP_OUT 1
#define UP_OUT_DDR DDRB
#define UP_OUT_PORT PORTB
#define UP_OUT 3

#define E_STOP_IN_DDR DDRC
#define E_STOP_IN_PORT PORTC
#define E_STOP_IN_PIN PINC
#define E_STOP_IN 1

//порт и выводы к которым подключен энкодер установки напряжения
#define PORT_Enc	PORTD
#define PIN_Enc 	PIND
#define DDR_Enc 	DDRD
#define Pin1_Enc 	6
#define Pin2_Enc 	7
//порт и выводы к которым подключен энкодер установки задержки
#define PORT_EncT 	PORTD
#define PIN_EncT 	PIND
#define DDR_EncT 	DDRD
#define Pin1_EncT 	4
#define Pin2_EncT 	5

/*
	Здесь определяются выводы контроллера, подключенные к LCD. Выводы данных
	должны занимать один порт. Порядок выводов любой. Также на одном порту
	должны располагаться управляющие выводы. Тоже в любом порядке, не обяза-
	тельно подряд.
*/ 

#define LCDDATAPORT			PORTC					// Порт и пины,
#define LCDDATADDR			DDRC					// к которым подключены
#define LCDDATAPIN			PINC					// сигналы D4-D7.
#define LCD_D4				2
#define LCD_D5				3
#define LCD_D6				4
#define LCD_D7				5

#define LCDCONTROLPORT		PORTB					// Порт и пины,
#define LCDCONTROLDDR		DDRB					// к которым подключены
#define LCD_RS				0						// сигналы RS, RW и E.
#define LCD_RW				1
#define LCD_E				2




#endif /* INCFILE1_H_ */