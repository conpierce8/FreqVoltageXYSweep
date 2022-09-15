#! python3
# -*- coding: utf-8 -*-
#
# live_lockin_data.py
#
# Plot data acquired by lockin amplifier in real-time.
#
# Author:   Connor D. Pierce
# Created:  2018-06-29 13:35
# Modified: 2022-09-15 15:10:50
#
# Copyright (c) 2018-2022 Connor D. Pierce
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.
#
# SPDX-License-Identifier: MIT


"""Plot lock-in data in real-time.

Usage:

```
python live_lockin_data.py <filename>
```

where `filename` is the path to a file where FreqVoltageXYSweep is currently
saving data.
"""

import numpy as np
from matplotlib import pyplot
import matplotlib.animation as animation
import sys


freq = np.array([])
data = np.array([])

fig = pyplot.figure()
ax = pyplot.subplot(1, 1, 1)
line1, = pyplot.plot(freq, data, 'b.')

def plot_points(step):
    global freq, data
    with open(sys.argv[1], 'r') as file:
        linecount = 1
        for line in file:
            if linecount > len(freq):
                lineitems = line.split('\t')
                if len(lineitems) > 2:
                    f = float(lineitems[0])
                    a = float(lineitems[1])
                    freq = np.append(freq, f)
                    data = np.append(data, a)
                    #print("plotted some data")
            linecount += 1
    print("file length:{0}".format(linecount))
    line1.set_xdata(freq)
    line1.set_ydata(data)
    ax.set_xlim([0, np.max(freq)])
    ax.set_ylim([0, np.max(data)])
    return line1, 

ani = animation.FuncAnimation(fig, plot_points, range(0,15*60), interval=1000,
    repeat=False)
pyplot.show()
