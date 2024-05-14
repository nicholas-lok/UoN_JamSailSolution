/*****************************************************************//**
 *  @file   Li2_Serial_Gound.c
 *  @brief  
 * 
 *  @author N. Lok
 *  @date   08/March/2024
 *********************************************************************/
#include "Li2_Serial_Ground.h"

SERIAL_RTNS Serial_Init( Serial_Port **_Port, const char *_portName, uint32_t _baudRate ) {
    /** Find and Open Serial Port */
    if ( Serial_Rtn( sp_get_port_by_name( _portName, _Port ), "sp_get_port_by_name", "Serial_Init" ) != SP_OK )
        goto INIT_FAIL;
    if ( Serial_Rtn( sp_open( *_Port, SP_MODE_READ_WRITE ), "sp_open", "Serial_Init" ) != SP_OK )
        goto INIT_FAIL;
    if ( Serial_Rtn( sp_set_baudrate( *_Port, _baudRate ), "sp_set_baudrate", "Serial_Init" ) != SP_OK )
        goto INIT_FAIL;

    /** Default Settings for Most Serial Port Setups */
    if ( Serial_Rtn( sp_set_bits( *_Port, 8 ), "sp_set_bits", "Serial_Init" ) != SP_OK )
        goto INIT_FAIL;
    if ( Serial_Rtn( sp_set_parity( *_Port, SP_PARITY_NONE ), "sp_set_parity", "Serial_Init" ) != SP_OK )
        goto INIT_FAIL;
    if ( Serial_Rtn( sp_set_stopbits( *_Port, 1 ), "sp_set_stopbits", "Serial_Init" ) != SP_OK )
        goto INIT_FAIL;
    if ( Serial_Rtn( sp_set_flowcontrol( *_Port, SP_FLOWCONTROL_NONE ), "sp_set_flowcontrol", "Serial_Init" ) != SP_OK )
        goto INIT_FAIL;

    /** Flush both buffers */
    if ( Serial_Rtn( sp_flush( *_Port, SP_BUF_BOTH ), "sp_flush", "Serial_Init" ) != SP_OK )
        goto INIT_FAIL;

    /** All Good */
    return SP_OK;

INIT_FAIL:
    /** Free serial port and return error code */
    sp_free_port( *_Port );
    return SP_ERR_FAIL;
}

SERIAL_RTNS Serial_WaitTXReady( Serial_Port *_Port ) {
    uint16_t bytes;
    long start = Timer_GetTimeMS();

    /** While bytes is more than 0 */
    do {
        /** Check how many bytes waiting in TX buffer */
        bytes = Serial_TXWaiting( _Port );
        if ( bytes < 0 ) return bytes;

        /** Check if timed out */
        if ( ( Timer_GetTimeMS() - start ) == SERIAL_WRITE_TIMEOUT ) return SP_TIMEOUT;
        //if ( ( Timer_GetTimeMS() - start ) == SERIAL_WRITE_TIMEOUT ) printf( "\ntrigger" );
        //printf( "%ld ", ( Timer_GetTimeMS() - start ) );
    } while ( bytes > 0 );
    
    /** All Good */
    return SP_OK;
}

SERIAL_RTNS Serial_WaitRXReady( Serial_Port *_Port ) {
    uint16_t bytes;
    long start = Timer_GetTimeMS();

    /** While bytes is more than 0 */
    do {
        /** Check how many bytes waiting in RX buffer */
        bytes = Serial_RXWaiting( _Port );
        if ( bytes < 0 ) return bytes;
        
        /** Check if timed out */
        if ( ( Timer_GetTimeMS() - start ) == SERIAL_READ_TIMEOUT ) return SP_TIMEOUT;
        //if ( ( Timer_GetTimeMS() - start ) == SERIAL_READ_TIMEOUT ) printf( "\ntrigger1" );
        //printf( "%ld ", ( Timer_GetTimeMS() - start ) );
    } while ( bytes == 0 );

    /** All Good */
    return SP_OK;
}

SERIAL_RTNS Serial_Write( Serial_Port *_Port, uint8_t *_dataTX, uint16_t _len ) {
    return sp_blocking_write( _Port, _dataTX, _len, SERIAL_WRITE_TIMEOUT );
}

SERIAL_RTNS Serial_Read( Serial_Port *_Port, uint8_t *_dataRX, uint16_t _len ) {
    return sp_blocking_read( _Port, _dataRX, _len, SERIAL_READ_TIMEOUT );
}

SERIAL_RTNS Serial_Drain( Serial_Port *_Port ) {
    return sp_drain( _Port );
}


SERIAL_RTNS Serial_TXWaiting( Serial_Port *_Port ) {
    return sp_output_waiting( _Port );
}

SERIAL_RTNS Serial_RXWaiting( Serial_Port *_Port ) {
    return sp_input_waiting( _Port );
}

uint8_t Serial_Rtn( SERIAL_RTNS _rtn, const char *_funcName, const char *_callerName ) {
    char *errMsg;
    /** Prints different error message depending on the type of error */
    switch ( _rtn ) {
        case SP_ERR_ARG:
            fprintf( stderr, "\n[%s][F:@%s|C:@%s] Invalid argument.", Timer_GetDateTime(), _funcName, _callerName );
            break;

        case SP_ERR_FAIL:
            errMsg = sp_last_error_message();
            fprintf( stderr, "\n[%s][F:@%s|C:@%s] Failed: %s", Timer_GetDateTime(), _funcName, _callerName, errMsg );
            sp_free_error_message( errMsg );
            break;

        case SP_ERR_SUPP:
            fprintf( stderr, "\n[%s][F:@%s|C:@%s] Not supported.", Timer_GetDateTime(), _funcName, _callerName );
            break;

        case SP_ERR_MEM:
            fprintf( stderr, "\n[%s][F:@%s|C:@%s] Couldn't allocate memory.", Timer_GetDateTime(), _funcName, _callerName );
            break;

        case SP_TIMEOUT:
            fprintf( stderr, "\n[%s][F:@%s|C:@%s] Serial Port Timed Out While Waiting.", Timer_GetDateTime(), _funcName, _callerName );
            break;

        case SP_OK:
            break;

        default:
            if ( _rtn > 0 ) {
                return (uint8_t)_rtn;
            } else {
                
                fprintf( stderr, "\n[%s][F:@%s|C:@%s] Unknown Error Code: %d", Timer_GetDateTime(), _funcName, _callerName, _rtn );
                abort();
            }
            break;
    }

    /** Return error results */
    return _rtn;
}