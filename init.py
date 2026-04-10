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
            9600,
            timeout=2,
            write_timeout=2


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

    ser.write(b"\r")
    ser.write(bytes([byteCount]))
    for byte in writeData:
        try:
            ser.write(bytes([byte]))
            bytesWritten += 1

        except serial.SerialTimeoutException:
            print("Error: Cannot write to STM")
   
    if byteCount != bytesWritten:
        print("Error: Incomplete write to STM")

def UART2_read(ser, byteCount):
    """Default UART to read from STM 
    Args:
        ser (port): Initialised port for serial communication
    Returns:
        string: the decoded returned data
    """
    data = b''
    for i in range(byteCount + 27):
        data += ser.read(1)
    return data.decode("utf-8")


# polling serial devices

port = find_port()
ser = init_port(port)

msg = input("Please enter a message: ")
UART2_write(ser, msg)
time.sleep(1)
response = UART2_read(ser, len(msg))

print(response)

# reading message from head after loop
#time.sleep(1)

#reading = UART2_read(ser, 3)

#print(reading)

#message="\r\n"

# try:
#     while True:
#         msg = input("Please enter a message: ")+"\r\n"
#         UART2_write(ser, msg)

#         reading = UART2_read(ser, len(msg))

#         print(reading)
# except KeyboardInterrupt:
#     print("Program finished.")
#     ser.close()