## Simple python script that reads from an arduino 
## serial port, looking for the text 'click'
## and running a shell command if it gets 'click'
## works in collaboration with a wio terminal 
## running a script that writes click to serial
## when its joystick is clicked

import serial
import time
import os 
import subprocess


def restart_seq():
    global global_p
    if global_p != 0:
        global_p.kill()
    subprocess.run(['killall', '-9', 'oto-sequencer'])
    global_p = subprocess.Popen('oto-sequencer')

def write_read():
    # arduino.write(bytes(x+'\t', 'utf-8'))
    # time.sleep(0.05)
    
    data = arduino.readline()
    if "CLICK" in str(data):
        print('click')
        restart_seq()
    return data


arduino = serial.Serial(port='/dev/ttyACM0', baudrate=115200, timeout=.1)
global_p = 0
while True:
    value = write_read()
