/*
 * LUR7_ancomp.c - A collection of functions to setup and ease the use of the LUR7 PCB
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
#include "LUR7_ancomp.h"

void ancomp_init(void) {
	AC2CON = (1 << AC2EN) | (1 << AC2IE) | (1 << AC2M2) | (1 << AC2M1);
	DIDR0 = (1 << ADC2D) | (1 << ADC3D);
}

ISR(ANACOMP2_vect) {
	if (ACSR & (1 << AC2O)) {
		early_bod_warning_ISR();
	} else {
		early_bod_safe_ISR();
	}
}
