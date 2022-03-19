# Importing Libraries
import serial
import time
import os 

arduino = serial.Serial(port='/dev/ttyACM0', baudrate=115200, timeout=.1)

def restart_seq():
    os.system('/home/matthewyk/src/audio-projects/oto-sequencer/launcher.sh')

def write_read():
    # arduino.write(bytes(x+'\t', 'utf-8'))
    # time.sleep(0.05)
    data = arduino.readline()
    if "CLICK" in str(data):
        print('click')
        restart_seq()
    return data

while True:
    value = write_read()
