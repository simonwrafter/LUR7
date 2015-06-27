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

int main(void) {
	io_init(); //! <li> initialise LUR_io.
	adc_init(); //! <li> initialise LUR7_adc.
	ancomp_init(); //! <li> initialise LUR7_ancomp.
	can_init(); //! <li> initialise LUR7_CAN.
	timer1_init(OFF); //! <li> initialise LUR7_timer0.

	power_off_default(); //! <li> power off unused periferals.
	power_off_timer0(); //! <li> no PWM output is required, so LUR7_timer1 is powered off.

	interrupts_on(); //! <li> enable interrupts.
	can_enable(); //! <li> enable CAN.

	update_RPM(7000);
	update_watertemp(90);
	update_speed(456);
	update_oiltemp(78);
	update_gear(3);
	
	shift_strobe();
	
	while (1) {
		
		update_display();
		_delay_ms(200);
		/*
		// blink
		for (uint8_t i = 0; i<7; i++) {
			for (uint8_t j = 0; j<64; j++) {
				if (i%2) {
					shift_bit(ON);
				} else {
					shift_bit(OFF);
				}
			}
			shift_strobe();
			_delay_ms(300);
		}
		
		// varannan
		for (uint8_t i = 0; i<6; i++) {
			for (uint8_t j = 0; j<70; j++) {
				if ((i+j) & 1) {
					shift_bit(ON);
				} else {
					shift_bit(OFF);
				}
			}
			shift_strobe();
			_delay_ms(700);
		}
		
		//vandraren
		for (uint8_t n=0; n<2; n++) {
			for (uint8_t i = 0; i<64; i++) {
				for (uint8_t j = 0; j<64; j++) {
					if (i==j) {
						shift_bit(ON);
					} else {
						shift_bit(OFF);
					}
				}
				shift_strobe();
				//_delay_ms(2);
			}
			for (uint8_t i = 0; i<64; i++) {
				for (uint8_t j = 0; j<64; j++) {
					if (64-i==j) {
						shift_bit(ON);
					} else {
						shift_bit(OFF);
					}
				}
				shift_strobe();
				//_delay_ms(2);
			}
		}
		*/
	}
	return 0; //! </ul>
}

void timer1_isr_100Hz(uint8_t interrupt_nbr) {}

void timer0_isr_stop(void) {}

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
