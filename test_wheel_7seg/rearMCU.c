/*
 * / rearMCU.c - A collection of functions to setup and ease the use of the LUR7 PCB
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
#include "rearMCU.h"
//#include "../header_and_config/LUR7_io.h"


static volatile uint16_t speed_l = 0;
static volatile uint16_t speed_r = 0;
static volatile uint16_t susp_l = 0;
static volatile uint16_t susp_r = 0;
volatile uint32_t overflowTimeCounter = 0;
volatile uint32_t velocity_l = 0;
volatile uint32_t velocity_r = 0;
uint8_t seg = 0;

void init_interrupt(void) {
	ext_int_on(IN5, 0, 1);
	ext_int_on(IN8, 0, 1);
	ext_int_on(IN9, 0, 1);
}

void update_analog(void) {
	//susp_l = get_analog(SUSPENSION_L);
	//susp_r = get_analog(SUSPENSION_R);
	//steering = get_analog(STEERING_WHEEL);
}

ISR(INT_IN9_vect) { // SPEED_L
	speed_l++; //for now...
	//update7seg(speed_l);

}

//ISR(INT1_vect) { // SPEED_R
//	speed_r++; //for now...
//}

//With each bit change 16 us, TIMER1 will overflow each 1,048576 s
ISR(TIMER1_OVF_vect){
	overflowTimeCounter++;
	velocity_l = speed_l;
	speed_l = 0;
	update7seg(velocity_l);
}

void update7seg(uint8_t i){
	seg = bin_to_7seg(i, 0);
	set_output(OUT1, !(seg & _BV(7)));
	set_output(OUT2, !(seg & _BV(6)));
	set_output(OUT3, !(seg & _BV(5)));
	set_output(OUT4, !(seg & _BV(4)));
	set_output(OUT5, !(seg & _BV(3)));
	set_output(OUT6, !(seg & _BV(2)));
	set_output(OUT7, !(seg & _BV(1)));
}

void lightAll(){
	if(get_input(IN8)){
		set_output(OUT1, 0);
		set_output(OUT2, 0);
		set_output(OUT3, 0);
		set_output(OUT4, 0);
		set_output(OUT5, 0);
		set_output(OUT6, 0);
		set_output(OUT7, 0);
		set_output(OUT8, 0);
	} else{
		set_output(OUT1, 1);
		set_output(OUT2, 1);
		set_output(OUT3, 1);
		set_output(OUT4, 1);
		set_output(OUT5, 1);
		set_output(OUT6, 1);
		set_output(OUT7, 1);
		set_output(OUT8, 1);
	}
}

void blink(uint8_t bo){
	if(bo){
		set_output(OUT2, 1);
	}
	else{
		set_output(OUT2, 0);

	}
}

uint8_t bin_to_7seg(uint8_t binary, uint8_t dp) {
	if (binary >= 0 && binary <= 10) {
		if (!dp) {
			return sev_seg[binary];
		} else {
			return sev_seg[binary] + 1;
		}
	}
	return sev_seg[10];
}

void pcISR_in1(){}
void pcISR_in2(){}
void pcISR_in3(){}
void pcISR_in4(){}
void pcISR_in5(){}
void pcISR_in6(){}
void pcISR_in7(){}
void pcISR_in8(){}
void pcISR_in9(){}

