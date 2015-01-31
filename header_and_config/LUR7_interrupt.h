/*
 * / LUR7_interrupt.h - A collection of functions to setup and ease the use of the LUR7 PCB
 * / Copyright (C) 2014  Simon Wrafter <simon.wrafter@gmail.com>
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

#ifndef _LUR7_INTERRUPT_H_
#define _LUR7_INTERRUPT_H_

//interrupt vectors
//   external interrupts

#define INT_IN5_vect INT3_vect //PC0 - IN5
#define INT_IN8_vect INT2_vect //PB5 - IN8
#define INT_IN9_vect INT1_vect //PB2 - IN9

//   pin change interrupts
#define PCINT_IN1_vect PCINT2_vect //PD3
#define PCINT_IN2_vect PCINT2_vect //PD2
#define PCINT_IN3_vect PCINT2_vect //PD1
#define PCINT_IN4_vect PCINT0_vect //PB7
#define PCINT_IN5_vect PCINT1_vect //PC0
#define PCINT_IN6_vect PCINT0_vect //PB6
#define PCINT_IN7_vect PCINT2_vect //PD0
#define PCINT_IN8_vect PCINT0_vect //PB5
#define PCINT_IN9_vect PCINT0_vect //PB2

//interrupt controls
//   external interrupts
#define ISC_IN5_0 ISC30
#define ISC_IN5_1 ISC31
#define ISC_IN8_0 ISC20
#define ISC_IN8_1 ISC21
#define ISC_IN9_0 ISC10
#define ISC_IN9_1 ISC11

//   pin change interrupts
#define PCIE_IN1 PCIE2
#define PCIE_IN2 PCIE2
#define PCIE_IN3 PCIE2
#define PCIE_IN4 PCIE0
#define PCIE_IN5 PCIE1
#define PCIE_IN6 PCIE0
#define PCIE_IN7 PCIE2
#define PCIE_IN8 PCIE0
#define PCIE_IN9 PCIE0

//interrupt mask
//   external interrupts
#define INT_IN5 INT3
#define INT_IN8 INT2
#define INT_IN9 INT1

//   pin change interrupts
#define PCINT_IN1 PCINT19 //PD3
#define PCINT_IN2 PCINT18 //PD2
#define PCINT_IN3 PCINT17 //PD1
#define PCINT_IN4 PCINT7  //PB7
#define PCINT_IN5 PCINT8  //PC0
#define PCINT_IN6 PCINT6  //PB6
#define PCINT_IN7 PCINT16 //PD0
#define PCINT_IN8 PCINT5  //PB5
#define PCINT_IN9 PCINT2  //PB2

extern void pcISR_in1(void);
extern void pcISR_in2(void);
extern void pcISR_in3(void);
extern void pcISR_in4(void);
extern void pcISR_in5(void);
extern void pcISR_in6(void);
extern void pcISR_in7(void);
extern void pcISR_in8(void);
extern void pcISR_in9(void);

//EXTERNAL INTERRUPTS
uint8_t ext_int_on(uint8_t, uint8_t, uint8_t);
uint8_t ext_int_off(uint8_t);

//PIN CHANGE INTERRUPTS
void pc_int_on(uint8_t);
void pc_int_off(uint8_t);
void interrupts_on(void);
void interrupts_off(void);


#endif // _LUR7_INTERRUPT_H_