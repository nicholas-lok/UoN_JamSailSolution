/*****************************************************************//**
 *  @file   Li2_Serial.c
 *  @brief  
 * 
 *  @author N. Lok
 *  @date   08/March/2024
 *********************************************************************/
#include "Li2_Serial_Space.h"

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

SERIAL_RTNS Serial_AddEvent( Serial_Port *_Port, Serial_Event **_event, SERIAL_EVENT_TYPE _eventType ) {
    /** Allocate new event */
    if ( Serial_Rtn( sp_new_event_set( _event ), "sp_new_event_set", "Serial_AddEvent" ) != SP_OK )
        return SP_ERR_FAIL;

    /** Defines the event type */
    if ( Serial_Rtn( sp_add_port_events( *_event, _Port, _eventType ), "sp_add_new_events", "Serial_AddEvent" ) != SP_OK )
        return SP_ERR_FAIL;

    /** All Good */
    return SP_OK;
}

uint8_t Serial_Rtn( SERIAL_RTNS _rtn, const char *_funcName, const char *_callerName ) {
    char *errMsg;
    /** Prints different error message depending on the type of error */
    switch ( _rtn ) {
        case SP_ERR_ARG:
            fprintf( stderr, "\n[%s][F:@%s|C:@%s] Invalid argument.", Time_Get(), _funcName, _callerName );
            break;

        case SP_ERR_FAIL:
            errMsg = sp_last_error_message();
            fprintf( stderr, "\n[%s][F:@%s|C:@%s] Failed: %s", Time_Get(), _funcName, _callerName, errMsg );
            sp_free_error_message( errMsg );
            break;

        case SP_ERR_SUPP:
            fprintf( stderr, "\n[%s][F:@%s|C:@%s] Not supported.", Time_Get(), _funcName, _callerName );
            break;

        case SP_ERR_MEM:
            fprintf( stderr, "\n[%s][F:@%s|C:@%s] Couldn't allocate memory.", Time_Get(), _funcName, _callerName );
            break;

        case SP_OK:
            break;

        default:
            if ( _rtn > 0 ) {
                return (uint8_t)_rtn;
            } else {
                fprintf( stderr, "\n[%s][F:@%s|C:@%s] Unknown Error Code: %d", Time_Get(), _funcName, _callerName, _rtn );
            }
            break;
    }

    /** Return error results */
    return _rtn;
}