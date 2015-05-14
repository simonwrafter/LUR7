/*
 * / main.c - A collection of functions to setup and ease the use of the LUR7 PCB
 * / Copyright (C) 2015  Simon Wrafter <simon.wrafter@gmail.com>
 * /
 * / This program is free software: you can redistribute it and/or modify
 * / it under the terms of the GNU General Public License as published by
 * / the Free Software Foundation, either version 3 of the License, or
 * / (at your option) any later version.
 * /
 * / This program is distributed in the hope that it will be useful,
 * / but WITHOUT ANY WARRANTY; without even the implied warranty of
 * / MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * / GNU General Public License for more details.
 * /
 * / You should have received a copy of the GNU General Public License
 * / along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "../header_and_config/LUR7.h"

int main(void) {
	io_init();

	set_output(OUT1, OFF);
	set_output(OUT2, OFF);
	set_output(OUT3, OFF);
	set_output(OUT4, OFF);
	set_output(OUT5, OFF);
	set_output(OUT6, OFF);
	set_output(OUT7, OFF);
	set_output(OUT8, OFF);
	pc_int_on(IN1);
	pc_int_on(IN2);
	pc_int_on(IN3);
	pc_int_on(IN4);
	pc_int_on(IN5);
	pc_int_on(IN6);
	pc_int_on(IN7);
	pc_int_on(IN8);
	pc_int_on(IN9);


	interrupts_on();

	while (1) {
	}
	return 0;
}

void pcISR_in1(void) {
	toggleAll();
}
void pcISR_in2(void) {
	toggleAll();
}
void pcISR_in3(void) {
	toggleAll();
}
void pcISR_in4(void) {
	toggleAll();
}
void pcISR_in5(void) {
	toggleAll();
}
void pcISR_in6(void) {
	toggleAll();
}
void pcISR_in7(void) {
	toggleAll();
}
void pcISR_in8(void) {	
	toggleAll();
}
void pcISR_in9(void) {
	toggleAll();
}

void toggleAll(void){
	toggle_output(OUT1);
	toggle_output(OUT2);
	toggle_output(OUT3);
	toggle_output(OUT4);
	toggle_output(OUT5);
	toggle_output(OUT6);
	toggle_output(OUT7);
	toggle_output(OUT8);

}	

void timer1_isr_100Hz(uint8_t interrupt_nbr) {}

//void CAN_ISR_RXOK(uint32_t id, uint8_t dlc, uint8_t * data) {}
//void CAN_ISR_TXOK(uint32_t id, uint8_t dlc, uint8_t * data) {}
//void CAN_ISR_OTHER(void) {}

void early_bod_warning_ISR(void) {}
void early_bod_safe_ISR(void) {}
