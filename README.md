Project Name

RFID RC232 Communication

Description
This project demonstrates communication with an RFID RC232 device using a serial port in a Windows environment. 
It sends commands to the device and retrieves data from NFC tags.

Prerequisites
- Windows operating system
- C compiler
- RFID RC232 device connected to your computer's serial port

Getting Started
1. Clone the project repository.
2. Connect the RFID RC232 device to the computer's serial port.
3. Compile the source code using a C compiler.
4. Run the executable.

Usage
1. Open the serial port: The project opens the serial port (COM13) to establish communication with the RFID RC232 device.
2. Configure the serial port: The project sets up the UART parameters (baud rate, byte size, stop bits, parity) for communication.
3. Send a command to the device: The project prompts the user to enter a command to send to the device. The available commands are described in the source code.
4. Activate the RFID RC232 device: The project sends the activation sequence for the chosen command to the RFID RC232 device.
5. Read NFC tag data: The project reads the NFC tag data from the serial port and displays the card UID, bootloader ACK, response, data, and checksum.
6. Close the serial port: The project closes the serial port after completing the communication.

Configuration
- UART_COM_PORT: The COM port to which the RFID RC232 device is connected. Modify the value of this constant to match the actual COM port.
- MAX_TAG_LENGTH: The maximum length of the NFC tag data. Increase this value if the NFC tag data exceeds the default length.

Contributing
Contributions to this project are welcome. Please follow the guidelines for contributing mentioned in the project repository.

License
This project is licensed under the MIT License.

Acknowledgments
The project is inspired by the RFID RC232 communication protocol and the requirements of working with NFC tags.
