import numpy as np
from matplotlib import pyplot
import matplotlib.animation as animation
import sys

#20180629/midstiffness/long time const/outputSpacer_01.txt

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