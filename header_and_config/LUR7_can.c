/*
 * LUR7_can.c - A collection of functions to setup and ease the use of the LUR7 PCB
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

/*! \file LUR7_can.c
 * \ref LUR7_CAN performs communication tasks between the microcontrollers, ECU
 * and onboard logging computer using the CAN protocol.
 *
 * All code is released under the GPLv3 license.
 *
 * When writing code for the LUR7 PCB this file should not be included directly,
 * instead you should include the \ref LUR7.h file to each source file.
 *
 * \see LUR7_can
 * \see LUR7_can.h
 * \see <http://www.gnu.org/copyleft/gpl.html>
 * \author Simon Wrafter
 * \copyright GNU Public License v3.0
 *
 * \defgroup LUR7_CAN Shared - CAN Bus Communication
 *
 * This is the killer feature of the ATmega32M1, a built in CAN bus controller.
 * CAN is a communication bus used heavily in the automotive industry, it is
 * designed to withstand both common and differential mode electro-magnetic
 * disturbances. In order to read information from the ECU (DTA S60pro) there is
 * a choice between RS-232 and CAN, due to the environmental requirements with
 * large electro-magnetic disturbances the CAN system is necessary.
 *
 * The CAN bus is extended to include not only one microcontroller and the ECU.
 * All electronics nodes will attatch to the same bus and use it to communicate.
 *
 * For a better understanding of how the CAN bus works see the ISO standard
 * linked below, the ATmege32M1 manual and other web resources. Delving in to
 * the details of the system is beyond the scope of this document.
 *
 * \see LUR7_can.c
 * \see LUR7_can.h
 * \see <http://www.dtafast.co.uk/Downloads/Manuals/S%20Series%20Manual.pdf>
 * \see <http://www.bosch-semiconductors.de/media/pdf_1/canliteratur/can2spec.pdf>
 * \see <http://www.gnu.org/copyleft/gpl.html>
 * \author Simon Wrafter
 * \copyright GNU Public License v3.0
 */

#include "LUR7.h"
#include "LUR7_can.h"

/*******************************************************************************
 * constant variable definitions
 ******************************************************************************/
// Addresses, Masks and DLCs
// +  DTA
// +  +  General (0x2000 - 0x2003)
const uint32_t CAN_DTA_ID = 0x00002000; //!< The base ID of CAN messages from the DTA
const uint32_t CAN_DTA_MASK = 0xFFFFFFFC; //!< Mask for the four lowest number DTA IDs (0x2000 - 0x2003)
const uint8_t  CAN_DTA_DLC = 8; //!< DLC of DTA messages

// +  Front-MCU
// +  +  Logging
const uint32_t CAN_FRONT_LOG_SPEED_ID = 0x00004000; //!< Message ID for front wheel speeds
const uint32_t CAN_FRONT_LOG_SUSPENSION_ID = 0x00004001; //!< Message ID for front suspension
const uint32_t CAN_FRONT_LOG_STEER_BRAKE_ID = 0x00004002; //!< Message ID for steering and braking
const uint32_t CAN_FRONT_LOG_STEER_BRAKE_MASK = 0xFFFFFFFF; //!< Mask for steering and braking
const uint8_t  CAN_FRONT_LOG_DLC = 4; //!< DLC of messages from front logging node

// +  Mid-MCU
// +  +  Gear and Clutch
const uint32_t CAN_GEAR_ID = 0x00001500; //!< The ID for messages carrying Gear Change information
const uint32_t CAN_CLUTCH_ID = 0x00001501; //!< The ID for messages carrying Clutch Position information
const uint32_t CAN_LAUNCH_ID = 0x00001502; //!< The ID of CAN messages for lunch control
const uint32_t CAN_GEAR_CLUTCH_LAUNCH_MASK = 0xFFFFFFFC; //!< Mask for Gear Change, Clutch Position and Launch Control IDs
const uint8_t  CAN_GEAR_CLUTCH_LAUNCH_DLC = 4; //!< DLC of Gear Change and Clutch Position messages

// +  +  Logging
const uint32_t CAN_LOG_ID = 0x00003000; //!< The ID of CAN messages for starting/stoping logging
const uint32_t CAN_LOG_MASK = 0xFFFFFFFF; //!< Mask for the LOG instruction
const uint8_t  CAN_LOG_DLC = 1; //!< DLC of DTA messages

// +  Rear MCU
// +  +  Logging
const uint32_t CAN_REAR_LOG_SPEED_ID = 0x4500; //!< Message ID for front wheel speeds
const uint32_t CAN_REAR_LOG_SUSPENSION_ID = 0x4501; //!< Message ID for front suspension
const uint32_t CAN_REAR_LOG_NEUTRAL_ID = 0x4502; //!< Message ID for logging of successful attempts at finding Neutral Gear
const uint32_t CAN_REAR_LOG_FILTER_ID = 0x4503; //!< Messsage ID for filtered clutch paddle positions
const uint32_t CAN_REAR_LOG_DUTYCYCLE_ID = 0x4504; //!< Message for servo dutycycle
const uint8_t  CAN_REAR_LOG_DLC = 4; //!< DLC of messages from rear logging node

// messages are backwards so they can be easily read with CANview.
// Pre-defined messages
uint8_t CAN_MSG_NONE[8] = "00000000"; //!< No message

// +  Front-MCU
// +  +  Brake Light
uint8_t CAN_MSG_BRAKE_ON[4] = "ONON"; //!< Message for Brake Light ON
uint8_t CAN_MSG_BRAKE_OFF[4] = "OFFO"; //!< Message for Brake Light OFF

// +  Mid-MCU
// +  +  Gear
uint8_t CAN_MSG_GEAR_UP[4] = "PUPU"; //!< Message for Gear Change UP
uint8_t CAN_MSG_GEAR_DOWN[4] = "NWOD"; //!< Message for Gear Change DOWN
uint8_t CAN_MSG_GEAR_NEUTRAL_SINGLE[4] = "LGNS"; //!< Message for Neutral Gear (single attempt)
uint8_t CAN_MSG_GEAR_NEUTRAL_REPEAT[4] = "TEPR"; //!< Message for Neutral Gear (repeat attempt)

// +  +  Launch Control
uint8_t CAN_MSG_LAUNCH[4] = "HCNL"; //!< Enable/(disable) launch control system.

// +  +  Logging
uint8_t CAN_MSG_LOG_START[4] = "TRTS"; //!< Start saving log data.
uint8_t CAN_MSG_LOG_STOP[4] = "POTS"; //!< Stop saving log data.


/*******************************************************************************
 * static function declarations
 ******************************************************************************/
static uint32_t _can_get_id(void);
static void _can_set_id(uint32_t identifier);
static void _can_set_msk(uint32_t mask);
static uint8_t _can_get_free_mob(void);
static void _can_handle_RXOK(void);
static void _can_handle_TXOK(void);

/*******************************************************************************
 * public api
 ******************************************************************************/

//! Hardware initialisation function.
/*!
 * To start using CAN run this function during the setup phase of the code.
 * The CAN hardware is reset and re configured. All interrupts except for timer
 * overflow are activated. To enable the CAN controller, see \ref can_enable.
 */
void can_init(void) {
	CANGCON = (1<<SWRES); // reset CAN
	CANTCON = 0x00; //set timing prescaler to zero

	CANBT1 = CONF_CANBT1; // set baudrate, CONF_CANBT1 defined in .h file
	CANBT2 = CONF_CANBT2; // set baudrate, CONF_CANBT2 defined in .h file
	CANBT3 = CONF_CANBT3; // set baudrate, CONF_CANBT3 defined in .h file

	CANGIE = 0xFE; // (1<<ENIT) | (1<<ENRX) | (1<<ENTX); //enable TXOK an RXOK interrupts
	CANIE1 = 0; // for compatibility
	CANIE2 = (1<<IEMOB5) | (1<<IEMOB4) | (1<<IEMOB3) | (1<<IEMOB2) | (1<<IEMOB1) | (1<<IEMOB0); // enable interrupts on all MOb

	//clear all MOb
	for (uint8_t mob_number = 0; mob_number < NBR_OF_MOB; mob_number++) {
		CANPAGE = (mob_number << MOBNB0); // select each MOb in turn

		//initiate everything to zero. IMPORTANT!!
		CANCDMOB = 0x00;
		CANSTMOB = 0x00;

		CANIDT4 = 0x00;
		CANIDT3 = 0x00;
		CANIDT2 = 0x00;
		CANIDT1 = 0x00;
		CANIDM4 = 0x00;
		CANIDM3 = 0x00;
		CANIDM2 = 0x00;
		CANIDM1 = 0x00;
	}
}

//! Setup reception of messages.
/*!
 * After the \ref can_init function, run this function to setup the reception of
 * messages.
 *
 * \param mob_id the message ID to look for.
 * \param mob_msk the mask for what bits of the ID to compare.
 * \param mob_dlc the expected number of data bytes.
 * \return the number of the MOb configured for RX.
 */
uint8_t can_setup_rx(uint32_t mob_id, uint32_t mob_msk, uint8_t mob_dlc) {

	uint8_t free_mob = _can_get_free_mob(); // first free mob
	if (free_mob == 0xFF) {
		return 0xFF; //no free mob, error
	}
	CANPAGE = free_mob << MOBNB0; // select first free MOb for use

	_can_set_id(mob_id); //id to compare against
	_can_set_msk(mob_msk); //mask for comparing id

	mob_dlc = (mob_dlc > 8) ? 8 : mob_dlc; // expected number of data bytes

	CANCDMOB = (1 << CONMOB1) | (1 << IDE) | (mob_dlc << DLC0); // configure MOb for reception of mob_dlc number of data bytes

	return free_mob; // the configured MOb
}

//! Send message.
/*!
 * Sends a message with id = \p mob_id, data = \p *mob_data and dlc = \p mob_dlc.
 * Run the function once per message. When the transmission is complete
 * CAN_ISR_TXOK() is executed, allowing for additional actions.
 *
 * \param mob_id the 29 bit message ID.
 * \param mob_data is a pointer to the data that will be sent.
 * \param mob_dlc is the number of bytes of data to send. (Maximum 8)
 * \return the number of the MOb used for TX.
 */
uint8_t can_setup_tx(uint32_t mob_id, uint8_t * mob_data, uint8_t mob_dlc) {

	uint8_t free_mob = _can_get_free_mob(); //first free MOb
	if (free_mob == 0xFF) {
		return 0xFF; //no free mob, error
	}
	CANPAGE = free_mob << MOBNB0; // select first free MOb for use

	CANSTMOB = 0x00; //clear MOb status

	_can_set_id(mob_id); // configure ID

	for (uint8_t i = mob_dlc; i > 0; i--) {
		CANMSG = mob_data[i-1]; // Set data. Reversed to send uint16_t non inverted. AVR is little endian, this way we can read the information with CANview. Arrays are sent backwards.
	}

	CANCDMOB = (1<<CONMOB0) | (1 << IDE) | (mob_dlc << DLC0); // enable transmission and set DLC

	return free_mob; // return the MOb used
}

//! Enable CAN.
/*!
 * Run this function at the end of the setup procedure. This activates the CAN bus.
 */
void can_enable(void) {
	CANGCON |= (1 << ENASTB); //enable CAN
}

//! Disable CAN.
/*!
 * Should there be a need to disable the CAN controller this function will do so.
 */
void can_disable(void) {
	CANGCON &= ~(1 << ENASTB); //disable CAN
}

//! Deconfigure reception of messages on \p mob.
/*!
 * Should a MOb configured for RX need to be freed, use this function.
 *
 * \param mob Message Object to clear.
 */
uint8_t can_free_rx(uint8_t mob) {
	if (mob >= NBR_OF_MOB) {
		return 0; // not a mob, error
	}
	CANPAGE = mob << MOBNB0;

	//reset everything to zero
	CANCDMOB = 0x00;
	CANSTMOB = 0x00;

	CANIDT4 = 0x00;
	CANIDT3 = 0x00;
	CANIDT2 = 0x00;
	CANIDT1 = 0x00;
	CANIDM4 = 0x00;
	CANIDM3 = 0x00;
	CANIDM2 = 0x00;
	CANIDM1 = 0x00;

	return 1;
}

/*******************************************************************************
 * static function definitions
 ******************************************************************************/

//! Reconstruct the message ID.
/*!
 * Rebuilds the message identifier from the CANIDTn registers.
 * \return 29 bit message ID.
 */
uint32_t _can_get_id(void) {
	return ((uint32_t)CANIDT4 >> 3) | ((uint32_t)CANIDT3 << 5) | ((uint32_t)CANIDT2 << 13) | ((uint32_t)CANIDT1 << 21);
}

//! Writes the ID of the MOb to the CANIDTn registers.
/*!
 * \param identifier the 29 bit message id.
 */
void _can_set_id(uint32_t identifier) {
	uint32_t id_v = (identifier << 3);

	CANIDT1 = *((uint8_t *) &id_v + 3);
	CANIDT2 = *((uint8_t *) &id_v + 2);
	CANIDT3 = *((uint8_t *) &id_v + 1);
	CANIDT4 = *((uint8_t *) &id_v + 0);
}

//! Writes the mask for the MOb to the CANIDMn registers.
/*!
 * \param mask the 29 bit id mask.
 */
void _can_set_msk(uint32_t mask) {
	uint32_t mask_v = (mask << 3);
	CANIDM1 = *((uint8_t *) &mask_v + 3);
	CANIDM2 = *((uint8_t *) &mask_v + 2);
	CANIDM3 = *((uint8_t *) &mask_v + 1);
	CANIDM4 = *((uint8_t *) &mask_v + 0) | (1<<RTRMSK) | (1<<IDEMSK);
}

//! First free MOb.
/*!
 * \return the first free MOb, 0xFF if no free MOb is found.
 */
uint8_t _can_get_free_mob() {
	for (uint8_t mob = 0; mob < NBR_OF_MOB; mob++) {
		if (!(CANEN2 & (1 << mob))) {
			return mob;
		}
	}
	return 0xFF;
}

/*******************************************************************************
 * Interrupt handling
 ******************************************************************************/

//! Interrupt Service Routine, CAN
/*!
 * Interrupts are handled very differently for CAN compared to the regular ones.
 * This is described at some depth in the ATmega32M1 datasheet and much has been
 * written about it on the AVRFreaks forum.
 *
 * There are only two flag registers, and each flag can be set for several
 * simultanious reasons. Therefore it is important that each possible cause is
 * tested individually. Once the interrupt has been handled the flags must be
 * cleared manually.
 *
 * Once the cause of the interrupt is clear static helper functions read out any
 * useful data from the MOb, executes extern functions to deal with the data,
 * and lastly resets the MOb for future use.
 */
ISR (CAN_INT_vect) {
	uint8_t save_CANPAGE = CANPAGE; // save CANPAGE

	CANPAGE = CANHPMOB & 0xF0; // select MOb with highest priority interrupt

	if (CANSTMOB & (1 << RXOK)) { //test for RXOK
		CANSTMOB &= ~(1 << RXOK); // clear interrupt flag
		_can_handle_RXOK(); //handle RXOK
	} else if (CANSTMOB & (1 << TXOK)) { //test for TXOK
		CANSTMOB &= ~(1 << TXOK); // clear interrupt flag
		_can_handle_TXOK(); //handle TXOK
	} else { // any other interrupt, most likely an error
		CAN_ISR_OTHER(); // extern function, handles errors
		CANSTMOB = 0x00; // clear interrupt flag, FIXME: errors not handled well
		CANGIT = 0x00; // clear general interrupts
	}

	CANPAGE = save_CANPAGE; //restore CANPAGE
}

//! Receives message.
/*!
 * Helper function for ISR receiving messages, relies on external function
 * implemented in application.
 */
void _can_handle_RXOK() {
	uint8_t mob = (CANPAGE & 0xF0) >> 4; // get mob number
	uint32_t id = _can_get_id(); // get id
	uint8_t dlc = CANCDMOB & 0x0F; // get dlc
	uint8_t data[dlc]; // create vector for data

	//read data
	for (uint8_t i = dlc; i > 0; i--) {
		data[i-1] = CANMSG; //CANMSG autoincrements, !AINC = 0. Reversed to send uint16_t non inverted. AVR is little endian, this way we can read the information with CANview. Arrays are sent backwards. inverted.
	}

	// send information to extern function in application to act on information
	CAN_ISR_RXOK(mob, id, dlc, data);

	CANCDMOB &= ~((1 << CONMOB1) | (1 << CONMOB0)); //disable MOb
	CANCDMOB |= (1 << CONMOB1); // re-enable reception
}

//! Ends transmission of message.
/*!
 * Helper function for ISR sending messages, relies on external function
 * implemented in application.
 */
void _can_handle_TXOK() {
	uint8_t mob = (CANPAGE & 0xF0) >> 4; // get mob number
	uint32_t id = _can_get_id(); // get id
	uint8_t dlc = CANCDMOB & 0x0F; // get dlc
	uint8_t data[dlc]; // create vector for data

	//read data
	for (uint8_t i = dlc; i > 0; i--) {
		data[i-1] = CANMSG; //CANMSG autoincrements, !AINC = 0. Reversed to send uint16_t non inverted. AVR is little endian, this way we can read the information with CANview. Arrays are sent backwards.
	}

	CAN_ISR_TXOK(mob, id, dlc, data); // extern function if more actions are required after TXOK
	CANCDMOB = 0x00; // clear control register
}

/*******************************************************************************
 * Extra helper functions
 ******************************************************************************/

uint8_t can_data_equals(uint8_t * data1, uint8_t * data2, uint8_t dlc) {
	for (uint8_t i = 0; i < dlc; i++) {
		if (data1[i] != data2[i]) {
			return FALSE;
		}
	}
	return TRUE;
}
