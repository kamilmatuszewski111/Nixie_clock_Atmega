//***************** (c) 2008 BTC Korporacja ***********************************
//                     http://www kamami.com
//
//    THE SOFTWARE INCLUDED IN THIS FILE IS FOR GUIDANCE ONLY.
//    BTC KORPORACJA SHALL NOT BE HELD LIABLE FOR ANY DIRECT, INDIRECT
//    OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
//    FROM USE OF THIS SOFTWARE.
//
//******************************************************************************
#include <avr/io.h>
#include <util/delay.h>
//-------------------------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------------------------
#define LCD_RS_DIR		DDRA //DDRB
#define LCD_RS_PORT 	PORTA //PORTB
#define LCD_RS			(1 << PA7) //(1 << PB0)

#define LCD_E_DIR		DDRA //DDRB
#define LCD_E_PORT		PORTA //PORTB
#define LCD_E			(1 << PA6) //(1 << PB2)

#define LCD_DB4_DIR		DDRA //DDRD
#define LCD_DB4_PORT	PORTA //PORTD
#define LCD_DB4			(1 << PA2) //(1 << PD4)

#define LCD_DB5_DIR		DDRA //DDRD
#define LCD_DB5_PORT	PORTA //PORTD
#define LCD_DB5			(1 << PA3) //(1 << PD5)

#define LCD_DB6_DIR		DDRA //DDRD
#define LCD_DB6_PORT	PORTA //PORTD
#define LCD_DB6			(1 << PA4) //(1 << PD6)

#define LCD_DB7_DIR		DDRA //DDRD
#define LCD_DB7_PORT	PORTA //PORTD
#define LCD_DB7			(1 << PA5) //(1 << PD7)

//-------------------------------------------------------------------------------------------------
// HD44780 commands
//-------------------------------------------------------------------------------------------------

#define HD44780_CLEAR					0x01

#define HD44780_HOME					0x02

#define HD44780_ENTRY_MODE				0x04
	#define HD44780_EM_SHIFT_CURSOR		0
	#define HD44780_EM_SHIFT_DISPLAY	1
	#define HD44780_EM_DECREMENT		0
	#define HD44780_EM_INCREMENT		2

#define HD44780_DISPLAY_ONOFF			0x08
	#define HD44780_DISPLAY_OFF			0
	#define HD44780_DISPLAY_ON			4
	#define HD44780_CURSOR_OFF			0
	#define HD44780_CURSOR_ON			2
	#define HD44780_CURSOR_NOBLINK		0
	#define HD44780_CURSOR_BLINK		1

#define HD44780_DISPLAY_CURSOR_SHIFT	0x10
	#define HD44780_SHIFT_CURSOR		0
	#define HD44780_SHIFT_DISPLAY		8
	#define HD44780_SHIFT_LEFT			0
	#define HD44780_SHIFT_RIGHT			4

#define HD44780_FUNCTION_SET			0x20
	#define HD44780_FONT5x7				0
	#define HD44780_FONT5x10			4
	#define HD44780_ONE_LINE			0
	#define HD44780_TWO_LINE			8
	#define HD44780_4_BIT				0
	#define HD44780_8_BIT				16

#define HD44780_CGRAM_SET				0x40

#define HD44780_DDRAM_SET				0x80

//-------------------------------------------------------------------------------------------------
// Functions declarations
//-------------------------------------------------------------------------------------------------
void LCD_WriteCommand(unsigned char);
void LCD_WriteData(unsigned char);
void LCD_WriteText(char *);
void LCD_GoTo(unsigned char, unsigned char);
void LCD_Clear(void);
void LCD_Home(void);
void LCD_ShiftLeft(void);
void LCD_ShiftRight(void);
void LCD_Initialize(void);
//-------------------------------------------------------------------------------------------------
// End of file
//-------------------------------------------------------------------------------------------------
