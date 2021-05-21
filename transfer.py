from time import sleep

import serial
import socketio

from ip import ip_address, port


ser = serial.Serial("/dev/ttyACM1", 9600)
sio = socketio.Client()
sio.connect(f'http://{ip_address}:{port}')

sleep(1)

while True:
     cc = str(ser.readline())
     print(cc[2:][:-5])
     sio.emit('add', cc[2:][:-5])
     sleep(0.002) # 0.005
