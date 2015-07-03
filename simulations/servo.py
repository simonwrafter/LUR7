# -*- coding: utf-8 -*-
"""
Created on Tue Apr 14 15:14:43 2015

@author: simon
"""

from  scipy import *
from  pylab import *

clutch_start = 432
clutch_end   = 373

CLUTCH_POS_LOOSE = 427.
clutch_pos_break = 387.
CLUTCH_POS_TIGHT = 378.

CLUTCH_DC_LOOSE = 6000.
clutch_dc_break = 9500.
CLUTCH_DC_TIGHT = 13500.
CLUTCH_DC_MAX = 20000.

clutch_factor_loose  = ((clutch_dc_break - CLUTCH_DC_LOOSE)) / (clutch_pos_break - CLUTCH_POS_LOOSE)
clutch_factor_tight  = ((CLUTCH_DC_TIGHT - clutch_dc_break)) / (CLUTCH_POS_TIGHT - clutch_pos_break)

servo_max = 100
servo_min = 0
servo_speed = 0.6

time_min = 0
time_max = 500
time_break = 1
time = linspace(time_min, time_max, time_max - time_min + 1)

"""
in_clutch = ones(time_max - time_min + 1)
in_clutch[0:time_break - time_min] = clutch_end
in_clutch[time_break - time_min:time_max - time_min + 1] = clutch_start
"""
in_clutch = (clutch_start-clutch_end)/2*(sin(1*time/(10*pi))) + (clutch_end + clutch_start)/2
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
axis([time_min, time_max, clutch_end, clutch_start])
plot(time, in_clutch, 'k', time, filtered, 'b')

dutycycle = []

for i in range(time_min, time_max+1):
    if filtered[i-time_min] > CLUTCH_POS_LOOSE:
        dutycycle.append(CLUTCH_DC_LOOSE / CLUTCH_DC_MAX)
    elif filtered[i-time_min] > clutch_pos_break:
        dutycycle.append(((filtered[i-time_min] - CLUTCH_POS_LOOSE)       * clutch_factor_loose + CLUTCH_DC_LOOSE) / CLUTCH_DC_MAX)
    elif filtered[i-time_min] > CLUTCH_POS_TIGHT:
        dutycycle.append(((filtered[i-time_min] - clutch_pos_break)   * clutch_factor_tight   + clutch_dc_break) / CLUTCH_DC_MAX)
    else:
        dutycycle.append(CLUTCH_DC_TIGHT/CLUTCH_DC_MAX)


figure(2)
clf()
grid(1)

plot(time, dutycycle)


angle = list()
for i in range(time_min, time_max+1):
    if i < time_break:
        angle.append(servo_min)
    else:
        angle.append(min(angle[i-1-time_min] + servo_speed, servo_max))

given_angle = (array(dutycycle) - CLUTCH_DC_LOOSE / CLUTCH_DC_MAX) / ((CLUTCH_DC_TIGHT - CLUTCH_DC_LOOSE)/CLUTCH_DC_MAX) * servo_max

figure(3)
clf()
grid(1)
axis([time_min, time_max, servo_min, servo_max])
plot(time, angle, 'k', time, given_angle, 'b')

figure(4)
clf()
grid(1)
#axis([])
plot([0, abs(CLUTCH_POS_LOOSE - clutch_pos_break) / abs(CLUTCH_POS_LOOSE - CLUTCH_POS_TIGHT), 1],
     [0, abs(clutch_dc_break - CLUTCH_DC_LOOSE) / abs(CLUTCH_DC_TIGHT - CLUTCH_DC_LOOSE), 1])
