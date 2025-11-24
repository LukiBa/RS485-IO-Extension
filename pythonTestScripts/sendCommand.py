import serial
import keyboard
import sys
import time
import numpy as np
import threading

MESSAGE_LENGTH_BYTE = 32

cmd = np.zeros([MESSAGE_LENGTH_BYTE], dtype=np.uint8)
cmd[0] = 0x01  # address
cmd[1] = 0x01  # command ID

ser = serial.Serial("COM6",115200)

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
