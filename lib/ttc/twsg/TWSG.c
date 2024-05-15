/*****************************************************************//**
 *  @file   TWSG.c
 *  @brief
 *
 *  @author N. Lok
 *  @date   08/March/2024
 *********************************************************************/
#include "TWSG.h"

int8_t TWSG_Init( TWSG_Comms_System *_Comms, const char *configDir ) {
    if ( _Comms->config.General.debugFlag ) printf( "->Starting Segment ...\n" ); /** Debug */

    /** */
    if ( ini_parse( configDir, TWSG_INIHandler, &_Comms->config ) < 0 ) return 1;

    /** */
    LI2_SETUP setup = { _Comms->config.General.debugFlag, _Comms->config.Li2.buffDebugFlag, false, 
                        _Comms->config.Li2.beaconStatus, _Comms->config.Li2.beaconInterval };

    /** */
    if ( Li2_Rtn( Li2_Init( &( _Comms->Li2 ), 
                              _Comms->config.Li2.serialPort, 
                              _Comms->config.Li2.baudRate, 
                              setup ), "Li2_Init", "TWSG_Init" ) != LI2_OK ) return 2;
    
    /** */
    _Comms->transferBuffer = ( uint8_t * )malloc( _Comms->config.TWSG.transferBufferSize );

    /** All good */
    if ( _Comms->config.General.debugFlag ) printf( "\n->Segment Initiated Successfully\n" );
    return 0;
}

int8_t TWSG_WaitForPacket( TWSG_Comms_System *_Comms, uint8_t _packetFilter ) {
    /** */
    long start = Timer_GetTimeMS();

    /** Check if timed out */
    while ( ( Timer_GetTimeMS() - start ) != _Comms->config.TWSG.wait_timeout ) {
        /** */
        if ( sp_input_waiting( _Comms->Li2.SerialPort ) > 0 ) {
            printf( "\n->Recieving %d Bytes ----", sp_input_waiting( _Comms->Li2.SerialPort ) ); /** Debug */

            /** Check for LI2 data receive command */
            if ( Li2_Rtn( Li2_ReceiveCDICmd( &( _Comms->Li2 ), LI2_CDI_RECEIVE_DATA ), "Li2_ReceiveCDICmd", "TWSG_WaitForPacket" ) != LI2_OK ) return 1;

            /** */
            TWSG_Pri_Header PriHeader;
            TWSG_ReadPriHeader( _Comms, &( _Comms->Li2.bufferIn[LI2_CDI_HEADER_LEN + LI2_AX25_HDR_STRUCT_LEN] ), &PriHeader );
            if ( PriHeader.packetName != _packetFilter ) return 2;

            /** All Good */
            return 0;
        }

        
    }

    return 1;
}

void TWSG_SetPriHeader( TWSG_Comms_System *_Comms, uint8_t *_buffer, TWSG_SUB_SYS_ID _subSysID, 
                        TWSG_PACKET_TYPE _packetType, TWSG_SEC_HDR_FLAG _secFlag, 
                        uint8_t _packetName, uint8_t _packetSubName ) {
    /** */
    uint8_t APID = _Comms->config.TWSG.segment ? ( ( _subSysID << 3 ) | _Comms->config.TWSG.groundID ) : ( ( _Comms->config.TWSG.groundID << 3 ) | _subSysID );

    /** */
    _buffer[0] = ( _packetType << 7 ) | ( _secFlag << 6 ) | APID;
    _buffer[1] = ( _packetName << 4 ) | _packetSubName;
}

int8_t TWSG_ReadPriHeader( TWSG_Comms_System *_Comms, uint8_t *_buffer, TWSG_Pri_Header *_PriHeader ) {
    /**  */
    _PriHeader->packetType = _buffer[0] >> 7;
    _PriHeader->SecHdrFlag = _buffer[0] >> 6;
    if ( _Comms->config.General.debugFlag ) printf( "\n\nType: %d, SecFlag: %d \n", _PriHeader->packetType, _PriHeader->SecHdrFlag );

    /**  */
    _PriHeader->APID.src = _buffer[0] >> 2;
    _PriHeader->APID.des = _buffer[0];
    if ( _Comms->config.General.debugFlag ) printf( "src: %d, des: %d \n", _PriHeader->APID.src, _PriHeader->APID.des );

    /**  */
    _PriHeader->packetName = _buffer[1] >> 4;
    _PriHeader->packetSubName = _buffer[1];
    if ( _Comms->config.General.debugFlag ) printf( "packetName: %d, packetSubName: %d \n", _PriHeader->packetName, _PriHeader->packetSubName );

    /** All Good */
    return 0;
}

int8_t TWSG_ReadSecHeader( TWSG_Comms_System *_Comms, uint8_t *_buffer, TWSG_Sec_Header *_SecHeader ) {
    /**  */
    _SecHeader->seqFlag = _buffer[0] >> 6;

    /**  Extract 2 bits (bit 7 and 6) */
    _SecHeader->packetCount = ( uint32_t )_buffer[0] << 16;
    _SecHeader->packetCount |= ( uint32_t )_buffer[1] << 8;
    _SecHeader->packetCount |= _buffer[2];

    /** All Good */
    return 0;
}

static int TWSG_INIHandler( void *user, const char *section, const char *name, const char *value ) {
    TWSG_Config *config = ( TWSG_Config * )user;

#define MATCH(s, n) strcmp(section, s) == 0 && strcmp(name, n) == 0
    if ( MATCH( "General", "Debug_Flag" ) ) {
        config->General.debugFlag = ( strcmp( _strdup( value ), "true" ) == 0 ) ? true : false;
    } else if ( MATCH( "TWSG", "Segment" ) ) {
        config->TWSG.segment = ( _strdup( value ) == "Ground" ) ? TWSG_SEG_GROUND : TWSG_SEG_SPACE;
    } else if ( MATCH( "TWSG", "Ground_ID" ) ) {
        config->TWSG.groundID = atoi( value );
    } else if ( MATCH( "TWSG", "Transfer_Buffer_Size" ) ) {
        config->TWSG.transferBufferSize = atoi( value );
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
    } else {
        return 0;  /* unknown section/name, error */
    }

    /** All Good */
    return 1;
}