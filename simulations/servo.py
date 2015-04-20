# -*- coding: utf-8 -*-
"""
Created on Tue Apr 14 15:14:43 2015

@author: simon
"""

from  scipy import *
from  pylab import *

clutch_start = 490
clutch_end = 350

CLUTCH_POS_CLOSED = 360.
clutch_pos_break_closed = 400.
clutch_pos_break_open = 470.
CLUTCH_POS_OPEN = 485.

CLUTCH_DC_CLOSED = 3000.
clutch_dc_break_closed = 7000.
clutch_dc_break_open = 9000.
CLUTCH_DC_OPEN = 13000.
CLUTCH_DC_MAX = 20000.

clutch_factor_closed  = ((clutch_dc_break_closed - CLUTCH_DC_CLOSED))       / (clutch_pos_break_closed - CLUTCH_POS_CLOSED);
clutch_factor_mid     = ((clutch_dc_break_open   - clutch_dc_break_closed)) / (clutch_pos_break_open   - clutch_pos_break_closed)
clutch_factor_open    = ((CLUTCH_DC_OPEN         - clutch_dc_break_open))   / (CLUTCH_POS_OPEN         - clutch_pos_break_open)

servo_max = 130
servo_min = 0
servo_speed = 0.6

time_min = 0
time_max = 200
time_break = 1
time = linspace(time_min, time_max, time_max - time_min + 1)

"""
in_clutch = ones(time_max - time_min + 1)
in_clutch[0:time_break - time_min] = clutch_end
in_clutch[time_break - time_min:time_max - time_min + 1] = clutch_start
"""
in_clutch = (CLUTCH_POS_OPEN-CLUTCH_POS_CLOSED)/7*(sin(10*time/(pi))) + (CLUTCH_POS_CLOSED + CLUTCH_POS_OPEN)/2
#in_clutch[100: 110] = 490


filter_factor = .1

filtered = [in_clutch[0]]

for i in range(time_min, time_max):
    i_c = i - time_min + 1
    i_f = i-time_min
    filtered.append(filter_factor * in_clutch[i_c] + (1 - filter_factor) * filtered[i_f])

figure(1)
clf()
grid(1)
axis([time_min, time_max, CLUTCH_POS_CLOSED, CLUTCH_POS_OPEN])
plot(time, in_clutch, 'k', time, filtered, 'b')

dutycycle = []

for i in range(time_min, time_max+1):
    if filtered[i-time_min] > CLUTCH_POS_OPEN:
        dutycycle.append(CLUTCH_DC_OPEN/CLUTCH_DC_MAX)
    elif filtered[i-time_min] > clutch_pos_break_open:
        dutycycle.append(((filtered[i-time_min] - clutch_pos_break_open)   * clutch_factor_open   + clutch_dc_break_open) / CLUTCH_DC_MAX)
    elif filtered[i-time_min] > clutch_pos_break_closed:
        dutycycle.append(((filtered[i-time_min] - clutch_pos_break_closed) * clutch_factor_mid    + clutch_dc_break_closed) / CLUTCH_DC_MAX)
    elif filtered[i-time_min] > CLUTCH_POS_CLOSED:
        dutycycle.append(((filtered[i-time_min] - CLUTCH_POS_CLOSED)       * clutch_factor_closed + CLUTCH_DC_CLOSED) / CLUTCH_DC_MAX)
    else:
        dutycycle.append(CLUTCH_DC_CLOSED / CLUTCH_DC_MAX)

"""
figure(2)
clf()
grid(1)

plot(time, dutycycle)
"""

angle = list()
for i in range(time_min, time_max+1):
    if i < time_break:
        angle.append(servo_min)
    else:
        angle.append(min(angle[i-1-time_min] + servo_speed, servo_max))

given_angle = (array(dutycycle) - CLUTCH_DC_CLOSED / CLUTCH_DC_MAX) / ((CLUTCH_DC_OPEN - CLUTCH_DC_CLOSED)/CLUTCH_DC_MAX) * servo_max

figure(3)
clf()
grid(1)
axis([time_min, time_max, servo_min, servo_max])
plot(time, angle, 'k', time, given_angle, 'b')
