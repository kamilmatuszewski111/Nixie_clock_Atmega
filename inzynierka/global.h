/*! \file global.h \brief AVRlib project global include. */
//*****************************************************************************
//
// File Name	: 'global.h'
// Title		: AVRlib project global include 
// Author		: Pascal Stang - Copyright (C) 2001-2002
// Created		: 7/12/2001
// Revised		: 9/30/2002
// Version		: 1.1
// Target MCU	: Atmel AVR series
// Editor Tabs	: 4
//
//	Description : This include file is designed to contain items useful to all
//					code files and projects.
//
// This code is distributed under the GNU Public License
//		which can be found at http://www.gnu.org/licenses/gpl.txt
//
//*****************************************************************************

#ifndef GLOBAL_H
#define GLOBAL_H

// global AVRLIB defines
#include "avrlibdefs.h"
// global AVRLIB types definitions
#include "avrlibtypes.h"

// project/system dependent defines

// CPU clock speed
//#define F_CPU        16000000               		// 16MHz processor
//#define F_CPU        14745000               		// 14.745MHz processor
//#define F_CPU        8000000               		// 8MHz processor
//#define F_CPU        7372800               		// 7.37MHz processor
//#define F_CPU        4000000               		// 4MHz processor
//#define F_CPU        3686400               		// 3.69MHz processor
#define CYCLES_PER_US ((F_CPU+500000)/1000000) 	// cpu cycles per microsecond


#define SIG_INTERRUPT0 _VECTOR(1)
#define SIG_INTERRUPT1 _VECTOR(2)
#define SIG_INTERRUPT2 _VECTOR(3)
#define SIG_OUTPUT_COMPARE2 _VECTOR(4)
#define SIG_OVERFLOW2 _VECTOR(5)
#define SIG_INPUT_CAPTURE1 _VECTOR(6)
#define SIG_OUTPUT_COMPARE1A _VECTOR(7)
#define SIG_OUTPUT_COMPARE1B _VECTOR(8)
#define SIG_OVERFLOW1 _VECTOR(9)
#define SIG_OUTPUT_COMPARE0 _VECTOR(10)
#define SIG_OVERFLOW0 _VECTOR(11)
#define SIG_SPI _VECTOR(12)
#define SIG_UART_RECV _VECTOR(13)
#define SIG_UART_DATA _VECTOR(14)
#define SIG_UART_TRANS _VECTOR(15)
#define SIG_ADC _VECTOR(16)
/* EEPROM Ready */
//#define EE_RDY_vect            _VECTOR(17)
/* Analog Comparator */
//#define ANA_COMP_vect            _VECTOR(18)
#define SIG_2WIRE_SERIAL _VECTOR(19) //
/* Store Program Memory Ready */
//#define SPM_RDY_vect            _VECTOR(20)


#endif
