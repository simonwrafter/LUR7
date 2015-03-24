/*
 * main.c - Entry Point for execution of code on the mid MCU used in the LUR7.
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
#include "config.h"
#include "display.h"

volatile uint8_t print = FALSE;

int main(void) {
	io_init();
	adc_init();
	ancomp_init();
	can_init();
	timer1_init(OFF);
	power_off_default();
	power_off_timer0();

	interrupts_on();

	while (1) {
		for (int i=0; i < 64; i++) {
			for (int j=0; j<64; j++){
				if (i==j) {
					shift_bit(1);
				} else if (i == j+1) {
					shift_bit(1);
				}else {
					shift_bit(0);
				}
				shift_strobe();
				_delay_ms(500);
			}
		}
	}

	return 0;
}

void timer1_isr_100Hz(uint8_t interrupt_nbr) {

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

void CAN_ISR_RXOK(uint8_t mob, uint32_t id, uint8_t dlc, uint8_t * data) {}
void CAN_ISR_TXOK(uint8_t mob, uint32_t id, uint8_t dlc, uint8_t * data) {}
void CAN_ISR_OTHER(void) {}
void early_bod_warning_ISR(void) {}
void early_bod_safe_ISR(void) {}
