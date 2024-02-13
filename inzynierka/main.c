#define F_CPU 1000000UL
#include "global.h"
#include <avr/io.h>
#include <stdio.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "PCF8563.h"
//#include "time.h"
#include "DCF.h"


DCF_datetime DCF_dekode;
PCF_DateTime pcfDateTime;
//DCF_datetime DCF_dekode;

uint8_t h,m,s = 0;
uint8_t godzina, minuty, sekundy = 0;
volatile int temp_value=0;
uint8_t wysw=0;

void digits(int digit);
void clear_ports();
int main(void)
{
	DDRB = (1<<PB7)|(1<<PB6)|(1<<PB5)|(1<<PB4)|(1<<PB3)|(1<<PB2);
	DDRC &= (0<<PC0)|(1<<PC6)|(1<<PC7)|~(1 << PC5);
	//DDRC = 0xFF;
	DDRD &=~ (1<<PD3);
	DDRA = 0xFF;
	PORTC |= (1<<PC5);
	
	PCF_Init(1);
	DCF_Initalize();
	PCF_SetTimer(PCF_TIMER_DISABLED, 0);

	PCF_DateTime dateTime;
	dateTime.second = 0;
	dateTime.minute = 23;
	dateTime.hour = 20;
	dateTime.day = 29;
	dateTime.weekday = 7;
	dateTime.month = 10;
	dateTime.year = 2023;
	PCF_SetDateTime(&dateTime);
	DCF_ON;
	while(1)
	{
		if(DCF_sync == 1)
		{
			if (DCF_decode(&DCF_dekode)!=0)
			{
				m=DCF_dekode.min;
				h=DCF_dekode.h;
				
				DCF_OFF;
				DCF_sync=0;
				//PCF_DateTime dataTime;
				dateTime.second = 0;
				dateTime.minute = m;
				dateTime.hour = h;
				PCF_SetDateTime(&pcfDateTime);
			}
		}
		PCF_GetDateTime(&pcfDateTime);
		godzina = pcfDateTime.hour;
		minuty = pcfDateTime.minute;
		sekundy = pcfDateTime.second;
		
		if (godzina == 1) DCF_ON;
		
		
		// Wy?wietlanie jedno?ci sekund:
		temp_value = sekundy %10;

		PORTB = (1<<PB2);
		PORTA = 0x00;

		digits(temp_value);
		clear_ports();
		// Wy?wietlanie dziesi?tek sekund:
		sekundy = sekundy/10;
		temp_value = sekundy%10;
		
		PORTA = 0x00;
		PORTB = (1<<PB3);

		digits(temp_value);
		clear_ports();
		// Wy?wietlanie jedno?ci minut:
		temp_value = minuty%10;
		
		PORTA = 0x00;
		PORTB = (1<<PB4);

		digits(temp_value);
		clear_ports();
		// Wy?wietlanie dziesi?tek minut:
		minuty = minuty/10;
		temp_value = minuty%10;

		PORTA = 0x00;
		PORTB = (1<<PB5);

		digits(temp_value);
		clear_ports();
		// Wy?wietlanie jedno?ci godzin:
		temp_value = godzina%10;

		PORTA = 0x00;
		PORTB = (1<<PB6);

		digits(temp_value);
		clear_ports();
		// Wy?wietlanie dziesi?tek godzin:
		godzina = godzina/10;
		temp_value = godzina%10;

		PORTA = 0x00;
		PORTB = (1<<PB7);

		digits(temp_value);
		clear_ports();
		
		/*
		sprintf(buff, "Czas:%d:%d:%d",godzina,minuty,sekundy);
		LCD_Clear();
		LCD_GoTo(0,0);
		LCD_WriteText(buff);
		
		sprintf(buff2,"%d:%d  %d %d %d",h,m,DCF_sync,DCF_bit,flaga);
		LCD_GoTo(0,1);
		LCD_WriteText(buff2);
		_delay_ms(10);
		
			if (! (PINC & (0<<PC0)))
		{
		if (sekundy < 10) { dziesiatki = 0; jednosci = sekundy;}
		else { jednosci = sekundy%10; sekundy = sekundy/10; dziesiatki = sekundy%10;}
		
		if (minuty < 10) { tysiace = 0; setki = minuty;}
		else { setki = minuty%10; minuty = minuty/10; tysiace = minuty%10;}
		}
		else
		{
		if (minuty < 10) { dziesiatki = 0; jednosci = minuty;}
		else { jednosci = minuty%10; minuty = minuty/10; dziesiatki = minuty%10;}
		
		if (godzina < 10) { tysiace = 0; setki = godzina;}
		else { setki = godzina%10; godzina = godzina/10; tysiace = godzina%10;}
		}
		*/
		//_delay_ms(10);
		/*
		jednosci = sekundy % 10;
		sekundy = sekundy/10;
		dziesiatki = sekundy%10;
		sekundy = sekundy/10;
		setki = sekundy%10;
		sekundy = sekundy/10;
		tysiace = sekundy%10;
		
		*/
		
		/*
		PORTB = ~wyswietl[jednosci];
		PORTD = ~128;
		_delay_us(1500);
		
		PORTB = ~wyswietl[dziesiatki];
		PORTD = ~64;
		_delay_us(1500);
		
		PORTB = ~wyswietl2[setki];
		PORTD = ~32;
		_delay_us(1500);
		
		PORTB = ~wyswietl[tysiace];
		PORTD = ~16;
		//_delay_us(500);
		*/
		/*			switch(wysw)
		{
		case 0:
		{
		if( tysiace != 0)
		{
		PORTD = ~16;
		PORTB = ~wyswietl[tysiace];
		}
		wysw++;
		break;
		}
		case 1:
		{			if(setki != 0 || (setki == 0 && tysiace != 0))
		{
		PORTD = ~32;
		PORTB = ~wyswietl[setki];
		}
		wysw++;
		break;
		}
		case 2:
		{	if(dziesiatki != 0 || (dziesiatki == 0 && (setki != 0 || tysiace !=0)))
		{
		PORTD = ~64;
		PORTB = ~wyswietl[dziesiatki];
		}
		wysw++;
		break;
		}
		case 3:
		{
		
		PORTD = ~128;
		PORTB = ~wyswietl[jednosci];
		
		wysw=0;
		break;
		}
		

		
		}
		
		*/
		}

		}

void digits(int digit){
	switch(digit)
	{
		case 0:
		{
		PORTC = (1<<PC6);
		break;
		}
		case 1:
		{
		PORTC = (1<<PC7);
		break;
		}
		case 2:
		{
		PORTA = (1<<PA7);
		break;
		}
		case 3:
		{
		PORTA = (1<<PA6);
		break;
		}
		case 4:
		{
		PORTA = (1<<PA5);
		break;
		}
		case 5:
		{
		PORTA = (1<<PA4);
		break;
		}
		case 6:
		{
		PORTA = (1<<PA3);
		break;
		}
		case 7:
		{
		PORTA = (1<<PA2);
		break;
		}
		case 8:
		{
		PORTA = (1<<PA1);
		break;
		}
		case 9:
		{
		PORTA = (1<<PA0);
		break;
		}
	}
}
void clear_ports(){
	_delay_ms(1);
	PORTA = 0x00;
	PORTC = 0x00;
	PORTB = 0x00;
}

