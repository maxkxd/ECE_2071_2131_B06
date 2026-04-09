import serial
import serial.tools.list_ports
import sys # Remove later
import time

# 

# https://pythonhosted.org/pyserial/tools.html#serial.tools.list_ports.ListPortInfo
def find_port():
    """Finds the port where the STM is connected


    Returns:
        string: the location of the port
    """
   
    ports = serial.tools.list_ports.comports()


    stmManufacturer = "STMicroelectronics"
   
    for port in ports:
        # Checking manufacturer - if true then that is the port we need
        if port.manufacturer == stmManufacturer:
            return port.device
   
    print("Port not found\nExiting...")
    sys.exit()


# connecting ports
def init_port(port):
    """Initialises the port for serial communication


    Args:
        port (string): the location of the port


    Returns:
        port: Initialised port for serial communication
    """


    try:
        ser = serial.Serial(
            port,
            115200,
            timeout=5,
            write_timeout=5


            #dsrdtr=False # claude
        )


        print("Port connected...")
        #time.sleep(3)
        return ser
   
    except serial.serialutil.SerialException:
        print("Error: Port not found")
        sys.exit()




def UART2_write(ser, data):
    """Using the default UART to write to STM


    Args:
        ser (port): Initialised port for serial communication
        data (string): string data to transmit
    """

    bytesWritten = 0
    writeData = data.encode("utf-8")
    byteCount = len(writeData)

    for byte in writeData:
        try:
            ser.write(bytes([byte]))
            bytesWritten += 1
            time.sleep(0.01) #allowing time to for STM32 to receive byte


        except serial.SerialTimeoutException:
            print("Error: Cannot write to STM")
   
    if byteCount != bytesWritten:
        print("Error: Incomplete write to STM")




def UART2_read(ser, byteCount):
    """Default UART to read from STM


    Args:
        ser (port): Initialised port for serial communication
        byteCount (int): Number of bytes of return data


    Returns:
        string: the decoded returned data
    """
    data = ser.read(byteCount)  


    return data.decode()


# polling serial devices

port = find_port()
ser = init_port(port)

cmd = input("Please enter a message: ")+"\n"

UART2_write(ser, cmd)
#time.sleep(1)
#response = UART2_read(ser, 6)


#print(response)

# reading message from head after loop
#time.sleep(1)

#reading = UART2_read(ser, 3)

#print(reading)

#message="\r\n"

ser.close()