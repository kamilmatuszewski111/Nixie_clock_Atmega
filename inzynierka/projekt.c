#define F_CPU 8000000UL
#define F_TWI   10000UL
#include <avr/io.h>
#include <avr/delay.h>  
#include <avr/interrupt.h>
#include "lib/HD44780.h"
#include "lib/DCF.h"
#include "lib/pcf8583.h"

//definicja klawiaturki

#define KEY_PIN   PINB
#define KEY_DDR   DDRB
#define KEY_PORT  PORTB
#define KEY_UP		PB5
#define KEY_DOWN  PB4
#define KEY_OK 		PB3


// zdefiniowanie 4 znaków zasiegu
char zasieg[4][8]={{0,0,0,0,0,0,0,0},
									 {0,0,0,0,0,0,0x1F,0},
									 {0,0,0,0,0x07,0x0F,0x1F,0},
									 {0,0,0x01,0x03,0x07,0x0F,0x1F,0}};


	//zmienna przetrzymuj¹ca odkodowane dane
 DCF_datetime DCF_dekode;
 // zmienna z której dane zapisywane s¹ do RTC
 pcf8583_ctl data_save; 
 //zmienna do której odczytywane s¹ dane z RTC
 pcf8583_ctl data_read; 



int main()
{

	//zmienna przechowuj¹ca aktualnie pobrany czas
	pcf8583_ctl data; 
	
	//inicjalizacji magistrali i2c do komunikacji z RTC
	twi_init();

		//konfiguracja przycisków
	KEY_DDR&=~(_BV(KEY_UP)|_BV(KEY_OK)|_BV(KEY_DOWN));
	// wewnetrzny PULL UP do przycisków
	KEY_PORT|=_BV(KEY_UP)|_BV(KEY_OK)|_BV(KEY_DOWN);

	//pin PD3(INT1) jako wejœcie
	DDRD&=~_BV(PD3); 

	//inicjalizacja komponentów bilbioteki DCF77
	DCF_Initalize();
	
	//inicjalizuj wyœwietlacz
	LCD_Initalize(); 

  // za³aduj do pamieci wyœwietlacza 4 stopnie zasiêgu
	LCD_Def_Char(zasieg[0], 1);
	LCD_Def_Char(zasieg[1], 2);
	LCD_Def_Char(zasieg[2], 3);
	LCD_Def_Char(zasieg[3], 4);

	// bufor na tekst 
	char buff[20];

	//zmienna opisuj¹ca aktualne menu (0..3)
	uint8_t menu=0;
	//wejscie w podmenu dla wybranego menu
	uint8_t podmenu=0;

	//zmienne s³u¿aca do opóznienia migania aktywnej pozycji w ustawieniu rêcznym
	uint16_t licznik=0;	
	uint8_t show=0;

	//g³ówna pêtla
	while(1)
	{
	
	  // sprawdŸ czy przycisk wcisniety
		if (bit_is_clear(KEY_PIN, KEY_UP))
		{
			if (podmenu==0)  // jezeli podmenu = 0 to znaczy ze aktywne jest menu g³ówne
			{
				if (menu<3)	 // 4 pozycje menu 0..3
					menu++;
				_delay_ms(150); // jezeli wcisnieto przycisk to czekaj 150ms aby pozbyæ sie drgania styków
			}
			if (menu==3) // ustawianie czasi i daty
			{
					switch (podmenu) // zwiekszanie poszczególnych wartosci
							{
								case 1:if (data.hr<23) data.hr++; break; // godziny max 23
								case 2:if (data.min<59) data.min++; break; // minuty max 59
								case 3:if (data.sec<59) data.sec++; break; //sekundy max 59
								case 4:if (data.days<31) data.days++; break; // dzien max 31
								case 5:if (data.month<12) data.month++; break; // miesiac max 12
								case 6:if ((data.year/100)<99) data.year+=100; break; // czesc roku liczba setek max 99
								case 7:if ((data.year/100)<99) data.year++; break; //czesc roku dziesiateki jednosci
								case 8:LCD_Clear(); podmenu=0;
										_delay_ms(1000);
								 break; //przejscie do potwierzenia zapisu godziny
							}
				_delay_ms(100);
				show=1;
			}
		} // sprawdŸ czy wcisniety przycisk
		else if (bit_is_clear(KEY_PIN, KEY_OK))
		{
			if (menu == 3) // ustaw godzine
			{
				if (data.year<2012) //jezeli rok mniejszy nix 2012 to ustaw na 2012
					data.year=2012;
				// wyswietl godzine i date
				LCD_GoTo(0,0);	
				sprintf(buff, "    %02d:%02d:%02d",data.hr, data.min, data.sec);
				LCD_WriteText(buff);	
				LCD_GoTo(0,1);
				sprintf(buff, "data:%02d-%02d-%04d", data.days, data.month, data.year);
				LCD_WriteText(buff);

				if (podmenu==8) //jezeli na zapytanie czy zapisac nacisniety zosta OK to zapisz
					{
						write_pcf8583(&data);	
						_delay_ms(1000);
						podmenu=0;
						menu=0;
						LCD_Clear(); 
					}
				else
				{
					podmenu++;
					if (podmenu == 8) // pytanie czy zapisac zmiany
						LCD_Clear(); 
				}
				
			}
			else
			{
				if (podmenu==0) 
				{
					DCF_bit=0;
					podmenu=1; 
				}
				else if (podmenu!=0) 
				{
					podmenu=0;
				}
			
				LCD_Clear();  // wyczysc wyœwietlacz
			}
			_delay_ms(150);
		}
	  else if (bit_is_clear(KEY_PIN, KEY_DOWN))
		{
			if (podmenu==0)
			{ 
				if (menu>0)	
					menu--;
				_delay_ms(150);
			}
			if (menu==3) // ustawianie czasi i daty
			{
					switch (podmenu) // zwiekszanie poszczególnych wartosci
							{
								case 1:if (data.hr>0) data.hr--; break; // godziny min 0
								case 2:if (data.min>0) data.min--; break; // minuty min 0
								case 3:if (data.sec>0) data.sec--; break; //sekundy min 0
								case 4:if (data.days>1) data.days--; break; // dzien min 1
								case 5:if (data.month>1) data.month--; break; // miesiac min 1
								case 6:if (data.year>=2100) data.year-=100; break; // czesc roku liczba setek min 20
								case 7:if (data.year>2012) data.year--; break; //czesc roku dziesiateki jednosci min 2012
								case 8:LCD_Clear(); podmenu=0;
										_delay_ms(1000);
								 break; //przejscie do potwierzenia zapisu godziny
							}
				_delay_ms(100);
				show=1;
			}

		}


	switch(menu)
		{
			case 0: 
						DCF_OFF;
						read_pcf8583(&data);
						LCD_GoTo(0,0);	
						sprintf(buff, "    %02d:%02d:%02d",data.hr, data.min, data.sec);
						LCD_WriteText(buff);	
						LCD_GoTo(0,1);
						sprintf(buff, "data:%02d-%02d-%04d", data.days, data.month, data.year);
						LCD_WriteText(buff);	
			break;
			case 1: 
						
						if (podmenu==0)
						{
							read_pcf8583(&data);
							LCD_GoTo(0,0);	
							sprintf(buff, "    %02d:%02d:%02d",data.hr, data.min, data.sec);
							LCD_WriteText(buff);	
							LCD_GoTo(0,1);                 
							LCD_WriteText(" Ustaw z DCF77  ");	
						}
						else
						{
							DCF_ON;
							// zasieg
									LCD_GoTo(15,0);
									sprintf(buff, "%c", DCF_zasieg);
									LCD_WriteText(buff);
									LCD_GoTo(15,1);
									if (bit_is_set(PIND, PD3))
										LCD_WriteText("-");
									else
										LCD_WriteText("_");
							// zasieg
							if (DCF_sync==0)
							{
								LCD_GoTo(0,0);	
								LCD_WriteText("Szukam sync... ");	
							}
							else
							{
								LCD_GoTo(0,0);
								sprintf(buff, "Pobrano %02dbit  ", DCF_bit);	
								LCD_WriteText(buff);			
		
							}
								if (DCF_decode(&DCF_dekode)!=0)
								{

									data_save.h_sec=0;
									data_save.sec=0;
									data_save.min=DCF_dekode.min;
									data_save.hr=DCF_dekode.h;
									data_save.days=DCF_dekode.day;
									data_save.month=DCF_dekode.month;
									data_save.year=DCF_dekode.year;
									
									write_pcf8583(&data_save); 

									LCD_GoTo(2,1);
									sprintf(buff, "%02d:%02d:%02d ", DCF_dekode.h, DCF_dekode.min, 0);
									LCD_WriteText(buff);

									_delay_ms(4000);
									LCD_Clear();
									podmenu=0;
									menu=0;



								}

						}
							
			break;
			case 2:  
						DCF_ON;
							// porównywanie czasu RTC z DCF
							if (podmenu==0)
							{
								read_pcf8583(&data);
								LCD_GoTo(0,0);	
								sprintf(buff, "    %02d:%02d:%02d",data.hr, data.min, data.sec);
								LCD_WriteText(buff);	
								LCD_GoTo(0,1);                 
								LCD_WriteText("Porownaj z DCF77");	
							}
							else if (podmenu==1)
							{
									// zasieg
									LCD_GoTo(15,0);
									sprintf(buff, "%c", DCF_zasieg);
									LCD_WriteText(buff);
									LCD_GoTo(15,1);
									if (bit_is_set(PIND, PD3))
										LCD_WriteText("-");
									else
										LCD_WriteText("_");
								// zasieg
								if (DCF_sync==0)
								{
									LCD_GoTo(0,0);	
									LCD_WriteText("Szukam sync... ");	
								}
								else
								{
									LCD_GoTo(0,0);
									sprintf(buff, "Pobrano %02dbit  ", DCF_bit);	
									LCD_WriteText(buff);			
								}
								if (DCF_decode(&DCF_dekode)!=0)
								{
									read_pcf8583(&data_read);		// pobranie czasu z RTC
									DCF_bit=0;							
									LCD_GoTo(0,1);	
									sprintf(buff, "r: %02d:%02d:%02d.%02d ",data_read.hr, data_read.min, data_read.sec, data_read.h_sec);
									LCD_WriteText(buff);

									
									LCD_GoTo(0,0);	
									sprintf(buff, "d: %02d:%02d:%02d.%02d ",DCF_dekode.h, DCF_dekode.min, 0, 0);
									LCD_WriteText(buff);	

									_delay_ms(20000);
									LCD_Clear();
								}
								
							
							}

			break;
			case 3: 
						DCF_OFF;
						if (podmenu==0)
						{
							read_pcf8583(&data);
							LCD_GoTo(0,0);	
							sprintf(buff, "    %02d:%02d:%02d",data.hr, data.min, data.sec);
							LCD_WriteText(buff);	
							LCD_GoTo(0,1);                 
							LCD_WriteText("Ustaw recznie.. ");	
				
						}
						else if(podmenu==8) // komunikat czy zapisac
						{
							LCD_GoTo(0,0);				
							LCD_WriteText("OK- zapisz");											
							LCD_GoTo(0,1);	
							LCD_WriteText("GORA/DOL- anuluj");											
						}
						else //ustawienie czasu i daty
						{

							// s³uzy do migania aktualnej pozycji
							licznik++;
							if (licznik>0x6FF) // opoxnienie
							{
								licznik=0;
								if (show) show=0;	else  show=1;
							}

							switch (podmenu)
							{
								case 1:LCD_GoTo(4,0); break;
								case 2:LCD_GoTo(7,0); break;
								case 3:LCD_GoTo(10,0); break;
								case 4:LCD_GoTo(5,1); break;
								case 5:LCD_GoTo(8,1); break;
								case 6:LCD_GoTo(11,1); break;
								case 7:LCD_GoTo(11,1); break;
							}

							
							if (show) // pokaza ustawiana wartosc
							{
								switch (podmenu)
								{
									case 1:sprintf(buff, "%02d", data.hr); break;
									case 2:sprintf(buff, "%02d", data.min); break;
									case 3:sprintf(buff, "%02d", data.sec); break;
									case 4:sprintf(buff, "%02d", data.days); break;
									case 5:sprintf(buff, "%02d", data.month); break;
									case 6:sprintf(buff, "%04d", data.year); break;
									case 7:sprintf(buff, "%04d", data.year); break;
								}
							}
							else // zamaz wartosc
							{
								if (podmenu==7) //gdy wartosc dziesiatek i jednostek lat
									LCD_GoTo(13,1);
								sprintf(buff, "  ");
							}
														
							LCD_WriteText(buff);

						}

			break;
		
		}
		
				
}

}
	





