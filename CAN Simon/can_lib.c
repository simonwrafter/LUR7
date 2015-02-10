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

#include "..header_and_config/LUR7.h"
#include "can_lib.h"

/*******************************************************************************x
 * public api
 ******************************************************************************/

/*
 * To start using CAN run this function during the setup phase of the code.
 * It activates TXOK and RXOK interrupts on all 6 MObs.
 */
void can_init(void) {
	CANGCON = (1<<SWRES); // reset CAN
	CANTCON = 0x00; //set timing prescaler to zero
	
	CANBT1 = CONF_CANBT1; // set baudrate, CONF_CANBT1 defined in .h file
	CANBT2 = CONF_CANBT2; // set baudrate, CONF_CANBT2 defined in .h file
	CANBT3 = CONF_CANBT3; // set baudrate, CONF_CANBT3 defined in .h file
	
	CANGIE = (1<<ENIT) | (1<<ENRX) | (1<<ENTX); //enable TXOK an RXOK interrupts
	CANIE1 = 0; // for compatibility
	CANIE2 = (1<<IEMOB5) | (1<<IEMOB4) | (1<<IEMOB3) | (1<<IEMOB2) | (1<<IEMOB1) | (1<<IEMOB0); // enable interrupts on all MOb
	
	//clear all MOb
	for (uint8_t mob_number = 0; mob_number < NBR_OF_MOB; mob_number++) {
		CANPAGE = (mob_number << 4);	// select each MOb in turn
		
		for (uint8_t volatile *can_reg=&CANSTMOB; can_reg<&CANSTML; can_reg++) {
			*can_reg=0x00; // go through all registers and set to zero
		}
	}
}

/*
 * After the init function, run this function to setup receiving messages.
 */
void can_setup_rx(uint8_t mob_nbr, uint32_t mob_id, uint32_t mob_msk, uint8_t mob_rtr, uint8_t mob_dlc) {
	uint8_t save_CANPAGE = CANPAGE; //save CANPAGE
	CANPAGE = mob_nbr << MOBNB0; // put mob_nbr in MOb Number bits
	
	_can_set_id(mob_id, mob_rtr);
	_can_set_msk(mob_msk);
	
	mob_dlc = (mob_dlc > 8) ? 8 : mob_dlc;
	
	CONCDMOB = (1 << CONMOB1) | (mob_dlc << DLC0);
	
	CANPAGE = save_CANPAGE; //restore CANPAGE
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
	return (CANIDT4 >> 3) | (CANIDT3 << 5) | (CANIDT2 << 13) | (CANIDT1 << 21);
}

/*
 * set ID registers and RTR bit
 */
void _can_set_id(uint32_t identifier, uint8_t rtr_id) {
	uint8_t id_v[4] = (uint8_t *) (identifier << 3);
	CANIDT1 = id_v[3];
	CANIDT2 = id_v[2];
	CANIDT3 = id_v[1];
	CANIDT4 = id_v[0] | ((rtr_id ? 1 : 0) << RTRTAG);
}

/*
 * set mask registers
 */
void _can_set_msk(uint32_t mask) {
	uint8_t mask_v[4] = (uint8_t *) (mask << 3);
	CANIDM1 = mask_v[3];
	CANIDM2 = mask_v[2];
	CANIDM3 = mask_v[1];
	CANIDM4 = mask_v[0] | (1<<RTRMSK) | (1<<IDEMSK);
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
		_can_handle_RXOK();
	} else if (CANSTMOB & (1 << TXOK)) {
		CANSTMOB &= ~(1 << TXOK); //clear interrupt flag
		_can_handle_TXOK();
	} else {
		CANSTMOB = 0x00;
	}
	
	CANPAGE = save_CANPAGE; //restore CANPAGE
	SREG = save_SREG; //restore SREG
}

/*
 * helper function for ISR receiving messages, relies on external function
 * implemented in application.
 */
void _can_handle_RXOK() {
	uint32_t id = _can_get_id();
	uint8_t dlc = CACDMOB & 0x0F;
	uint8_t data[dlc];
	
	for (int i = 0; i < dlc; i++) {
		data[i] = CANMSG; //CANMSG autoincrements, !AINC = 0.
	}
	
	CAN_ISR_RXOK(id, dlc, data);
}

/*
 * helper function for ISR sending messages, relies on external function
 * implemented in application.
 */
void _can_handle_TXOK() {
	;
}

