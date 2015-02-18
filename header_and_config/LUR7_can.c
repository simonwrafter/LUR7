/*
 * can_lib.c - A collection of functions to setup and ease the use of the LUR7 PCB
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

#include "LUR7.h"
#include "LUR7_can.h"

/*******************************************************************************x
 * public api
 ******************************************************************************/

/*
 * To start using CAN run this function during the setup phase of the code.
 * It activates TXOK and RXOK interrupts on all 6 MObs.
 */
void can_init(void) {
	CANGCON = (1<<SWRES); // reset CAN
	//CANTCON = 0x00; //set timing prescaler to zero

	CANBT1 = CONF_CANBT1; // set baudrate, CONF_CANBT1 defined in .h file
	CANBT2 = CONF_CANBT2; // set baudrate, CONF_CANBT2 defined in .h file
	CANBT3 = CONF_CANBT3; // set baudrate, CONF_CANBT3 defined in .h file

	CANGIE = 0xFF;// (1<<ENIT) | (1<<ENRX) | (1<<ENTX); //enable TXOK an RXOK interrupts
	CANIE1 = 0; // for compatibility
	CANIE2 = (1<<IEMOB5) | (1<<IEMOB4) | (1<<IEMOB3) | (1<<IEMOB2) | (1<<IEMOB1) | (1<<IEMOB0); // enable interrupts on all MOb

	//clear all MOb
	for (uint8_t mob_number = 0; mob_number < NBR_OF_MOB; mob_number++) {
		CANPAGE = (mob_number << 4); // select each MOb in turn

		//initiate everything to zero
		CANSTMOB = 0x00;
		CANCDMOB = 0x00;
		
		//CANIDT4 = 0x00;
		//CANIDT3 = 0x00;
		//CANIDT2 = 0x00;
		//CANIDT1 = 0x00;
		//CANIDM4 = 0x00;
		//CANIDM3 = 0x00;
		//CANIDM2 = 0x00;
		//CANIDM1 = 0x00;
	}
}

/*
 * After the init function, run this function to setup receiving messages.
 */
void can_setup_rx(uint32_t mob_id, uint32_t mob_msk, uint8_t mob_dlc) {
	//uint8_t save_CANPAGE = CANPAGE; //save CANPAGE
	CANPAGE = _can_get_free_mob() << MOBNB0; // select first free MOb for use

	_can_set_id(mob_id); //id to compare against
	_can_set_msk(mob_msk); //mask for comparing id

	mob_dlc = (mob_dlc > 8) ? 8 : mob_dlc; // expected number of data bytes

	CANCDMOB = (1 << CONMOB1) | (1 << IDE) | (mob_dlc << DLC0); // configure MOb for reception of mob_dlc number of data bytes

	//CANPAGE = save_CANPAGE; //restore CANPAGE
}

/*
 * send message with id = mob_id, data = *mob_data and dlc = mob_dlc
 * run once per message. use CAN_ISR_TXOK() for additional actions on send completion
 */
void can_setup_tx(uint32_t mob_id, uint8_t * mob_data, uint8_t mob_dlc) {
	//uint8_t save_CANPAGE = CANPAGE; //save CANPAGE
	CANPAGE = _can_get_free_mob() << MOBNB0; // select first free MOb for use
	
	CANSTMOB = 0x00; //clear MOb status

	_can_set_id(mob_id);

	for (uint8_t i = 0; i < mob_dlc; i++) {
		CANMSG = mob_data[i]; // set data
	}

	CANCDMOB = (1<<CONMOB0) | (1 << IDE) | (mob_dlc << DLC0); // enable transmission and set DLC

	//CANPAGE = save_CANPAGE; //restore CANPAGE
}

/*
 * run this function at the end of the setup procedure.
 */
void can_enable() {
	CANGCON |= (1 << ENASTB); //enable CAN
}

/*
 * for completeness
 */
void can_disable() {
	CANGCON &= ~(1 << ENASTB); //disable CAN
}

/*******************************************************************************
 * static functions
 ******************************************************************************/

/*
 * rebuilds the message identifier
 */
uint32_t _can_get_id() {
	return (CANIDT4 >> 3) | ((uint32_t)CANIDT3 << 5) | ((uint32_t)CANIDT2 << 13) | ((uint32_t)CANIDT1 << 21);
}

/*
 * set ID registers and RTR bit
 */
void _can_set_id(uint32_t identifier) {
	uint32_t id_v = (identifier << 3);

	CANIDT1 = *((uint8_t *) &id_v + 3);
	CANIDT2 = *((uint8_t *) &id_v + 2);
	CANIDT3 = *((uint8_t *) &id_v + 1);
	CANIDT4 = *((uint8_t *) &id_v + 0);
}

/*
 * set mask registers
 */
void _can_set_msk(uint32_t mask) {
	uint32_t mask_v = (mask << 3);
	CANIDM1 = *((uint8_t *) &mask_v + 3);
	CANIDM2 = *((uint8_t *) &mask_v + 2);
	CANIDM3 = *((uint8_t *) &mask_v + 1);
	CANIDM4 = *((uint8_t *) &mask_v + 0) | (1<<RTRMSK) | (1<<IDEMSK);
}

/*
 * Returns the number of the first free MOb, 0xFF if no free MOb is found
 */
uint8_t _can_get_free_mob() {
	for (uint8_t mob = 0; mob < NBR_OF_MOB; mob++) {
		if (CANEN2 & (1 << mob)) {
			return mob;
		}
	}
	return 0xFF;
}

/*******************************************************************************
 * Interrupt handling
 ******************************************************************************/

ISR (CAN_INT_vect) {
	uint8_t save_SREG = SREG; // save SREG
	uint8_t save_CANPAGE = CANPAGE; // save CANPAGE

	CANPAGE = CANHPMOB & 0xF0; // select MOb with highest priority interrupt

	if (CANSTMOB & (1 << RXOK)) {
		CANSTMOB &= ~(1 << RXOK); //clear interrupt flag
		CANCDMOB &= ~((1 << CONMOB1) | (1 << CONMOB0)); //disable MOb
		_can_handle_RXOK();
	} else if (CANSTMOB & (1 << TXOK)) {
		CANSTMOB &= ~(1 << TXOK); //clear interrupt flag
		CANCDMOB &= ~((1 << CONMOB1) | (1 << CONMOB0)); //disable MOb
		_can_handle_TXOK();
	} else {
		CANSTMOB = 0x00; //clear interrupt flag, FIXME: errors not handled
		CANCDMOB &= ~((1 << CONMOB1) | (1 << CONMOB0)); //disable MOb
		CANGIT = 0x00; //clear general interrupts
		CAN_ISR_OTHER();
	}

	CANPAGE = save_CANPAGE; //restore CANPAGE
	SREG = save_SREG; //restore SREG
}

/*
 * helper function for ISR receiving messages, relies on external function
 * implemented in application.
 */
void _can_handle_RXOK() {
	uint32_t id = _can_get_id(); // get id
	uint8_t dlc = CANCDMOB & 0x0F; // get dlc
	uint8_t data[dlc]; // create vector for data

	//read data
	for (int i = 0; i < dlc; i++) {
		data[i] = CANMSG; //CANMSG autoincrements, !AINC = 0.
	}

	// send information to extern function in application to act on information
	CAN_ISR_RXOK(id, dlc, data);

	CANCDMOB |= (1 << CONMOB1); // reenable reception
}

/*
 * helper function for ISR sending messages, relies on external function
 * implemented in application.
 */
void _can_handle_TXOK() {
	uint32_t id = _can_get_id(); // get id
	uint8_t dlc = CANCDMOB & 0x0F; // get dlc
	uint8_t data[dlc]; // create vector for data

	//read data
	for (int i = 0; i < dlc; i++) {
		data[i] = CANMSG; //CANMSG autoincrements, !AINC = 0.
	}

	CAN_ISR_TXOK(id, dlc, data); // extern function if more actions are required after TXOK
	CANCDMOB = 0x00; // clear control register
}
