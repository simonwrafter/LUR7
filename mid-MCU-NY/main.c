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

/*! \file main.c
 * \ref main is the Entry Point for execution of code on the mid MCU used in
 * the LUR7.
 *
 * All code is released under the GPLv3 license.
 *
 * \see \ref main
 * \see \ref config.h
 * \see <http://www.gnu.org/copyleft/gpl.html>
 * \author Simon Wrafter
 * \copyright GNU Public License v3.0
 *
 * \defgroup main Main source file
 * \ref main.c is the top source code file for the mid LUR7 MCU.
 *
 * \todo failsafe / manual gear shift
 *
 * \see \ref main.c
 * \see \ref config
 * \see <http://www.gnu.org/copyleft/gpl.html>
 * \author Simon Wrafter
 * \copyright GNU Public License v3.0
 */

#include "../header_and_config/LUR7.h"
#include "config.h"
#include "display.h"

//! The MOb configured for RX of logging start/stop instructions.
volatile uint8_t CAN_DTA_MOb;
//! Variable containing information on whether logging is active or not.
volatile uint8_t logging = FALSE;
//! Flag set when new information has been received and the panel is ready to be updated
volatile uint8_t new_info = TRUE;
//! Left clutch position sensor value.
volatile uint16_t clutch_pos_left = 0;
//! Right clutch position sensor value.
volatile uint16_t clutch_pos_right = 0;
//! Atomically written copy of left clutch position sensor value.
volatile uint16_t clutch_pos_left_atomic = 0;
//! Atomically written copy of right clutch position sensor value.
volatile uint16_t clutch_pos_right_atomic = 0;
//! Used for stopping clutch CAN messages.
volatile uint8_t clutch_CAN_disable = FALSE;
//! Debounce for gear shifting
volatile uint8_t gear_debounce = FALSE;

//void ugly_reset(void);

//! Main function.
/*!
 * The entry point of the execution of code for the middle MCU. All hardware that
 * is to be used is configured and initialised and the remaining peripherals
 * powered off.
 *
 * To the largest extent possible tasks are handled through interrupts. When
 * this is not possible the task may be performed in the loop of the
 * \ref main function.
 *
 * The structure of main is:
 */
int main(void) {
	//! <ul>
	//! <li> SETUP <ul>
	//! <li> Initialisation <ol>
	io_init(); //! <li> initialise LUR_io.
	adc_init(); //! <li> initialise LUR7_adc.
	ancomp_init(); //! <li> initialise LUR7_ancomp.
	can_init(); //! <li> initialise LUR7_CAN.
	timer1_init(OFF); //! <li> initialise LUR7_timer1.
	//! </ol>

	//! <li> LUR7_power. <ol>
	power_off_default(); //! <li> power off unused periferals.
	power_off_timer0(); //! <li> no PWM output is required, so LUR7_timer1 is powered off.
	//! </ol>

	//! <li> Setup CAN RX <ol>
	CAN_DTA_MOb = can_setup_rx(CAN_DTA_ID, CAN_DTA_MASK, CAN_DTA_DLC); //! <li> Reception of DTA packages, ID 0x2000-3.
	//! </ol>

	//! <li> Input interrupts <ol>
	ext_int_on(IO_GEAR_UP, 0, 1); //! <li> Gear up, any (!) flank trigger external interrupt
	ext_int_on(IO_GEAR_DOWN, 0, 1); //! <li> Gear down, any (!) flank trigger external interrupt
	ext_int_on(IO_GEAR_NEUTRAL, 1, 0); //! <li> Neutral gear, falling flank trigger external interrupt

	//pc_int_on(IO_GP_BTN);
	pc_int_on(IO_LOG_BTN);
	//! </ol>

	//! <li> Enable system <ol>
	interrupts_on(); //! <li> enable interrupts.
	can_enable(); //! <li> enable CAN.
	//! </ol>
	//! </ul>

	//! <li> LOOP <ul>
	while (1) {
		//! <li> Always do: <ol>
		if (!clutch_CAN_disable) {
			clutch_pos_left = adc_get(IO_CLUTCH_LEFT); //! <li> get left clutch paddle position
			ATOMIC_BLOCK(ATOMIC_FORCEON) {
				clutch_pos_left_atomic = clutch_pos_left; //! <li> copy value to atomic variable
			} // end ATOMIC_BLOCK

			clutch_pos_right = adc_get(IO_CLUTCH_RIGHT); //! <li> get right clutch paddle position
			ATOMIC_BLOCK(ATOMIC_FORCEON) {
				clutch_pos_right_atomic = clutch_pos_right; //! <li> copy value to atomic variable
			} // end ATOMIC_BLOCK
		}
		
		//! </ol>
		//! <li> If new information for panel <ol>
		if (new_info) {
			new_info = FALSE; //! <li> clear flag
			update_display(get_input(IO_ALT_BTN)); //! <li> update panel
		} //! </ol>
	} //! </ul>
	return 0; //! </ul>
}

/*void ugly_reset(void) {
	set_output(OUT8, GND);
	ext_int_off(IO_GEAR_UP);
	ext_int_off(IO_GEAR_DOWN);
	_delay_ms(1);
	set_output(OUT8, TRI);
	ext_int_on(IO_GEAR_UP, 1, 1);
	ext_int_on(IO_GEAR_DOWN, 1, 1);
}*/

// Interrupts
//! Timer Interrupt, 100 Hz
/*!
 * In order to schedule tasks or perform them with a well defined time delta,
 * the 100 Hz interrupt generator of LUR7_timer0 is used.
 *
 * \note To ensure that no corrupted values are sent, only atomically written
 * copies of all variables are used.
 *
 * All tasks scheduled use the CAN bus to transmit information. To not have all
 * messages sent out simultaneously they are spread out across different
 * occurrences of the interrupt using \p interrupt_nbr to identify when to execute.
 * The following table shows how tasks are spread out.
 *
 * | \p interrupt_nbr | Clutch position |
 * | :--------------: | :-------------: |
 * | 0, 10, 20, .. 90 | x               |
 * | 1, 11, 21, .. 91 | x               |
 * | 2, 12, 22, .. 92 | x               |
 * | 3, 13, 23, .. 93 | x               |
 * | 4, 14, 24, .. 94 | x               |
 * | 5, 15, 25, .. 95 | x               |
 * | 6, 16, 26, .. 96 | x               |
 * | 7, 17, 27, .. 97 | x               |
 * | 8, 18, 28, .. 98 | x               |
 * | 9, 19, 29, .. 99 | x               |
 *
 * \param interrupt_nbr The id of the interrupt, counting from 0-99.
 */
void timer1_isr_100Hz(uint8_t interrupt_nbr) {
	uint32_t c_data = ((uint32_t) clutch_pos_left_atomic << 16) | clutch_pos_right_atomic;
	can_setup_tx(CAN_CLUTCH_ID, (uint8_t *) &c_data, CAN_GEAR_CLUTCH_LAUNCH_DLC);
}

/*!
 * lzlrsgkoslr
 */
void timer0_isr_stop(void) {
	gear_debounce = FALSE;
}

//! Gear Up interrupt handler
/*!
 * When the paddle for changing gears up is depressed, this ISR is executed
 * sending a message to the rear MCU to do the shifting.
 */
ISR (INT_GEAR_UP) { //IN9
	if (!get_input(IO_GEAR_UP) && !gear_debounce) {
		can_setup_tx(CAN_GEAR_ID, CAN_MSG_GEAR_UP, CAN_GEAR_CLUTCH_LAUNCH_DLC);
	} else {
		gear_debounce = TRUE;
		timer0_start(5000);
	}
}
//! Gear Down interrupt handler
/*!
 * When the paddle for changing gears down is depressed, this ISR is executed
 * sending a message to the rear MCU to do the shifting.
 */
ISR (INT_GEAR_DOWN) { //IN8
	if (!get_input(IO_GEAR_DOWN) && !gear_debounce) {
		can_setup_tx(CAN_GEAR_ID, CAN_MSG_GEAR_DOWN, CAN_GEAR_CLUTCH_LAUNCH_DLC);
	} else {
		gear_debounce = TRUE;
		timer0_start(5000);
	}
}
//! Neutral Gear interrupt handler
/*!
 * When the button finding the neutral gear is pressed, this ISR is executed
 * sending a message to the rear MCU to do the shifting. The message also
 * contains information on current gear, allowing the rear MCU to behave
 * accordingly.
 */
ISR (INT_GEAR_NEUTRAL) { //IN5
	if (get_input(IO_ALT_BTN)) {
		can_setup_tx(CAN_GEAR_ID, CAN_MSG_GEAR_NEUTRAL_SINGLE, CAN_GEAR_CLUTCH_LAUNCH_DLC);
	} else {
		can_setup_tx(CAN_GEAR_ID, CAN_MSG_GEAR_NEUTRAL_REPEAT, CAN_GEAR_CLUTCH_LAUNCH_DLC);
	}
}

//! Pin Change Interrupt handler for IN1.
/*! \todo manual gear changes on/off, FAILSAFE */
void pcISR_in1(void) {
	//ext_int_off(IO_GEAR_UP);
	//ext_int_off(IO_GEAR_DOWN);
	//ext_int_off(IO_GEAR_NEUTRAL);
	//clutch_CAN_disable = TRUE;
	//set_output(IO_GEAR_STOP_LED, ON);
}

//! Pin Change Interrupt handler for IN2.
/*! Logging start/stop button.broadcasts a messabe to start or stop logging. */
void pcISR_in2(void) { /*
	if (get_input(IO_LOG_BTN)) {
		if (get_input(IO_ALT_BTN)) {
			if (logging) {
				can_setup_tx(CAN_LOG_ID, (uint8_t *) &CAN_MSG_LOG_STOP, CAN_LOG_DLC);
				logging = TRUE;
			} else {
				can_setup_tx(CAN_LOG_ID, (uint8_t *) &CAN_MSG_LOG_START, CAN_LOG_DLC);
				logging = FALSE;
			}
		} else {
			can_setup_tx(CAN_LAUNCH_ID, (uint8_t *) &CAN_MSG_LAUNCH, CAN_GEAR_CLUTCH_LAUNCH_DLC);
		}
	} */
}

//! Pin Change Interrupt handler for IN3.
/*! Alternate purpose button, not used as interrupt */
void pcISR_in3(void) {}
//! Pin Change Interrupt handler for IN4.
/*! \warning in6 used as analog input */
void pcISR_in4(void) {}
//! Pin Change Interrupt handler for IN5.
/*! \warning in5 used as external interrupt */
void pcISR_in5(void) {}
//! Pin Change Interrupt handler for IN6.
/*! \warning in6 used as analog input */
void pcISR_in6(void) {}
//! Pin Change Interrupt handler for IN7.
/*! unused */
void pcISR_in7(void) {}
//! Pin Change Interrupt handler for IN8.
/*! \warning in8 used as external interrupt */
void pcISR_in8(void) {}
//! Pin Change Interrupt handler for IN9.
/*! \warning in9 used as external interrupt */
void pcISR_in9(void) {}

//! CAN message receiver function.
/*!
 * The Mid MCU listens to messages from the DTA, the information extracted from
 * these packages are presented to the driver on the LED panel.
 *
 * The messages are read as follows:
 */
void CAN_ISR_RXOK(uint8_t mob, uint32_t id, uint8_t dlc, uint8_t * data) {
	//! <ul>
	uint8_t id_lsb = ((uint8_t *) &id)[3]; //! <li> extract message specifier.
	if (mob == CAN_DTA_MOb) { //! <li> if received from DTA: <ul>
		uint16_t ana3 = 0;
		switch (id_lsb) {
			case 0 : //! <li> ID = 0x2000. <ul>
				update_RPM((data[6] << 8) | data[7]); //! <li> extract RPM.
				update_watertemp((data[2] << 8) | data[3]); //! <li> extract water temperature [C].
				new_info = TRUE; //! <li> set flag to update panel
				break; //! </ul>
			case 1 : //! <li> ID = 0x2001. <ul>
				update_speed((data[2] << 8) | data[3]);  //! <li> extract speed [km/h * 10]
				break; //! </ul>
			case 2 : //! <li> ID = 0x2002. <ul>
				update_oiltemp((data[5] << 8) | data[6]);  //! <li> extract oil temperature [C].
				break; //! </ul>
			case 4 : //! <li> ID = 0x2004. <ul>
				ana3 = ((uint16_t) data[2] << 8) | data[3];//! <li> extract current gear.
				if (ana3 > 700 || ana3 < 900){
					update_gear(1); // 791
				} else if (ana3 > 1100 && ana3 < 1500){
					update_gear(0); // 1296
				} else if (ana3 > 1600 && ana3 < 1850){
					update_gear(2); // 1730
				} else if (ana3 > 2450 && ana3 < 2700){
					update_gear(3); // 2587
				} else if (ana3 > 3350 && ana3 < 3600){
					update_gear(4); // 3500
				} else if (ana3 > 4350 && ana3 < 4600){
					update_gear(5); // 4453
				}
				else {
					update_gear(10); //blank display
				}
				break; //! </ul>
			default :
				break;
		}
	} //! </ul>
	//! </ul>
}
//! CAN message sent function.
/*! Executed when TX completes. */
void CAN_ISR_TXOK(uint8_t mob, uint32_t id, uint8_t dlc, uint8_t * data) {}
//! CAN Error hendler.
/*!
 * \todo implement CAN error handling
 */
void CAN_ISR_OTHER(void) {}

//! Brown Out warning.
/*!
 * \todo implement BOD handling
 */
void early_bod_warning_ISR(void) {}
//! Brown Out warning over.
/*!
 * \todo implement BOD handling
 */
void early_bod_safe_ISR(void) {}
