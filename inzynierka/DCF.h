#include <avr/interrupt.h>

#define TIMER1_ON (TIMSK |= (1<<TOIE1))
#define TIMER1_OFF (TIMSK &= ~(1<<TOIE1))

#define DCF_INT INT1			//INT1
#define DCF_INT_vector INT1_vect

//ustawienia przerwania aby reagowa³ na zbocze narastaj¹ce lub opadaj¹ce
#define DCF_INT_RISING MCUCR =(1<<ISC11) | (1<<ISC10)
#define DCF_INT_FALLING MCUCR=(1<<ISC11)

#define DCF_INT_ON GIFR|=(1<<INTF1)
#define DCF_INT_OFF GIFR&=~(1<<INTF1)

// definicja sprawdzajaca czy przerwanie reaguje na zbocze narastajace czy opadajace
#define DCF_IF_RISING ((MCUCR & (1<<ISC11)) && (MCUCR & (1<<ISC10))) 
#define DCF_IF_FALLING ((MCUCR & (1<<ISC11)) && (~MCUCR & (1<<ISC10))) 

// DCF P1 : 1 - off; 0 - on  
#define DCF_P1_DIR		DDRB
#define DCF_P1_PORT 	PORTB
#define DCF_P1			(1<<PB0)

// DCF P1 odbiornik: on, off
#define DCF_ON 		DCF_P1_PORT &=~DCF_P1
#define DCF_OFF 	DCF_P1_PORT |=DCF_P1


typedef struct DCF_dt {
			uint8_t min;
			uint8_t h;
			uint8_t month;
			uint8_t day;
			uint16_t year;
} DCF_datetime;

volatile char DCF_sync;
volatile uint8_t DCF_zasieg;
volatile uint8_t DCF_bit;
volatile uint8_t DCF_dane[60];
volatile uint8_t flaga;
void DCF_Initalize();
uint8_t DCF_decode(DCF_datetime *DCF_dt);

ISR(TIMER1_OVF_vect);
ISR (INT1_vect);



