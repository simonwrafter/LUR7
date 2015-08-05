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

#include "ff.h"

#include "SPI_routines.h"

FATFS fs;

int main(void) {
	//io_init();
	//adc_init();
	//ancomp_init();
	//can_init();
	//timer1_init(OFF);
	//power_off_default();
	//power_off_timer0();
	
	DDRC |= 1 << DDC1;
	DDRB |= 1 << DDB0;
	interrupts_on();
	//can_enable();
	
	set_output(LED0, ON);
	
	FIL file;
	FRESULT fr;
	UINT bw;
	
	_delay_ms(1000);
	
	fr = f_mount(&fs, "", 0);
	_delay_ms(1000);
	if (fr) {
		//set_output(LED0, OFF);
	} else {
		fr = f_open(&file, "logloglo.txt", FA_WRITE);
		if (fr) {/*
			do {
				set_output(LED0, ON);
				_delay_ms(300);
				set_output(LED0, OFF);
				_delay_ms(300);
				set_output(LED0, ON);
			} while  (--fr);*/
		} else {
			fr = f_printf(&file, "Hello World!");
			f_close(&file);
		}
	}
	
	while (1) {
		_delay_ms(1000);
		SPI_transmit(0x00);
		SPI_transmit(0xAA);
		SPI_transmit(0x00);
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
