//**************************************************************
// ****** FUNCTIONS FOR SPI COMMUNICATION *******
//**************************************************************
//Controller: ATmega32M1 (Clock: 16 Mhz-external)
//Compiler	: AVR-GCC (winAVR with AVRStudio)
//Project V.: Version - 2.4.1
//Author	: CC Dharmani, Chennai (India)
//			  www.dharmanitech.com
//Date		: 24 Apr 2011
//**************************************************************

#ifndef _SPI_ROUTINES_H_
#define _SPI_ROUTINES_H_

void SPI_init(void);
unsigned char SPI_transmit(unsigned char);
unsigned char SPI_receive(void);

void SPI_select(void);
void SPI_deselect(void);

void SPI_high_speed(void);

#endif
