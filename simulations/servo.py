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
clutch_pos_break_closed = 410.
clutch_pos_break_open = 460.
CLUTCH_POS_OPEN = 485.

CLUTCH_DC_CLOSED = 3000.
clutch_dc_break_closed = 7000.
clutch_dc_break_open = 9000.
CLUTCH_DC_OPEN = 13000.
CLUTCH_DC_MAX = 20000.

clutch_factor_closed  = ((clutch_dc_break_closed - CLUTCH_DC_CLOSED))       / (clutch_pos_break_closed - CLUTCH_POS_CLOSED);
clutch_factor_mid     = ((clutch_dc_break_open   - clutch_dc_break_closed)) / (clutch_pos_break_open   - clutch_pos_break_closed)
clutch_factor_open    = ((CLUTCH_DC_OPEN         - clutch_dc_break_open))   / (CLUTCH_POS_OPEN         - clutch_pos_break_open)

time = linspace(1, 1000, 1000)

dump = ones(1000)
dump[0:100] = clutch_start
dump[100:1000] = clutch_end

filter_factor = 0.1

filtered = [dump[0]]

for i in range(999):
    filtered.append(filter_factor * dump[i+1] + (1 - filter_factor) * filtered[i])

figure(1)
clf()
grid(1)

plot(time, dump, 'k', time, filtered, 'b')

dutycycle = []

for i in range(1000):
    if filtered[i] > CLUTCH_POS_OPEN:
        dutycycle.append(CLUTCH_DC_OPEN)
    elif filtered[i] > clutch_pos_break_open:
        dutycycle.append((filtered[i] - clutch_pos_break_open)   * clutch_factor_open   + clutch_dc_break_open)
    elif filtered[i] > clutch_pos_break_closed:
        dutycycle.append((filtered[i] - clutch_pos_break_closed) * clutch_factor_mid    + clutch_dc_break_closed)
    elif filtered[i] > CLUTCH_POS_CLOSED:
        dutycycle.append((filtered[i] - CLUTCH_POS_CLOSED)       * clutch_factor_closed + CLUTCH_DC_CLOSED)
    else:
        dutycycle.append(CLUTCH_DC_CLOSED)

figure(2)
clf()
grid(1)

plot(time, dutycycle)


