/*****************************************************************//**
 *  @file   config.c
 *  @brief
 *
 *  @author N. Lok
 *  @date   08/March/2024
 *********************************************************************/
#include "config.h"

int8_t Config_Init( Config *_Config, const char *_configDir ) {
    /** */
    if ( ini_parse( _configDir, INIHandler, _Config ) != 0 ) return 1;

    /** All Good */
    return 0;
}

static int INIHandler( void *user, const char *section, const char *name, const char *value ) {
    Config *config = ( Config * )user;

#define MATCH(s, n) strcmp(section, s) == 0 && strcmp(name, n) == 0
    if ( MATCH( "General", "Debug_Flag" ) ) {
        config->General.debugFlag = ( strcmp( _strdup( value ), "true" ) == 0 ) ? true : false;
    } else if ( MATCH( "TWSG", "Segment" ) ) {
        config->TWSG.segment = ( _strdup( value ) == "Ground" ) ? 0 : 1;
    } else if ( MATCH( "TWSG", "Ground_ID" ) ) {
        config->TWSG.groundID = atoi( value );
    } else if ( MATCH( "TWSG", "Wait_Timeout" ) ) {
        config->TWSG.wait_timeout = atoi( value );
    } else if ( MATCH( "Li2", "Baud_Rate" ) ) {
        config->Li2.baudRate = atoi( value );
    } else if ( MATCH( "Li2", "Serial_Port" ) ) {
        config->Li2.serialPort = _strdup( value );
    } else if ( MATCH( "Li2", "Buff_Debug_Flag" ) ) {
        config->Li2.buffDebugFlag = ( strcmp( _strdup( value ), "true" ) == 0 ) ? true : false;
    } else if ( MATCH( "Li2", "Beacon_Status" ) ) {
        config->Li2.beaconStatus = ( strcmp( _strdup( value ), "true" ) == 0 ) ? true : false;
    } else if ( MATCH( "Li2", "Beacon_Interval" ) ) {
        config->Li2.beaconInterval = atoi( value );
    } else if ( MATCH( "TransferBuff", "Size" ) ) {
        config->TransferBuff.size = atoi( value );
    } else {
        return 0;  /* unknown section/name, error */
    }

    /** All Good */
    return 1;
}