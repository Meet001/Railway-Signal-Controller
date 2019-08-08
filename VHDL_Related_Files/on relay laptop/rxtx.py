import serial                    # import pySerial module
import threading

ComPort1 = serial.Serial('portname1') # open the COM Port
ComPort2 = serial.Serial('portname2') # open the COM Port

ComPort1.baudrate = 9600          # set Baud rate
ComPort1.bytesize = 8             # Number of data bits = 8
ComPort1.parity   = 'N'           # No parity
ComPort1.stopbits = 1             # Number of Stop bits = 1

ComPort2.baudrate = 9600          # set Baud rate
ComPort2.bytesize = 8             # Number of data bits = 8
ComPort2.parity   = 'N'           # No parity
ComPort2.stopbits = 1             # Number of Stop bits = 1


def mythread1():
	while 1
		data1 = ComPort1.read(1)        # Wait and read data
		print(data1) 
		data2 = ComPort2.write(data1)

def mythread2():
	while 1
		data3 = ComPort2.read(1)        # Wait and read data
		print(data3) 
		data4 = ComPort1.write(data3)

newthread1 = threading.Thread(target = mythread1)

newthread2 = threading.Thread(target = mythread2)

newthread1.start()		

newthread2.start()

newthread1.join()
newthread2.join()

ComPort1.close()
ComPort2.close()                  # Close the COM Port