/*
    Title:    AVR-GCC test program for the STK200 eva board
    Author:   Volker Oth
    Date:     5/1999
    Purpose:  Decoding of the DCF77 real time signal.
    needed
    Software: AVR-GCC to compile, AVA to assemble and link
    needed
    Hardware: ATS90S8515 on STK200 board
    Note:     To contact me, mail to
                  volkeroth(at)gmx.de
*/

#include <io.h>
#include <interrupt.h>
#include <signal.h>
#include "global.h"
#include "timer.h"
#include "dcf77.h"
#include "uart.h"
#include "led.h"


void falling_edge(void)
/* callback routine called at falling edge of dcf77 signal */
{
    led_off(0);                            /* LED 0 off (signal) */
}

void rising_edge(void)
/* callback routine called at rising edge of dcf77 signal */
{
    led_on(0);                             /* LED 0 off (signal) */

    if (dcf77_status & (1<<DCF77_STATUS_INIT))
        led_on(1);                         /* LED 1 on (startbit) */
    else
        led_off(1);                        /* LED 1 off (startbit) */

    if (dcf77_status & (1<<DCF77_STATUS_BIT))
        led_on(2);                         /* LED 2 on (bit) */
    else
        led_off(2);                        /* LED 2 off (bit) */        

    /* send data to rs232 */
    uart_send(dcf77_data, DCF77_BUFFER_SIZE);
}


int main(void)
{
    /* init */
    led_init();
    dcf77_init();
    uart_init();
    timer1_init();

    /* set dcf77 callbacks */
    dcf77_call_rise = rising_edge;
    dcf77_call_fall = falling_edge;

    sei();                                 /* enable interrupts */
    for (;;) {}                            /* loop forever */
}