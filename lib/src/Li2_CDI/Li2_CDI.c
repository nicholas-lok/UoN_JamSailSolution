#include "../../include/Li2_CDI.h"

// Opens the specified serial port, configures its timeouts, and sets its
// baud rate.  Returns a handle on success, or INVALID_HANDLE_VALUE on failure.
LI2_RETURNS Serial_Init( Serial_Port **_Port, const char *_portName, uint32_t _baudRate ) {
    // Find and Open Serial Port
    if (Serial_Check(sp_get_port_by_name(_portName, _Port), "Serial_Init") != SP_OK)
        goto INIT_FAIL;
    if ( Serial_Check( sp_open( *_Port, SP_MODE_READ_WRITE ), "Serial_Init" ) != SP_OK )
        goto INIT_FAIL;
    if ( Serial_Check( sp_set_baudrate( *_Port, _baudRate ), "Serial_Init") != SP_OK )
        goto INIT_FAIL;

    // Default Settings for Most Serial Port Setups
    if ( Serial_Check( sp_set_bits( *_Port, 8 ), "Serial_Init" ) != SP_OK )
        goto INIT_FAIL;
    if ( Serial_Check( sp_set_parity( *_Port, SP_PARITY_NONE ), "Serial_Init" ) != SP_OK )
        goto INIT_FAIL;
    if ( Serial_Check( sp_set_stopbits( *_Port, 1 ), "Serial_Init" ) != SP_OK )
        goto INIT_FAIL;
    if ( Serial_Check( sp_set_flowcontrol( *_Port, SP_FLOWCONTROL_NONE ), "Serial_Init" ) != SP_OK )
        goto INIT_FAIL;

    return LI2_OK;

INIT_FAIL:
    sp_free_port( *_Port );
    return LI2_SERIAL_INIT_FAIL;
}

LI2_RETURNS Li2_Init( Li2_Radio *_Li2, const char *_serialPortName, uint32_t _serialBaudRate ) {
    printf( "Start Lithium Initialization\n" ); // Debug

    if ( Li2_Check( Serial_Init( &(_Li2->SerialPort), _serialPortName, _serialBaudRate ), "Li2_Init" ) != LI2_OK ) {
        return LI2_INIT_FAIL;
    }

    if ( Li2_Check( Li2_SendCDICmd( _Li2, CDI_NO_OP, NULL, 0 ), "Li2_Init" ) != LI2_OK ) {
        return LI2_INIT_FAIL;
    }

    if ( Li2_Check( Li2_ReadCDIAck( _Li2, CDI_NO_OP ), "Li2_Init" ) != LI2_OK ) {
        return LI2_INIT_FAIL;
    }

    return LI2_OK;
}

LI2_RETURNS  Li2_SendCDICmd( Li2_Radio* _Li2, CDI_CMDS _cmd, const uint8_t *_payload, SIZE_T _payloadLen ) {
    switch ( _cmd ) {
        case CDI_TRANSMIT_DATA:
        case CDI_SET_TRANSCEIVER_CONFIG:
        case CDI_RF_CONFIG:
        case CDI_BEACON_DATA:
        case CDI_BEACON_CONFIG:
            if ( _payloadLen > CDI_MAX_PACKET_LEN )
                return LI2_INVALID_CDI_CMD;
            break;

        case CDI_WRITE_OVER_AIR_KEY:
        case CDI_WRITE_FLASH:
        case CDI_FIRMWARE_UPDATE:
            if ( _payloadLen != 16 )
                return LI2_INVALID_CDI_CMD;
            break;

        case CDI_FAST_PA_SET:
            if ( _payloadLen != 1 )
                return LI2_INVALID_CDI_CMD;
            break;

        default:
            if ( _payloadLen != 0 )
                return LI2_INVALID_CDI_CMD;
    }

    _Li2->bufferOut[0] = CDI_SYNC_0;
    _Li2->bufferOut[1] = CDI_SYNC_1;
    _Li2->bufferOut[2] = CDI_I_Direction;
    _Li2->bufferOut[3] = _cmd;
    _Li2->bufferOut[4] = 0x00;
    _Li2->bufferOut[5] = (uint8_t)_payloadLen;

    SIZE_T sendLen = 8;

    Li2_CalcCDIChecksums( &(_Li2->bufferOut[2]), 4, &(_Li2->bufferOut[6]) );

    if ( _payloadLen > 0 ) {
        for ( auto i = 0; i < _payloadLen; i++ ) {
            _Li2->bufferOut[8 + i] = _payload[i];
        }

        Li2_CalcCDIChecksums( &(_Li2->bufferOut[2]), ( 6 + _payloadLen ), &(_Li2->bufferOut[8 + _payloadLen]) );
        
        sendLen += ( _payloadLen + 2 );
    }

    return Li2_Check( Li2_RawSend( _Li2, _Li2->bufferOut, sendLen ), "Li2_SendCDICmd" );
}

LI2_RETURNS Li2_RawSend( const Li2_Radio* _Li2, const uint8_t *_dataTX, SIZE_T _len ) {
    if ( Serial_Check( sp_blocking_write( _Li2->SerialPort, _dataTX, 8, SERIAL_WRITE_TIMEOUT ), "Li2_RawSend" ) != SP_OK )
        return LI2_SERIAL_WRITE_FAIL;
    if ( Serial_Check( sp_drain( _Li2->SerialPort ), "Li2_RawSend" ) != SP_OK )
        return LI2_SERIAL_WRITE_FAIL;

    return LI2_OK;
}

LI2_RETURNS Li2_ReadCDIAck( Li2_Radio* _Li2, CDI_CMDS _cmd ) {

    int8_t check = Serial_Check( sp_blocking_read( _Li2->SerialPort, _Li2->bufferIn, 8, SERIAL_READ_TIMEOUT ), "Li2_ReadCDIAck" );
    if ( check < 0 )
        return LI2_SERIAL_READ_FAIL;
    else if ( check != 8 )
        return LI2_INVALID_READ;

    if ( _Li2->bufferIn[3] != _cmd ) {
        return LI2_INVALID_ACK; // Wrong ACK
    }

    if ( ( ( (uint16_t)(_Li2->bufferIn[4]) << 8 ) | _Li2->bufferIn[5] ) == CDI_ACK ) {
        return LI2_OK; // Good
    } else if ( ( ( (uint16_t)(_Li2->bufferIn[4]) << 8 ) | _Li2->bufferIn[5] ) == CDI_NACK ) {
        return LI2_NACK_ERR; // NACK
    }

    return LI2_INVALID_ACK; // Invalid Response
}

/** @brief   Reads bytes from the serial port.
 *
 * @param[in]       serialPort - Handle for serial port to read from
 * @param[out]      buffer - Pointer for buffer to store bytes read
 * @param[in]       sizeBytes - The number of bytes to read
 *
 * @return          Returns after all the desired bytes have been read, or if there is a
 *                  timeout or other error. Returns the number of Bytes read from port or
 *                  -1 if there was an error reading.
 */
void Li2_CalcCDIChecksums( const uint8_t* data, uint16_t len, uint8_t* CK ) {
    CK[0] = 0;
    CK[1] = 0;

    for ( auto i = 0; i < len; i++ ) {
        CK[0] = CK[0] + data[i];
        CK[1] = CK[1] + CK[0];
    }
}

/* Helper function for error handling. */
int8_t Serial_Check( SERIAL_RETURNS _result, const char *_caller ) {
    char *errMsg;

    switch ( _result ) {
        case SP_ERR_ARG:
            fprintf( stderr, "[Serial Error][@%s] Invalid argument.\n", _caller );
            break;

        case SP_ERR_FAIL:
            errMsg = sp_last_error_message();
            fprintf( stderr, "[Serial Error][@%s] Failed: %s\n", _caller, errMsg );
            sp_free_error_message( errMsg );
            break;

        case SP_ERR_SUPP:
            fprintf( stderr, "[Serial Error][@%s] Not supported.\n", _caller );
            break;

        case SP_ERR_MEM:
            fprintf( stderr, "[Serial Error][@%s] Couldn't allocate memory.\n", _caller );
            break;

        case SP_OK:
        default:
            break;    
    }

    return _result;
}

/* Helper function for error handling. */
int8_t Li2_Check( LI2_RETURNS _result, const char* _caller ) {
    switch ( _result ) {
        case LI2_INIT_FAIL:
            fprintf(stderr, "[Li2 Error][@%s] Couldn't initialise Radio.\n", _caller);
            break;

        case LI2_SERIAL_INIT_FAIL:
            fprintf(stderr, "[Li2 Error][@%s] Couldn't initialise the Serial Port.\n", _caller);
            break;

        case LI2_SERIAL_WRITE_FAIL:
            fprintf(stderr, "[Li2 Error][@%s] Couldn't write to the radio\n", _caller );
            break;

        case LI2_SERIAL_READ_FAIL:
            fprintf(stderr, "[Li2 Error][@%s] Couldn't read from the radio\n", _caller);
            break;

        case LI2_INVALID_CDI_CMD:
            fprintf(stderr, "[Li2 Error][@%s] Invalid CDI command usage.\n", _caller);
            break;
        
        case LI2_INVALID_READ:
            fprintf(stderr, "[Li2 Error][@%s] Radio responded with an invalid amount of bytes for an ACK.\n", _caller);
            break;

        case LI2_INVALID_ACK:
            fprintf(stderr, "[Li2 Error][@%s] Radio responded with invalid ACK.\n", _caller);
            break;

        case LI2_NACK_ERR:
            fprintf(stderr, "[Li2 Error][@%s] Radio responded with NACK.\n", _caller);
            break;

        case LI2_OK:
        default:
            break;
    }

    return _result;
}
