
#include "DCF.h"

void DCF_Initalize()
{
	
	//konfiguracja kierunku portu s³u¿¹cego do w³¹czenia lub wy³aczenia odbiornika DCF
	DCF_P1_DIR |= DCF_P1;



  //konfiguracja TIMER1
	TCCR1B = (1<<CS11)|(1<<CS10); //preskaler 256 TIMER1
  TCNT1 = 0;
  TIFR |= (1<<TOV1);
 
 
  
	// konfiguracja przerwania od DCF77
  GICR|=(1<<DCF_INT);
  DCF_INT_RISING; //zbocze narastajace -szukanie bitu synchronizacji
  DCF_INT_ON;
  
	// ustawienie pocz¹tkowcyh wartoœci zmeinnych

	DCF_sync=0;
	DCF_bit=0;
	DCF_zasieg=1;
	
  sei(); // w³aczenie przerwan

	//w³aczenie Timera1
	TIMER1_ON;
}


//obs³uga przrwania od przepe³nienia licznika timera 1
ISR(TIMER1_OVF_vect)
{
	if (DCF_zasieg>1)
		DCF_zasieg--; //zmniejsz zasieg
	flaga=1;
	
}
//przerwanie od zmainy sygna³u DCF77
ISR (DCF_INT_vector)
{
	flaga=2;
	/*
		8Mhz =1/8000000
		Timer1 Preskaler 256
		1/8000000 * 256 = 0,000032s  - co tyle czasu nastepuje zwiekszenie licznika TCNT1
		100ms - oznacza 0 
			czyli sygna³ z przedzialu (50ms -- 150ms)
				0.05 / 	0,000032 ~ 1562
				0.15 /  0,000032 ~ 4687
		200ms - oznacza	1	
			czyli sygna³ z przedzialu (150ms -- 250ms)
				0.15 /  0,000032 ~ 4688
				0.25 /  0,000032 ~ 7812
		1,5s dp 2s - synchronizacja
				1.5 /  0,000032 ~ 46875
				2   /  0,000032 ~ 62500
    stan 0 powinien byc w czasie: pomiedzy 750ms (23437) a 950ms (29687)
	*/
	if (DCF_IF_RISING) //jezeli reakcja na zbocze narastajace oancza ¿e poprzednim poziom logiczny to 0
	{
		//	szukaj bitu synchronizacji czas od 1,5s do 2s
		if ((TCNT1 > 25000) && (TCNT1 < 33333))
		{
			DCF_sync=1; // ustaw bit synchronizacji
			DCF_bit=0;	// kolejny bit zapisany do tablicy o indexie 0
		}
		else if (!((TCNT1 > 12495) && (TCNT1 < 15827))) // jezeli sygnal 0 nie miesci sie w normach (750ms - 950ms) to zminiejsz zasieg
		{
			if (DCF_zasieg>1)
			DCF_zasieg--; // zmneijsz zasieg
		}
		TCNT1=0; // wyzereuj licznik Timera 1
		DCF_INT_FALLING; // ustawienie reakcji przerwania na zbocze opadajace
	}
	else
	{
		if ((TCNT1 > 833) && (TCNT1 < 2500)) // sprawdŸ czy czas mieœci siê w przedziale 50ms - 150ms
		{
			// pobranie bitu o wartoœci 0
			if (DCF_zasieg<4)
			DCF_zasieg++; //zwiesz zasieg

			if (DCF_sync==1)// jezeli odnaleziono bit synchronizacji to pobierz bit
			{
				DCF_dane[DCF_bit]=0; // zapisz bit
				DCF_bit++; // zwieksz index
			}
		}
		else if ((TCNT1 > 2500) && (TCNT1 < 4167)) // jêzeli czas 150ms - 200ms to oznacza bit 1
		{
			if (DCF_zasieg<4)
			DCF_zasieg++; // zwieksz zasieg
			
			if (DCF_sync==1)// jezeli odnaleziono bit synchronizacji to pobierz bit
			{
				DCF_dane[DCF_bit]=1;// zapisz bit do tablicy o okreslonym indexie DCF_bit
				DCF_bit++; //zwiêksz index
			}
		}
		else // jezeli inny czas to b³ad sygna³u
		{
			//usun flage synchronizacji
			DCF_sync=0;
			if (DCF_zasieg>1)
			DCF_zasieg--; // zmneijsz zasieg

		}
		TCNT1=0; // wyzeruj licznik Timera 1
		DCF_INT_RISING; // ustaw reakcje przerwania na zbocze narastaj¹ce
	}



}


uint8_t DCF_decode(DCF_datetime *DCF_dt)
{

	// potrzebne jest 59 bitów aby zdekodowac wszystkei dane
	if (DCF_bit<59)
		return 0;

  //bit 0 - zawsze 0 oraz bit 20 - zawsze 1			
	if ((DCF_dane[0]!=0)||(DCF_dane[20]!=1)) //jezeli bity niepoprawne to zakoncz
		return 0;

	//bity 21 - 27 - minuty bit 28 - bit parzystosci
	uint8_t DCF_parzystosc_m=0; 
	for (int a=21; a<29; a++) //sumowanie bitow
		DCF_parzystosc_m+=DCF_dane[a];

  if ((DCF_parzystosc_m%2)!=0) // jezeli suma bitow jest nieparzysta to zakoncz
		return 0;

	//bity 29 - 34 - godziny 35 - bit parzystosci 
	uint8_t DCF_parzystosc_h=0;
	for (int a=29; a<36; a++) 
		DCF_parzystosc_h+=DCF_dane[a];	

 	if ((DCF_parzystosc_h%2)!=0) // jezeli suma bitow jest nieparzysta to zakoncz
		return 0;

	//bity 36 - 57 - data; 58 - bit parzystosci 
	uint8_t DCF_parzystosc_data=0;
	for (int a=36; a<59; a++) 
			DCF_parzystosc_data+=DCF_dane[a];	

	if ((DCF_parzystosc_data%2)!=0) // jezeli suma bitow jest nieparzysta to zakoncz
		return 0;
			
  // jezeli bity parzystosci zosta³y sprawdzone mozna dekodowaæ sygna³
	
	//dekodowanie minut 
	DCF_dt->min	= DCF_dane[21]
								+ 2*DCF_dane[22]
								+ 4*DCF_dane[23]
								+ 8*DCF_dane[24]
								+ 10*DCF_dane[25]
								+ 20*DCF_dane[26]
								+ 40*DCF_dane[27];
	
	//dekodowanie godzin
  DCF_dt->h = DCF_dane[29]
								+ 2*DCF_dane[30]
								+ 4*DCF_dane[31]
								+ 8*DCF_dane[32]
								+ 10*DCF_dane[33]
								+ 20*DCF_dane[34];

	//dekodowanie dnia miesiaca
	DCF_dt->day	= DCF_dane[36]
								+ 2*DCF_dane[37]
							  + 4*DCF_dane[38]
								+ 8*DCF_dane[39]
								+ 10*DCF_dane[40]
								+ 20*DCF_dane[41];

	//dekodowanie miesiaca
	DCF_dt->month	= DCF_dane[45]
								+ 2*DCF_dane[46]
								+ 4*DCF_dane[47]
								+ 8*DCF_dane[48]
								+ 10*DCF_dane[49];

	//dekodowanie roku
	DCF_dt->year = DCF_dane[50] 
								+ 2 * DCF_dane[51]
                + 4 * DCF_dane[52]
                + 8 * DCF_dane[53]
                + 10 * DCF_dane[54]
                + 20 * DCF_dane[55]
                + 40 * DCF_dane[56]
                + 80 * DCF_dane[57]
								+ 2000;

  // sprawdzenie czy odczytane dane sa wiarygodne 
	//godzina nie wieksza niz 24 minuty nei wieksze niz 60, miesiac nei wiekszy niz 12, dzien nie wiekszy niz 31
  if ((DCF_dt->h>24)||(DCF_dt->min>60)||(DCF_dt->month>12)||(DCF_dt->day>31))
		return 0;
		      
	// zakonczenie sukcesem  
	return 1;
} 
