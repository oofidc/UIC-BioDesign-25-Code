import serial
import time

# Set your serial port and baudrate (change 'COM3' to your Arduino port)
ser = serial.Serial('COM10', 9600)  # e.g., 'COM3' for Windows, '/dev/ttyACM0' for Linux

with open('arduino_output_heating-1.txt', 'w') as f:
    while True:
        line = ser.readline().decode('utf-8').strip()
        print(line)  # Optional: show on screen
        f.write(line + '\n')
        f.flush()  # Write to file immediately
        time.sleep(0.5)

# To stop, press Ctrl+C in the terminal