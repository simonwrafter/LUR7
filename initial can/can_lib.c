/*
 * can_lib.c
 * 
 * Author: Simon Wrafter
 * 
 * Adaptions for Lund University Formula Student Engineering
 * 
 */

/*
 * can_lib.c
 *
 * Created: 10/18/2013 11:37:43 AM
 *  Author: Scott_Schmit

	The contents of this file were copy & pasted from the CAN Software Library
	on Atmel.com. The library was written for AT90CANxx devices, but was modified
	as an ATmegaxxM1 library.

 */

/*******************************************************************************
//! @file $RCSfile: can_lib.c,v $
//!
//! Copyright (c) 2007 Atmel.
//!
//! Use of this program is subject to Atmel's End User License Agreement.
//! Please read file license.txt for copyright notice.
//!
//! @brief This file contains the library of functions of:
//!             - CAN (Controller Array Network)
//!             - AT90CAN128/64/32
//!
//! This file can be parsed by Doxygen for automatic documentation generation.
//! This file has been validated with AVRStudio-413528/WinAVR-20070122.
//!
//! @version $Revision: 3.20 $ $Name: jtellier $
//!
//! @todo
//! @bug
*******************************************************************************/

//_____ I N C L U D E S ________________________________________________________
#include "config.h"
#include "can_lib.h"
#include "can_drv.h"

//_____ D E F I N I T I O N S __________________________________________________

//_____ F U N C T I O N S ______________________________________________________

/*------------------------------------------------------------------------------
//  @fn can_init
//!
//! CAN macro initialization. Reset the CAN peripheral, initialize the bit
//! timing, initialize all the registers mapped in SRAM to put MObs in
//! inactive state and enable the CAN macro.
//!
//! @warning The CAN macro will be enable after seen on CAN bus a receceive
//!          level as long as of an inter frame (hardware feature).
//!
//! @param  none.
//!
//! @return none.
//!
//-----------------------------------------------------------------------------*/
void can_init() {
	can_fixed_baudrate();	 // c.f. macro in "can_drv.h"
	can_clear_all_mob(); // c.f. function in "can_drv.c"
	Can_enable();		 // c.f. macro in "can_drv.h"
}

/*------------------------------------------------------------------------------
//  @fn can_cmd
//!
//! This function takes a CAN descriptor, analyses the action to do:
//! transmit, receive or abort.
//! This function returns a status (CAN_CMD_ACCEPTED or CAN_CMD_REFUSED) if
//! a MOb for Rx or Tx has been found. If no MOB has been found, the
//! application must be retry at a later date.
//! This function also updates the CAN descriptor status (MOB_PENDING or
//! MOB_NOT_REACHED) if a MOb for Rx or Tx has been found. If aborting
//! is performed, the CAN descriptor status will be set to STATUS_CLEARED.
//!
//! @param  st_cmd_t* - Can_descriptor pointer on CAN descriptor structure
//!         to select the action to do.
//!
//! @return CAN_CMD_ACCEPTED - command is accepted
//!         CAN_CMD_REFUSED  - command is refused
//!
//-----------------------------------------------------------------------------*/
uint8_t can_cmd(st_cmd_t* cmd) {
	uint8_t mob_handle;
	uint8_t cpt;
	uint32_t u32_temp;

	if (cmd->cmd == CMD_ABORT) {
		if (cmd->status == MOB_PENDING) {
			// Rx or Tx not yet performed
			Can_set_mob(cmd->handle);
			Can_mob_abort();
			Can_clear_status_mob();       // To be sure !
			cmd->handle = 0;
		}
		cmd->status = STATUS_CLEARED;
	} else  {
		mob_handle = can_get_mob_free();
		if (mob_handle != NO_MOB) {
			cmd->status = MOB_PENDING;
			cmd->handle = mob_handle;
			Can_set_mob(mob_handle);
			Can_clear_mob();
			
			switch (cmd->cmd) {
			//------------
			case CMD_TX:
				Can_set_ext_id(cmd->id.ext);
				for (cpt=0; cpt<cmd->dlc; cpt++) {
					CANMSG = *(cmd->pt_data + cpt);
				}
				if (cmd->rtr) {
					Can_set_rtr();
				} else {
					Can_clear_rtr();
				}
				Can_set_dlc(cmd->dlc);
				Can_config_tx();
				break;
			//------------
			case CMD_TX_DATA:
				Can_set_ext_id(cmd->id.ext);
				for (cpt = 0; cpt < cmd->dlc; cpt++) {
					CANMSG = *(cmd->pt_data + cpt);
				}
				cmd->rtr = 0;
				Can_clear_rtr();
				Can_set_dlc(cmd->dlc);
				Can_config_tx();
				break;
			//------------
			case CMD_TX_REMOTE:
				Can_set_ext_id(cmd->id.ext);
				cmd->rtr = 1;
				Can_set_rtr();
				Can_set_dlc(cmd->dlc);
				Can_config_tx();
				break;
			//------------
			case CMD_RX:
				u32_temp = 0;
				Can_set_ext_msk(u32_temp);
				Can_set_dlc(cmd->dlc);
				Can_clear_rtrmsk();
				Can_clear_idemsk();
				Can_config_rx();
				break;
			//------------
			case CMD_RX_DATA:
				u32_temp = 0;
				Can_set_ext_msk(u32_temp);
				Can_set_dlc(cmd->dlc);
				cmd->rtr = 0;
				Can_set_rtrmsk();
				Can_clear_rtr();
				Can_clear_idemsk();
				Can_config_rx();
				break;
			//------------
			case CMD_RX_REMOTE:
				u32_temp = 0;
				Can_set_ext_msk(u32_temp);
				Can_set_dlc(cmd->dlc);
				cmd->rtr = 1;
				Can_set_rtrmsk();
				Can_set_rtr();
				Can_clear_rplv();
				Can_clear_idemsk();
				Can_config_rx();
				break;
			//------------
			case CMD_RX_MASKED:
				Can_set_ext_id(cmd->id.ext);
				u32_temp = ~0;
				Can_set_ext_msk(u32_temp);
				Can_set_dlc(cmd->dlc);
				Can_clear_rtrmsk();
				Can_set_idemsk();
				Can_config_rx();
				break;
			//------------
			case CMD_RX_DATA_MASKED:
				Can_set_ext_id(cmd->id.ext);
				u32_temp = ~0;
				Can_set_ext_msk(u32_temp);
				Can_set_dlc(cmd->dlc);
				cmd->rtr = 0;
				Can_set_rtrmsk();
				Can_clear_rtr();
				Can_set_idemsk();
				Can_config_rx();
				break;
			//------------
			case CMD_RX_REMOTE_MASKED:
				Can_set_ext_id(cmd->id.ext);
				u32_temp = ~0;
				Can_set_ext_msk(u32_temp);
				Can_set_dlc(cmd->dlc);
				cmd->rtr = 1;
				Can_set_rtrmsk();
				Can_set_rtr();
				Can_clear_rplv();
				Can_set_idemsk();
				Can_config_rx();
				break;
			//------------
			case CMD_REPLY:
				for (cpt = 0; cpt < cmd->dlc; cpt++) {
					CANMSG = *(cmd->pt_data + cpt);
				}
				u32_temp = 0;
				Can_set_ext_msk(u32_temp);
				Can_set_dlc(cmd->dlc);
				cmd->rtr = 1;
				Can_set_rtrmsk();
				Can_set_rtr();
				Can_set_rplv();
				Can_clear_idemsk();
				Can_config_rx();
				break;
			//------------
			case CMD_REPLY_MASKED:
				Can_set_ext_id(cmd->id.ext);
				for (cpt = 0; cpt < cmd->dlc; cpt++) {
					CANMSG = *(cmd->pt_data + cpt);
				}
				u32_temp = ~0;
				Can_set_ext_msk(u32_temp);
				Can_set_dlc(cmd->dlc);
				cmd->rtr = 1;
				Can_set_rtrmsk();
				Can_set_rtr();
				Can_set_rplv();
				Can_set_idemsk();
				Can_config_rx();
				break;
			//------------
			default:
				// case CMD_NONE or not implemented command
				cmd->status = STATUS_CLEARED;
				break;
			//------------
			} // switch (cmd ...
		} // if (mob_handle ...
		else
		{
			cmd->status = MOB_NOT_REACHED;
			return CAN_CMD_REFUSED;
		}
	} // else of no CMD_ABORT
	return CAN_CMD_ACCEPTED;
}

/*------------------------------------------------------------------------------
//  @fn can_get_status
//!
//! This function allows to return if the command has been performed or not.
//! In an reception case, all the CAN message is stored in the structure.
//! This function also updates the CAN descriptor status (MOB_TX_COMPLETED,
//!  MOB_RX_COMPLETED, MOB_RX_COMPLETED_DLCW or MOB_???_ERROR).
//!
//! @param  st_cmd_t* pointer on CAN descriptor structure.
//!
//! @return CAN_STATUS_COMPLETED     - Rx or Tx is completed
//!         CAN_STATUS_NOT_COMPLETED - Rx or Tx is not completed
//!         CAN_STATUS_ERROR         - Error in configuration or in the
//!                                    CAN communication
//!
//-----------------------------------------------------------------------------*/
uint8_t can_get_status (st_cmd_t* cmd) {
	uint8_t a_status;
	uint8_t rtn_val;

	a_status = cmd->status;
	if ((a_status == STATUS_CLEARED) || (a_status == MOB_NOT_REACHED) || (a_status == MOB_DISABLE)) {
		return CAN_STATUS_ERROR;
	}
	Can_set_mob(cmd->handle);
	a_status = can_get_mob_status();

	switch (a_status) {
	//---------------
	case MOB_NOT_COMPLETED:
		// cmd->status not updated
		rtn_val = CAN_STATUS_NOT_COMPLETED;
		break;
	//---------------
	case MOB_RX_COMPLETED:
	case MOB_RX_COMPLETED_DLCW:
		cmd->dlc = Can_get_dlc();
		can_get_data(cmd->pt_data);
		cmd->rtr = Can_get_rtr();
		Can_get_ext_id(cmd->id.ext);
		// Status field of descriptor: 0x20 if Rx completed
		// Status field of descriptor: 0xA0 if Rx completed with DLCWarning
		cmd->status = a_status;
		Can_mob_abort();				// Freed the MOB
		Can_clear_status_mob(); //	 and reset MOb status
		rtn_val = CAN_STATUS_COMPLETED;
		break;
	//---------------
	case MOB_TX_COMPLETED:
		// Status field of descriptor: 0x40 if Tx completed
		cmd->status = a_status;
		Can_mob_abort();				// Freed the MOB
		Can_clear_status_mob(); //	 and reset MOb status
		rtn_val = CAN_STATUS_COMPLETED;
		break;
	//---------------
	default:
		// Status field of descriptor: (bin)000b.scfa if MOb error
		cmd->status = a_status;
		Can_mob_abort();				// Freed the MOB
		Can_clear_status_mob(); //	 and reset MOb status
		rtn_val = CAN_STATUS_ERROR;
		break;
	} // switch (a_status...
	return (rtn_val);
}

