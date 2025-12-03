import serial
import keyboard
import sys
import time
import numpy as np
import threading

MESSAGE_LENGTH_BYTE = 32

cmd = np.zeros([MESSAGE_LENGTH_BYTE], dtype=np.uint8)
cmd[0] = 0x01  # address
cmd[1] = 0x39  # command ID

cmd[2] = 0x01
cmd[3] = 100

cmd[4] = 0x01
cmd[5] = 20

cmd[6] = 0x01
cmd[7] = 30

cmd[8] = 0x01
cmd[9] = 40

cmd[10] = 0x01
cmd[11] = 50

cmd[12] = 0x01
cmd[13] = 60

cmd[14] = 0x01
cmd[15] = 70

cmd[16] = 0x01
cmd[17] = 80

ser = serial.Serial("COM3",115200)

def worker(ser):
    while True:
        if (ser.inWaiting() > 0):
            # read the bytes and convert from binary array to ASCII
            data_str = ser.read(ser.inWaiting()).decode('ascii') 
            # print the incoming string without putting a new-line
            # ('\n') automatically after every print()
            print(data_str, end='') 
            time.sleep(0.1)

worker_thread = threading.Thread(target=worker,args=(ser,),daemon=True)
worker_thread.start()

doSending = True
while doSending:
    ser.write(cmd.tobytes(order='C'))
    print("Wrote: {}".format(cmd))
    if input('redo sending: r') != 'r':
        doSending = False

print("Aus und vorbei.")
