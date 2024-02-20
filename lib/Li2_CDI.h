#ifndef LI2_CDI_H_INCLUDED
#define LI2_CDI_H_INCLUDED

// Configuration Commands
#define NO_OP_COMMAND           0x01
#define RESET_SYSTEM            0x02
#define TRANSMIT_DATA           0x03
#define RECEIVE_DATA            0x04
#define GET_TRANSCEIVER_CONFIG  0x05
#define SET_TRANSCEIVER_CONFIG  0x06
#define TELEMETRY_QUERY         0x07
#define WRITE_FLASH             0x08
#define RF_CONFIG               0x09
#define BEACON_DATA             0x10
#define BEACON_CONFIG           0x11
#define READ_FIRMWARE_REVISION  0x12
#define WRITE_OVER_AIR_KEY      0x13
#define FIRMWARE_UPDATE         0x14
#define FIRMWARE_PACKET         0x15
#define FAST_PA_SET             0x20

// Serial Baud Rates
#define BAUD_RATE_9600      0
#define BAUD_RATE_19200     1
#define BAUD_RATE_38400     2
#define BAUD_RATE_76800     3
#define BAUD_RATE_115200    4

// The Telemetry Structure
typedef struct telem_type {
    uint16_t op_counter;
    int16_t msp430_temp;
    uint8_t time_count[3];
    uint8_t rssi;
    uint32_t bytes_received;
    uint32_t bytes_transmitted;
} TELEMETRY_STRUCTURE_type;

// Opens the specified serial port, configures its timeouts, and sets its
// baud rate.  Returns a handle on success, or INVALID_HANDLE_VALUE on failure.
HANDLE OpenSerialPort( const char * comName, uint32_t baud_rate ) {
    HANDLE comPort = CreateFileA( comName, GENERIC_READ | GENERIC_WRITE, 0, NULL,
                                  OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );

    if ( comPort == INVALID_HANDLE_VALUE ) {
        fprintf( stderr, "[%s]: Invalid Handle Value?\n", comName );
        return INVALID_HANDLE_VALUE;
    }

    // Flush away any bytes previously read or written.
    BOOL success = FlushFileBuffers( comPort );
    if ( !success ) {
        fprintf( stderr, "[%s]: Failed to flush serial port?\n", comName );
        CloseHandle( comPort );
        return INVALID_HANDLE_VALUE;
    }

    // Configure read and write operations to time out after 100 ms.
    COMMTIMEOUTS timeouts = {0};
    timeouts.ReadIntervalTimeout = 0;
    timeouts.ReadTotalTimeoutConstant = 100;
    timeouts.ReadTotalTimeoutMultiplier = 0;
    timeouts.WriteTotalTimeoutConstant = 100;
    timeouts.WriteTotalTimeoutMultiplier = 0;

    success = SetCommTimeouts( comPort, &timeouts );
    if ( !success ) {
        fprintf( stderr, "[%s]: Failed to set serial timeouts?\n", comName );
        CloseHandle( comPort );
        return INVALID_HANDLE_VALUE;
    }

    // Set the baud rate and other options.
    DCB state = {0};
    state.DCBlength = sizeof( DCB );
    state.BaudRate = baud_rate;
    state.ByteSize = 8;
    state.Parity = NOPARITY;
    state.StopBits = ONESTOPBIT;

    success = SetCommState( comPort, &state );
    if ( !success ) {
        fprintf( stderr, "[%s]: Failed to set serial timeouts?\n", comName );
        CloseHandle( comPort );
        return INVALID_HANDLE_VALUE;
    }

    return comPort;
}

#endif // LI2_CDI_H_INCLUDED
