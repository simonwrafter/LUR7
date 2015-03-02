/*
 * can_lib.h - A collection of functions to setup and ease the use of the LUR7 PCB
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

#ifndef _LUR7_CAN_LIB_H_
#define _LUR7_CAN_LIB_H_

// - Baud rate definitions
#ifndef F_CPU
#  error  F_CPU not defined in LUR7.h
#endif
// ----------
#ifndef CAN_BAUDRATE
#  error  CAN_BAUDRATE not defined in LUR7.h
#endif
// ----------
#if F_CPU == 16000000             //!< Fclkio = 16 MHz, Tclkio = 62.5 ns
#   if   CAN_BAUDRATE == 100       //!< -- 100Kb/s, 16x Tscl, sampling at 75%
#       define CONF_CANBT1  0x12       // Tscl  = 10x Tclkio = 625 ns
#       define CONF_CANBT2  0x0C       // Tsync = 1x Tscl, Tprs = 7x Tscl, Tsjw = 1x Tscl
#       define CONF_CANBT3  0x37       // Tpsh1 = 4x Tscl, Tpsh2 = 4x Tscl, 3 sample points
#   elif CAN_BAUDRATE == 125       //!< -- 125Kb/s, 16x Tscl, sampling at 75%
#       define CONF_CANBT1  0x0E       // Tscl  = 8x Tclkio = 500 ns
#       define CONF_CANBT2  0x0C       // Tsync = 1x Tscl, Tprs = 7x Tscl, Tsjw = 1x Tscl
#       define CONF_CANBT3  0x37       // Tpsh1 = 4x Tscl, Tpsh2 = 4x Tscl, 3 sample points
#   elif CAN_BAUDRATE == 200       //!< -- 200Kb/s, 16x Tscl, sampling at 75%
#       define CONF_CANBT1  0x08       // Tscl  = 5x Tclkio = 312.5 ns
#       define CONF_CANBT2  0x0C       // Tsync = 1x Tscl, Tprs = 7x Tscl, Tsjw = 1x Tscl
#       define CONF_CANBT3  0x37       // Tpsh1 = 4x Tscl, Tpsh2 = 4x Tscl, 3 sample points
#   elif CAN_BAUDRATE == 250       //!< -- 250Kb/s, 16x Tscl, sampling at 75%
#       define CONF_CANBT1  0x06       // Tscl  = 4x Tclkio = 250 ns
#       define CONF_CANBT2  0x0C       // Tsync = 1x Tscl, Tprs = 7x Tscl, Tsjw = 1x Tscl
#       define CONF_CANBT3  0x37       // Tpsh1 = 4x Tscl, Tpsh2 = 4x Tscl, 3 sample points
#   elif CAN_BAUDRATE == 500       //!< -- 500Kb/s, 8x Tscl, sampling at 75%
#       define CONF_CANBT1  0x06       // Tscl = 4x Tclkio = 250 ns
#       define CONF_CANBT2  0x04       // Tsync = 1x Tscl, Tprs = 3x Tscl, Tsjw = 1x Tscl
#       define CONF_CANBT3  0x13       // Tpsh1 = 2x Tscl, Tpsh2 = 2x Tscl, 3 sample points
#   elif CAN_BAUDRATE == 1000      //!< -- 1 Mb/s, 8x Tscl, sampling at 75%
#       define CONF_CANBT1  0x02       // Tscl  = 2x Tclkio = 125 ns
#       define CONF_CANBT2  0x04       // Tsync = 1x Tscl, Tprs = 3x Tscl, Tsjw = 1x Tscl
#       define CONF_CANBT3  0x13       // Tpsh1 = 2x Tscl, Tpsh2 = 2x Tscl, 3 sample points
#   else
#       error This CAN_BAUDRATE value is not in "LUR7_can.h" file
#   endif

#elif F_CPU == 8000000              //!< Fclkio = 8 MHz, Tclkio = 125 ns
#   if   CAN_BAUDRATE == 100       //!< -- 100Kb/s, 16x Tscl, sampling at 75%
#       define CONF_CANBT1  0x08       // Tscl  = 5x Tclkio = 625 ns
#       define CONF_CANBT2  0x0C       // Tsync = 1x Tscl, Tprs = 7x Tscl, Tsjw = 1x Tscl
#       define CONF_CANBT3  0x37       // Tpsh1 = 4x Tscl, Tpsh2 = 4x Tscl, 3 sample points
#   elif CAN_BAUDRATE == 125       //!< -- 125Kb/s, 16x Tscl, sampling at 75%
#       define CONF_CANBT1  0x06       // Tscl  = 4x Tclkio = 500 ns
#       define CONF_CANBT2  0x0C       // Tsync = 1x Tscl, Tprs = 7x Tscl, Tsjw = 1x Tscl
#       define CONF_CANBT3  0x37       // Tpsh1 = 4x Tscl, Tpsh2 = 4x Tscl, 3 sample points
#   elif CAN_BAUDRATE == 200       //!< -- 200Kb/s, 20x Tscl, sampling at 75%
#       define CONF_CANBT1  0x02       // Tscl  = 2x Tclkio = 250 ns
#       define CONF_CANBT2  0x0E       // Tsync = 1x Tscl, Tprs = 8x Tscl, Tsjw = 1x Tscl
#       define CONF_CANBT3  0x4B       // Tpsh1 = 6x Tscl, Tpsh2 = 5x Tscl, 3 sample points
#   elif CAN_BAUDRATE == 250       //!< -- 250Kb/s, 16x Tscl, sampling at 75%
#       define CONF_CANBT1  0x02       // Tscl  = 2x Tclkio = 250 ns
#       define CONF_CANBT2  0x0C       // Tsync = 1x Tscl, Tprs = 7x Tscl, Tsjw = 1x Tscl
#       define CONF_CANBT3  0x37       // Tpsh1 = 4x Tscl, Tpsh2 = 4x Tscl, 3 sample points
#   elif CAN_BAUDRATE == 500       //!< -- 500Kb/s, 8x Tscl, sampling at 75%
#       define CONF_CANBT1  0x02       // Tscl  = 2x Tclkio = 250 ns
#       define CONF_CANBT2  0x04       // Tsync = 1x Tscl, Tprs = 3x Tscl, Tsjw = 1x Tscl
#       define CONF_CANBT3  0x13       // Tpsh1 = 2x Tscl, Tpsh2 = 2x Tscl, 3 sample points
#   elif CAN_BAUDRATE == 1000      //!< -- 1 Mb/s, 8x Tscl, sampling at 75%
#       define CONF_CANBT1  0x00       // Tscl  = 1x Tclkio = 125 ns
#       define CONF_CANBT2  0x04       // Tsync = 1x Tscl, Tprs = 3x Tscl, Tsjw = 1x Tscl
#       define CONF_CANBT3  0x13       // Tpsh1 = 2x Tscl, Tpsh2 = 2x Tscl, 3 sample points
#   else
#       error This CAN_BAUDRATE value is not in "LUR7_can.h" file
#   endif

#else
#   error This F_CPU value is not in "LUR7_can.h" file
#endif

// - system definitions
#define NBR_OF_MOB 6

// - function declarations

/*******************************************************************************
 * public api
 ******************************************************************************/
void can_init(void);
uint8_t can_setup_rx(uint32_t mob_id, uint32_t mob_msk, uint8_t mob_dlc);
uint8_t can_setup_tx(uint32_t mob_id, uint8_t * mob_data, uint8_t mob_dlc);
void can_enable(void);
void can_disable(void);

/*******************************************************************************
 * static functions
 ******************************************************************************/
uint32_t _can_get_id(void);
void _can_set_id(uint32_t identifier);
void _can_set_msk(uint32_t mask);
uint8_t _can_get_free_mob(void);
void _can_handle_RXOK(void);
void _can_handle_TXOK(void);

/*******************************************************************************
 * extern functions
 ******************************************************************************/

extern void CAN_ISR_RXOK(uint32_t id, uint8_t dlc, uint8_t * data);
extern void CAN_ISR_TXOK(uint32_t id, uint8_t dlc, uint8_t * data);
extern void CAN_ISR_OTHER(void);

#endif // _LUR7_CAN_LIB_H_
