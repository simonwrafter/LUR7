/*
 * main.c - A collection of functions to setup and ease the use of the LUR7 PCB
 * Copyright (C) 2014  Simon Wrafter <simon.wrafter@gmail.com>
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

int main(void) {
	io_init();
	//adc_init();
	//ancomp_init();
	//can_init();
	//timer0_init();
	timer1_init(); // if PWM output
	//power_off_default();
	//power_off_timer1(); //if no PWM output

	interrupts_on();
	//can_enable();
	
	uint16_t duty = 0;
	uint8_t dir = 1;
	
	while (1) {
		timer1_dutycycle(duty++);
		_delay_us(200);
		if (duty == 20000) {
			dir *= -1;
		}
		duty *= dir;
	}
	return 0;
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

void timer0_isr_100Hz(uint8_t interrupt_nbr) {}

void CAN_ISR_RXOK(uint32_t id, uint8_t dlc, uint8_t * data) {}
void CAN_ISR_TXOK(uint32_t id, uint8_t dlc, uint8_t * data) {}
void CAN_ISR_OTHER(void) {}

void early_bod_warning_ISR(void) {}
void early_bod_safe_ISR(void) {}
