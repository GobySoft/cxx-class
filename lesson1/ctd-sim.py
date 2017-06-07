#!/usr/bin/python

import struct
import time
import os
import select
import termios
import string
import sys



def response(x):
    return {
        "SET STARTUP NOHEADER": "",
    }.get(x, "") 

# Conductivity, Pressure, Temperature, Density, Salinity, Sound velocity
values = [[20, 5.4, 15.2, 1023.1, 15.1, 1484], 
          [21, 7.4, 15.4, 1023.4, 15.7, 1485.6],
          [31, 15.4, 15.2, 1025, 24.1, 1494.7]]

print values

master, slave = os.openpty()        
old_settings = termios.tcgetattr(master)
new_settings = termios.tcgetattr(master)   # Does this to avoid modifying a reference that also modifies old_settings
new_settings[3] = new_settings[3] & ~termios.ECHO
termios.tcsetattr(master, termios.TCSADRAIN, new_settings)

print os.ttyname(slave)
if os.path.exists("/tmp/ttyCTDSim"):
    os.remove("/tmp/ttyCTDSim")
os.symlink(os.ttyname(slave), "/tmp/ttyCTDSim")

is_sampling = False

while 1:    
    
    # read input commands
    r, w, e = select.select([master], [], [], 0)
    if master in r:
        bytes = os.read(master, 8192)
        print bytes.encode("hex")
        for line in string.split(bytes, '\r'):
            if line != "":
                print ">",line
                print "<",response(line)
                if string.strip(line) == "MONITOR":
                    is_sampling = True
                else:
                    os.write(master, response(line))
            
    # write sample data
    if is_sampling:
        for value in values:
            out = ' '.join(str(w) for w in value)+'\r\n'
            print out
            os.write(master, out)
            time.sleep(.1)
    else:
        time.sleep(.1)
            
        
