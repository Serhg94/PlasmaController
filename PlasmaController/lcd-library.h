#ifndef LCD_LIBRARY_H_
#define LCD_LIBRARY_H_
#include "conf.h"
#include <util/delay.h>
#include <avr/io.h>
#include "binary.h"
#include <stdbool.h>

/*
	Задержка строба. Эта задержка может быть увеличена, если LCD работает
	нестабильно. Но, поскольку эта задержка вставляется дважды при пересылке
	каждого байта, увеличение её может существенно замедлить выполнение
	программы.
*/


#define LCD_STROBEDELAY_US	5						// Задержка строба

/*
	Здесь можно подключить необходимые функции к библиотеке.
	Включение дополнительных функций увеличивает размер скомпилированного
	файла.
		#define USE_RAM_FUNCTIONS - раскомментируйте, если нобходимы функ-
			ции вывода из RAM.
		#define USE_FLASH_FUNCTIONS - Раскомментируйте, если необходимы
			функции вывода из FLASH-памяти.
		#define USE_EEPROM_FUNCTIONS - Раскомментируйте, если необходимы
			функции вывода из EEPROM.
		#define USE_CUSTOM_CHARACTER_FUNCTIONS - Раскомментируйте, если
			необходимы функции работы со знакогенератором.
*/
#define LCD_USE_RAM_FUNCTIONS
#define LCD_USE_FLASH_FUNCTIONS
//#define LCD_USE_EEPROM_FUNCTIONS
#define LCD_USE_CUSTOM_CHARACTER_FUNCTIONS

/*
	Дальше следуют определения констант режимов работы LCD. С этим разделом
	крайне желательно ознакомиться, но менять что-то здесь и ниже стоит
	только если вы твёрдо уверены, что знаете, что делаете.

	Первые два определения - режимы отправки данных LCD. Они применяются
	с функциями lcdRawGetByte() и lcdRawSendByte(). Так, например, вызов
	lcdRawSendByte(0x40, LCD_DATA) передаст байт 0x40, как данные.
*/

#define LCD_COMMAND			0
#define LCD_DATA			1

/*
	Эти определения используются с функцией lcdSetCursor().
*/

#define LCD_CURSOR_OFF		0
#define LCD_CURSOR_ON		2
#define LCD_CURSOR_BLINK	3

/*
	Эти определения используются с функцией lcdSetDisplay().
*/

#define LCD_DISPLAY_OFF		0
#define LCD_DISPLAY_ON		4

/*
	Эти определения используются с функцией lcdDisplayScroll().
*/

#define LCD_SCROLL_LEFT		0
#define LCD_SCROLL_RIGHT	4
/*
	Здесь описания функций, реализованных в библиотеке.
	
	Первые две функции являются функциями нижнего уровня и без их исполь-
	зования в большинстве случаев можно обойтись.
		lcdRawGetByte() - в зависимости от параметра читает либо байт
			из памяти LCD (LCD_DATA), либо состояние LCD (LCD_COMMAND).
		lcdRawSendByte() - отправляет байт в LCD, либо как данные, либо
			как команду, в зависимости от значения второго параметра. 
*/


char lcdRawGetByte(char state);
void lcdRawSendByte(char byte, char state);

/*
	Следующая функция возвращает 0xff, если LCD занят обработкой команды,
	и 0x00, если LCD готов к приёму следующей команды. Эта проверка
	автоматически выполняется во всех следующих функциях, так что исполь-
	зовать данную функцию в большинстве случаев не нужно.
*/

char lcdIsBusy(void);

/*
	Следующие функции выполняют инициализацию LCD и установку режимов
	работы.
		lcdInit() - начальная инициализация LCD. Необходимо выполнять
			перед началом работы с LCD.
		lcdSetCursor() - Устанавливает режим отображения курсора
			(LCD_CURSOR_OFF, LCD_CURSOR_ON, LCD_CURSOR_BLINK).
		lcdSetDisplay() - Включает или выключает отображение данных
			на экране LCD (LCD_DISPLAY_OFF, LCD_DISPLAY_ON).
	Внимание! Вызов lcdSetCursor() всегда выполнит включение экрана.
	Также, вызов lcdSetDisplay() всегда выключит курсор.
*/

void lcdInit(void);
void lcdSetCursor(char cursor);
void lcdSetDisplay(char state);

/*
	Эти функции выполняют задачи очистки, сдвига и позиционирования.
		lcdClear() - выполняет очистку экрана, ставит курсор на начало
			памяти LCD (первая строка, первый символ).
		lcdGotoXY() - устанавливает курсор в заданную позицию на экране.
			Строки и колонки нумеруются начиная с нуля.
			Внимание! Функция никак не отслеживает размеры экрана и
			границы видимой области!
		lcdDisplayScroll() - прокручивает видимую область экрана LCD.
			Первый параметр определяет количество знакомест, на какое нужно
			сдвинуть экран, второй - направление (LCD_SROLL_LEFT,
			LCD_SCROLL_RIGHT).
*/

void lcdClear(void);
bool lcdGotoXY(char str, char col);
void lcdDisplayScroll(char pos, char dir);

/*
	Функции вывода строк на экран.
		lcdPuts() - выводит на экран нультерминированную строку. Для отобра-
			жения пользовательских символов (lcdLoadCharacter() etc., см.
			ниже) можно	использовать esc-последовательности "\1"-"\7"
			("\0" обозначает конец строки), или "\10" - "\17" (символы дос-
			тупны и по тем и по другим адресам).
			Внимание! Функция не отслеживает границу видимой области на
			экране, поэтому о своевременном переводе строки должен заботиться
			программист.
		lcdPutsf() - аналогично lcdPuts(), но работает с FLASH-памятью.
		lcdPutse() - аналогично lcdPuts(), но работает с EEPROM.
*/
#ifdef LCD_USE_RAM_FUNCTIONS
	void lcdPuts(char *str);
#endif
#ifdef LCD_USE_FLASH_FUNCTIONS
	void lcdPutsf(char *str);
#endif
#ifdef LCD_USE_EEPROM_FUNCTIONS
	void lcdPutse(uint8_t *str);
#endif

/*
	Функции для работы со знакогенератором.
		lcdLoadCharacter() - загружает пользовательский символ в знако-
		генератор lcd по указанному адресу. Возможно одновременное ис-
		пользование до восьми символов. Адрес символа может принимать зна-
		чение от 0 до 7, он соответствует ASCII коду символа. Для вывода
		на экран символа с кодом 0, можно воспользоваться esc последо-
		вательностью \10, но записать символ в ячейку 10 нельзя. 
		lcdLoadCharacterf() - загружает символ из flash в знакогенератор
			по указанному адресу.
		lcdLoadCharactere() - загружает символ из eeprom в знакогенератор
			по указанному адресу.
*/

#ifdef LCD_USE_CUSTOM_CHARACTER_FUNCTIONS
	#ifdef LCD_USE_RAM_FUNCTIONS
		void lcdLoadCharacter(char code, char *pattern);
	#endif
	#ifdef LCD_USE_FLASH_FUNCTIONS
		void lcdLoadCharacterf(char code, char *pattern);
	#endif
	#ifdef LCD_USE_EEPROM_FUNCTIONS
		void lcdLoadCharactere(char code, char *pattern);
	#endif
#endif

#endif /* LCD-LIBRARY_H_ */