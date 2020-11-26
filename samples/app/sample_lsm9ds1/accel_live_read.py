from ast import Index
from matplotlib.animation import Animation
import numpy as np
import matplotlib.pyplot as plt
import matplotlib.animation as animation
import serial
from threading import Thread
from queue import Queue

################################################################################
# Variables

# Values counter
sample_count = 0

# Queue for values
q = Queue()

# Serial port
ser = serial.Serial('/dev/ttyACM0')

# Accelerometer data
accel_t = []
accel_x = []
accel_y = []
accel_z = []


fig, ax = plt.subplots()
line_x, = ax.plot(accel_t, accel_x, color = "r", lw=2)
line_y, = ax.plot(accel_t, accel_y, color = "g", lw=2)
line_z, = ax.plot(accel_t, accel_z, color = "b", lw=2)

################################################################################
# Read values from serial interface and save them in a queue.
def read_serial(_q, sample_count):
    while (1):
        # Read line from serial port (terminated by \n)
        line_raw = ser.readline()
        line = line_raw.decode('ascii')

        # Parse line and add values to queue
        try:
            # Decode binary to string
            values = line.split()
            str_x = values[0]
            str_y = values[1]
            str_z = values[2]

            # Prepent sample count
            values = [
                sample_count,
                float(str_x),
                float(str_y),
                float(str_z)
            ]
            sample_count = sample_count + 1

            # Put values in queue
            _q.put(values)

            # print queue size
            print("Queue size: " + str(_q.qsize()))

        except ValueError:
            print("split error")

        except IndexError:
            print("index error")

# Start thread for reading values from serial interface
worker = Thread(target=read_serial, args=(q, sample_count,))
worker.setDaemon(True)
worker.start()

################################################################################
# Initialize matplotlib canvas
def init():
    ax.grid()
    ax.set_ylim(-2, 2)
    ax.set_xlim(0, 200)

    line_x.set_data(accel_t, accel_x)
    line_y.set_data(accel_t, accel_y)
    line_z.set_data(accel_t, accel_z)

    return [line_x, line_y]

# Update matplotlib canvas
def update(frame):
    if (q.empty()):
        #print("nix zum updaten")
        return [line_x, line_y, line_z] #, line3]

    #print("UPDATE !!!!")

    values = q.get()

    accel_t.append(values[0])
    accel_x.append(values[1])
    accel_y.append(values[2])
    accel_z.append(values[3])

    line_x.set_data(accel_t, accel_x)
    line_y.set_data(accel_t, accel_y)
    line_z.set_data(accel_t, accel_z)

    # Move window
    xmin, xmax = ax.get_xlim()
    current_t = accel_t[len(accel_t) - 1]

    if ((current_t + 20) > xmax):
        ax.set_xlim(xmin + 1, xmax + 1)
        ax.figure.canvas.draw()

    return [line_x, line_y, line_z]

# Create matlibplot animation
anim = animation.FuncAnimation(
    fig=fig,
    func=update,
    init_func=init,
    interval=4,
    blit=True
)

# Show plot
plt.show()