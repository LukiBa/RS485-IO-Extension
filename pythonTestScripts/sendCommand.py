import serial
import keyboard
import sys
import time
import numpy as np
import threading

MESSAGE_LENGTH_BYTE = 32

cmd = np.zeros([MESSAGE_LENGTH_BYTE], dtype=np.uint8)
cmd[MESSAGE_LENGTH_BYTE-1] = 0xBB
cmd[MESSAGE_LENGTH_BYTE-2] = 0x3B

cmd[0] = 0x05  # address
cmd[1] = 0x31 # command ID

cmdAll1 = 1
cmdAll2 = 1

cmd[2] = cmdAll1 # dim1
cmd[3] = cmdAll2

cmd[4] = cmdAll1 #dim2
cmd[5] = cmdAll2

cmd[6] = cmdAll1 #dim5
cmd[7] = cmdAll2

cmd[8] = cmdAll1 #dim6
cmd[9] = cmdAll2

cmd[10] = cmdAll1 #dim7
cmd[11] = cmdAll2

cmd[12] = cmdAll1 # dim8
cmd[13] = cmdAll2

cmd[14] = cmdAll1 #dim9
cmd[15] = cmdAll2

cmd[16] = cmdAll1 #dim10
cmd[17] = cmdAll2

ser = serial.Serial("COM5",115200)
print("Start.")

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
    hexString = [hex(x) for x in cmd]
    print("Wrote: {}".format(hexString))
    if input('redo sending: r') != 'r':
        doSending = False

print("Aus und vorbei.")
