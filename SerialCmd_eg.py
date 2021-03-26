import serial as s
import time
ser = s.Serial('COM3')
ad = 0x00
ser.close()
ser.open()
print(ser.readline())
time.sleep(0.1)
def writeAt(addr,data):
    out = b'w'
    out += addr
    out += data.to_bytes(2,'big')
    #print(out)
    ser.write(out)
    time.sleep(0.005) #this delay is important

def readAt(addr):
    ser.write(b'r'+addr)
    k = ser.readline()
    return k[:2]

def read2Array(max):
    ar = []
    add=0
    while add <= int.from_bytes(max,'big'):
        ar += [readAt(add.to_bytes(1,'big'))]
        add+=4
    return ar
#name of this function is wrong for consistency 
def write2Array(max,ar):
    add = 0
    l = len(ar)
    i=0
    while add <= int.from_bytes(max,'big') and i<l:
        writeAt(add.to_bytes(1,'big'),int.from_bytes(ar[i],'big'))
        add+=4
        i+=1


def file2array(fileName):
    f = open(fileName,"rb")
    ar = []
    k = f.read(2)
    while(k != b''):
        ar += [k]
        k = f.read(2)
    return ar

def array2file(fileName,ar):
    try:
        f= open(fileName,'wb')
        for i in ar:
            f.write(i)
        f.close()
        return True
    except:
        return False
#add=0
#help int.from_bytes(b'ff','big') and int.to_bytes(1,big)
#while add <= int.from_bytes(b'\xfc','big'):
#    writeAt(add.to_bytes(1,'big'),int.from_bytes(b'ab','big'))
#    add+=4
#add=0

#while add <= int.from_bytes(b'\xfc','big'):
#    print('Rx at',str(add),readAt(add.to_bytes(1,'big')))
#    add+=4

#arf = file2array("D:\CODES\EEprom Serial\\tx.txt")
#print(arf)
#write2Array(b'\xfc',arf)


ser.close()