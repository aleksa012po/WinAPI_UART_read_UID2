#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <windows.h>
#include <stdbool.h>
#include <conio.h>
#include <fileapi.h>

#define UART_COM_PORT "COM13"
#define MAX_TAG_LENGTH 16                     // BOOTLOADER ACK (4 bytes) + RESPONSE (7 bytes) + DATA (4 bytes) + CHECKSUM byte (1 byte)
#define RFID_ACTIVATION_SEQUENCE_LENGTH 7
#define CMD_HEADER 0x55
#define CMD_TRAILER 0xAA

/*
CMDs:

General purpose card related commands
GET_CARD_ID 0x13:
55 13 AA 00 00 00 F3
GET_CARD_ID_EX 0x2C:
55 2C AA 00 00 00 DA
GET_DLOGIC_CARD_TYPE 0x3C:
55 3C AA 00 00 00 CA
GET_LAST_CARD_ID_EX 0x7C:
55 7C AA 00 AA CC EC

Block manipulation commands
BLOCK_READ 0x16:
55 16 AA 05 00 00 F3
BLOCK_IN_SECTOR_READ 0x18:
55 18 AA 05 00 00 E9

USER_INTERFACE_SIGNAL 0x26:
1) 55 26 AA 00 01 01 E0
2) 55 26 AA 00 01 02 E1
3) 55 26 AA 00 01 03 E2
4) 55 26 AA 00 01 04 E3
5) 55 26 AA 00 01 05 E4
*/

int main()
{
    HANDLE hSerial;
    DCB dcbSerialParams = { 0 };
    BOOL bStatus;
    DWORD dwBytesRead;
    DWORD dwBytesWritten;
    COMMTIMEOUTS timeout;
    unsigned char NFC_Tag[MAX_TAG_LENGTH];

    // Open the serial port
    hSerial = CreateFile("\\\\.\\COM13", GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hSerial == INVALID_HANDLE_VALUE)
    {
        printf("Error opening %s\n", UART_COM_PORT);
        return 1;
    }
    else
        printf("----------------------------\nSuccessfully connected to %s\n", UART_COM_PORT);

    // Configure the serial port
    dcbSerialParams.DCBlength = sizeof(dcbSerialParams);
    bStatus = GetCommState(hSerial, &dcbSerialParams);
    if (bStatus == FALSE)
    {
        printf("Error setting up UART parameters\n");
        CloseHandle(hSerial);
        return 1;
    }
    else
        printf("----------------------------\nSuccessfully setting up UART parameters\n----------------------------\n");

    GetCommTimeouts(hSerial, &timeout);
    timeout.ReadIntervalTimeout = 500;
    timeout.ReadTotalTimeoutConstant = 500;
    timeout.ReadTotalTimeoutMultiplier = 1;

    timeout.WriteTotalTimeoutConstant = 500;
    timeout.WriteTotalTimeoutMultiplier = 1;

    SetCommTimeouts(hSerial, &timeout);

    GetCommState(hSerial, &dcbSerialParams);

    dcbSerialParams.BaudRate = CBR_115200;
    dcbSerialParams.ByteSize = 8;
    dcbSerialParams.StopBits = ONESTOPBIT;
    dcbSerialParams.Parity = NOPARITY;
    dcbSerialParams.fRtsControl = RTS_CONTROL_ENABLE;               // RTS (Request to Send)
    bStatus = SetCommState(hSerial, &dcbSerialParams);
    if (bStatus == FALSE)
    {
        printf("Failed to set serial parameters.\n");
        CloseHandle(hSerial);
        return 1;
    }
    else
            printf("****************************\n*   Opening RFID program   *\n****************************\n");
            printf("\n");

        char command[7];

        unsigned char calcChecksum(char *command, int length) {
          unsigned char checksum = 0;
          for (int i = 0; i < length; i++) {
            checksum += command[i];
          }
          return checksum;
        }

        int writeCommand(char cmd_code, char cmd_par0, char cmd_par1, char *cmdExt_, char cmdExt_len) {
          command[0] = CMD_HEADER;
          command[1] = cmd_code;
          command[2] = CMD_TRAILER;
          command[3] = cmdExt_len;
          command[4] = cmd_par0;
          command[5] = cmd_par1;
          command[6] = calcChecksum(command, cmdExt_len);

          return 0;
        }

        // Send the GET_CARD_ID or GET_CARD_ID_EX activation sequence to the RFID RC232
        //char commandChoice[7];
        printf("Please enter the right CMD to send to the device: ");
        for(int i = 0; i < 7; i ++)
        {
            scanf("%x", &command[i]);
        }
        printf("\n");

        for (int i = 0; i < 7; i ++)
        {
          bStatus = WriteFile(hSerial, (LPVOID)&command[i], RFID_ACTIVATION_SEQUENCE_LENGTH, &dwBytesWritten, NULL);
        }

        if (bStatus == FALSE || dwBytesWritten != RFID_ACTIVATION_SEQUENCE_LENGTH)
        {
            printf("Failed to send the activation sequence to the RFID RC232.\n");
            CloseHandle(hSerial);
            return 1;
        }


        // Read the NFC tag from the serial port
        bStatus = ReadFile(hSerial, NFC_Tag, MAX_TAG_LENGTH, &dwBytesRead, NULL);
        if (bStatus == FALSE)
        {
            printf("Failed to read from serial port.\n");
            CloseHandle(hSerial);
            return 1;
        }

        // Only display the message if a tag has been successfully read
        if (dwBytesRead == MAX_TAG_LENGTH)
        {
            // Print the NFC tag
            printf("Card UID is: ");
            for (int i = 0; i < dwBytesRead; i++)
            {
                printf("%02X ", NFC_Tag[i]);
                if (i == 3 || i == 10 || i == 14)
                {
                    printf("| ");
                }
            }
            printf("\n\nWhere first 4 bytes are BOOTLOADER ACK: ");
            for (int i = 0; i < 4; i++)
            {
                printf("%02X ", NFC_Tag[i]);
            }
            printf("\n\n");
            printf("Second 7 bytes are RESPONSE: ");
            for (int i = 4; i < 11; i++)
            {
                printf("%02X ", NFC_Tag[i]);
            }
            printf("\n\n");
            printf("Third 4 bytes are DATA: ");
            for (int i = 11; i < 15; i++)
            {
                printf("%02X ", NFC_Tag[i]);
            }
            printf("\n\n");
            printf("Fourth 1 byte is CHECKSUM byte: ");
            for (int i = 15; i < 16; i++)
            {
                printf("%02X ", NFC_Tag[i]);
            }
            printf("\n\n");
        }
            else
                {
                    printf("No tag detected.\n");
                }

        // Close the serial port
        CloseHandle(hSerial);

        return 0;
}
