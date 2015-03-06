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
#include "config.h"

volatile uint8_t logging = FALSE;
volatile uint8_t an_log_almost_valid = FALSE;
volatile uint8_t an_log_valid = FALSE;
volatile uint8_t w_log_valid = FALSE;

volatile uint16_t wheel_count_l = 0;
volatile uint16_t wheel_count_r = 0;
volatile uint16_t susp_l = 0;
volatile uint16_t susp_r = 0;
volatile uint16_t susp_l_safe = 0;
volatile uint16_t susp_r_safe = 0;
volatile uint16_t steering = 0;
volatile uint16_t steering_safe = 0;

volatile uint16_t brake = 0;
volatile uint16_t brake_safe = 0;
volatile uint8_t brake_light_status = OFF;

volatile uint8_t log_MOb;

int main(void) {
	io_init();
	adc_init();
	ancomp_init();
	can_init();
	timer0_init();
	//timer1_init(); // if PWM output
	power_off_default();
	power_off_timer1(); //if no PWM output
	
	//setup CAN RX +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	log_MOb = can_setup_rx(CAN_LOG_ID, CAN_LOG_MASK, CAN_LOG_DLC);
	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	
	interrupts_on();
	can_enable();
	
	while (1) {
		brake = adc_get(BRAKE_PRESSURE);
		ATOMIC_BLOCK(ATOMIC_FORCEON) {
			brake_safe = brake;
		}
		
		if (logging) {
			susp_l = adc_get(SUSPENSION_L);
			susp_r = adc_get(SUSPENSION_R);
			steering = adc_get(STEERING_WHEEL);
			
			if (!an_log_almost_valid) {
				an_log_almost_valid = TRUE;
			} else {
				if (!an_log_valid) {
					an_log_valid = TRUE;
				} else {
					ATOMIC_BLOCK(ATOMIC_FORCEON) {
						susp_l_safe = susp_l;
						susp_r_safe = susp_r;
						steering_safe = steering;
					}
				}
			}
		}
	}
	return 0;
}

//WHEEL_R
void pcISR_in1(void) {
	wheel_count_r++;
}
//WHEEL_L
void pcISR_in2(void) {
	wheel_count_l++;
}

void pcISR_in3(void) {}
void pcISR_in4(void) {}
void pcISR_in5(void) {}
void pcISR_in6(void) {}
void pcISR_in7(void) {}
void pcISR_in8(void) {}
void pcISR_in9(void) {}

void timer0_isr_100Hz(uint8_t interrupt_nbr) {
	
	if (brake_safe >= BRAKES_ON && brake_light_status == OFF) {
		brake_light_status = ON;
		can_setup_tx(CAN_BRAKE_LIGHT_ID, (uint8_t *) &CAN_MSG_BRAKE_ON, CAN_FRONT_LOG_DLC);
	} else if (brake_safe < BRAKES_ON && brake_light_status == ON) {
		brake_light_status = OFF;
		can_setup_tx(CAN_BRAKE_LIGHT_ID, (uint8_t *) &CAN_MSG_BRAKE_OFF, CAN_FRONT_LOG_DLC);
	}
	
	if (logging && !(interrupt_nbr % 10)) { //10 Hz
		if (w_log_valid) {
			uint32_t holder = ((uint32_t) wheel_count_l << 16) | wheel_count_r;
			can_setup_tx(CAN_FRONT_LOG_SPEED_ID, (uint8_t *) &holder, CAN_FRONT_LOG_DLC);
		}
		wheel_count_l = 0;
		wheel_count_r = 0;
		w_log_valid = TRUE;
	}
	
	if (logging && an_log_valid) {
		if (!((interrupt_nbr + 2) % 5)) { //20 Hz
			uint32_t holder = ((uint32_t) susp_l_safe << 16) | susp_r_safe;
			can_setup_tx(CAN_FRONT_LOG_SPEED_ID, (uint8_t *) &holder, CAN_FRONT_LOG_DLC);
		}
		
		if (!((interrupt_nbr + 4) % 5)) { //20 Hz
			uint32_t holder = ((uint32_t) brake_safe << 16) | steering_safe;
			can_setup_tx(CAN_FRONT_LOG_STEER_BRAKE_ID, (uint8_t *) &holder, CAN_FRONT_LOG_DLC);
		}
	}
}

void CAN_ISR_RXOK(uint8_t mob, uint32_t id, uint8_t dlc, uint8_t * data) {
	if (id == CAN_LOG_ID) {
		if (*data == CAN_MSG_LOG_START) {
			logging = TRUE;
			pc_int_on(WHEEL_L);
			pc_int_on(WHEEL_R);
		} else if (*data == CAN_MSG_LOG_STOP) {
			logging = FALSE;
			pc_int_off(WHEEL_L);
			pc_int_off(WHEEL_R);
			an_log_valid = FALSE;
			an_log_almost_valid = FALSE;
			w_log_valid = FALSE;
		}
	}
}
void CAN_ISR_TXOK(uint8_t mob, uint32_t id, uint8_t dlc, uint8_t * data) {}
void CAN_ISR_OTHER(void) {}

void early_bod_warning_ISR(void) {}
void early_bod_safe_ISR(void) {}
