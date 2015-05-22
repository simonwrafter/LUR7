/*
 * main.c - The main file for the LUR7 logger project.
 * Copyright (C) 2015  Simon Wrafter <simon.wrafter@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "../header_and_config/LUR7.h"
/*
#define F_CPU 16000000UL

#include <avr/io.h>
#include <util/delay.h>
*/
#include "spi_routines.h"
#include "sd_routines.h"
//#include "fat32.h"

int main(void) {
	unsigned char option, error, data, FAT32_active;
	unsigned int i;
	unsigned char fileName[13];
	
	//io_init();
	//adc_init();
	//ancomp_init();
	//can_init();
	//timer1_init(OFF);
	//power_off_default();
	//power_off_timer0();
	
	//interrupts_on();
	//can_enable();
	
	//**********************
	// Init
	//**********************
	
	SPI_init();
	
	cardType = 0; //idioti
	
	for (i=0; i<10; i++) {
		error = SD_init();
		if (!error) {
			break;
		}
	}

	//error = getBootSectorData(); //read boot sector and keep necessary data in global variables
	if(error) {
		//FAT32 incompatible drive
		while (1) {
			;
		}
	}
	
	SPI_high_speed();
	_delay_ms(1);   	//some delay for settling new spi speed
	
	while (1) {
		
	}
}

void pcISR_in1(void) {}
void pcISR_in2(void) {}
void pcISR_in3(void) {}
void pcISR_in4(void) {}
void pcISR_in5(void) {}
void pcISR_in6(void) {}
void pcISR_in7(void) {}
void pcISR_in8(void) {}
void pcISR_in9(void) {}

void timer1_isr_100Hz(uint8_t interrupt_nbr) {}
void timer0_isr_stop(void) {}

void CAN_ISR_RXOK(uint8_t mob, uint32_t id, uint8_t dlc, uint8_t * data) {}
void CAN_ISR_TXOK(uint8_t mob, uint32_t id, uint8_t dlc, uint8_t * data) {}
void CAN_ISR_OTHER(void) {}

void early_bod_warning_ISR(void) {}
void early_bod_safe_ISR(void) {}
