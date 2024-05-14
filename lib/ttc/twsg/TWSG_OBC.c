/*****************************************************************//**
 *  @file   TWSG_OBC.c
 *  @brief
 *
 *  @author N. Lok
 *  @date   08/March/2024
 *********************************************************************/
#include "TWSG_OBC.h"

int8_t TWSG_SendBeacon( TWSG_Comms_System *_Comms, TWSG_OBC_Beacon *_Payload ) {
    /** */
    uint8_t buffer[TWSG_PRI_HDR_LEN + TWSG_OBC_BEACON_LEN];
    TWSG_SetPriHeader( _Comms, buffer, TWSG_OBC_ID, 
                       TWSG_TELEMETRY, TWSG_NO_SEC_HDR, 
                       TWSG_OBC_BEACON, TWSG_NO_PACKET_SUB_NAME );

    /** */
    memcpy( &( buffer[2] ), &_Payload->callSign, 8 );
    memcpy( &( buffer[10] ), &_Payload->rebootCounter, 2 );
    memcpy( &( buffer[12] ), &_Payload->timestamp, 4 );
    memcpy( &( buffer[16] ), &_Payload->uptime, 4 );
    buffer[20] = _Payload->MCUTemp;
    buffer[21] = _Payload->FPGATemp;
    memcpy( &( buffer[22] ), &_Payload->magnetometer, 6 );
    memcpy( &( buffer[28] ), &_Payload->gyroscope, 6 );
    memcpy( &( buffer[34] ), &_Payload->cpuCurrent, 2 );
    buffer[36] = _Payload->radioTemp;
    memcpy( &( buffer[37] ), &_Payload->epsVBat, 2 );
    memcpy( &( buffer[39] ), &_Payload->epsCurrentSolar, 2 );
    memcpy( &( buffer[41] ), &_Payload->epsCurrentConsum, 2 );
    memcpy( &( buffer[43] ), &_Payload->epsVpanel01, 2 );
    memcpy( &( buffer[45] ), &_Payload->epsVpanel02, 2 );
    memcpy( &( buffer[47] ), &_Payload->epsVpanel03, 2 );
    memcpy( &( buffer[49] ), &_Payload->epsCurrent01, 2 );
    memcpy( &( buffer[51] ), &_Payload->epsCurrent02, 2 );
    memcpy( &( buffer[53] ), &_Payload->epsCurrent03, 2 );
    memcpy( &( buffer[55] ), &_Payload->epsBatTemp, 2 );
    memcpy( &( buffer[57] ), &_Payload->satelliteErrorFlags, 2 );
    buffer[59] = _Payload->satelliteOperationStatus;
    memcpy( &( buffer[60] ), &_Payload->checksum, 2 );

    /** */
    Li2_TXDataPacket( &( _Comms->Li2 ), buffer, ( TWSG_PRI_HDR_LEN + TWSG_OBC_BEACON_LEN ) );

    /** */
    return 0;
}

int8_t TWSG_SendHandshake( TWSG_Comms_System *_Comms, TWSG_OBC_Handshake *_Payload ) {
    /** */
    uint8_t buffer[TWSG_PRI_HDR_LEN + TWSG_OBC_HANDSHAKE_LEN];
    TWSG_SetPriHeader( _Comms, buffer, TWSG_OBC_ID, 
                       TWSG_TELEMETRY, TWSG_NO_SEC_HDR, 
                       TWSG_OBC_HANDSHAKE, TWSG_NO_PACKET_SUB_NAME );

    /** */
    memcpy( &( buffer[2] ), &_Payload->callSign, 8 );
    buffer[10] = _Payload->ack;

    /** */
    Li2_TXDataPacket( &( _Comms->Li2 ), buffer, ( TWSG_PRI_HDR_LEN + TWSG_OBC_HANDSHAKE_LEN ) );

    /** */
    return 0;
}

int8_t TWSG_SendHousekeeping( TWSG_Comms_System *_Comms, TWSG_OBC_Housekeep_Payload *_Payload ) {
    /** */
    uint8_t buffer[TWSG_PRI_HDR_LEN + 0];
    TWSG_SetPriHeader( _Comms, buffer, TWSG_OBC_ID, 
                       TWSG_TELEMETRY, TWSG_NO_SEC_HDR, 
                       TWSG_OBC_HOUSEKEEP, TWSG_NO_PACKET_SUB_NAME );

    /** TO-DO */

    /** */
    Li2_TXDataPacket( &( _Comms->Li2 ), buffer, ( TWSG_PRI_HDR_LEN + 0 ) );

    /** */
    return 0;
}

int8_t TWSG_SendBuffer( TWSG_Comms_System *_Comms, TWSG_OBC_Buff_Down_Payload *_Payload ) {  
    /** */
    uint8_t buffer[TWSG_MAX_PACKET_LEN];

    /** */
    uint8_t payloadLen = TWSG_MAX_PAYLOAD_SEC_LEN;
    uint32_t dataPacketCounter = 0;
    uint8_t seqSegFlag;

    /** */
    uint32_t numOfPackets = ( uint32_t )ceil( ( double )_Payload->Telemetry.Meta.totalPacketCount / TWSG_MAX_PAYLOAD_SEC_LEN );
    if ( _Comms->config.General.debugFlag ) printf( "\nNo. of Data Packets: %d\n", numOfPackets );
    for ( uint32_t i = 0; i < ( numOfPackets + ( numOfPackets / 10 ) + 1 ); ++i ) {
        /** Send Meta Packet every 10th Packet, else normal data packet */
        if ( ( i % 10 ) == 0 ) {
            /** */
            TWSG_SetPriHeader( _Comms, buffer, TWSG_OBC_ID, 
                               TWSG_TELEMETRY, TWSG_NO_SEC_HDR, 
                               TWSG_OBC_BUFF_DOWN, TWSG_OBC_BUFF_DOWN_META );

            /** */
            buffer[2] = _Payload->Telemetry.Meta.fileType;
            buffer[3] = _Payload->Telemetry.Meta.fileHash;
            memcpy( &( buffer[4] ), &_Payload->Telemetry.Meta.totalPacketCount, 4 );

            /** */
            LI2_RTNS rtn = Li2_Rtn( Li2_TXDataPacket( &( _Comms->Li2 ), buffer, ( TWSG_PRI_HDR_LEN + TWSG_OBC_BUFF_DOWN_META_LEN ) ), "Li2_TXDataPacket", "TWSG_SendBuffer" );
            if ( rtn != LI2_OK ) return 1;
        } else {
            /** */
            if ( dataPacketCounter == 0 ) {
                seqSegFlag = TWSG_START_FLAG;
            }  else if ( dataPacketCounter == ( numOfPackets - 1 ) ) {
                payloadLen = _Payload->Telemetry.Meta.totalPacketCount % TWSG_MAX_PAYLOAD_SEC_LEN;
                seqSegFlag = TWSG_END_FLAG;
            } else {
                seqSegFlag = TWSG_CONTINUE_FLAG;
            }

            

            /** */
            TWSG_SetPriHeader( _Comms, buffer, TWSG_OBC_ID, 
                               TWSG_TELEMETRY, TWSG_SEC_HDR, 
                               TWSG_OBC_BUFF_DOWN, TWSG_OBC_BUFF_DOWN_DATA );

            /** */
            buffer[2] = ( ( uint8_t )seqSegFlag << 6 ) | (uint8_t)( dataPacketCounter >> 16 );
            buffer[3] = ( uint8_t )( dataPacketCounter >> 8 );
            buffer[4] = ( uint8_t )dataPacketCounter;

            /** Copy data to transfer */
            memcpy( &( buffer[5] ), &( _Comms->transferBuffer[dataPacketCounter * TWSG_MAX_PAYLOAD_SEC_LEN] ), payloadLen );

            if ( _Comms->config.General.debugFlag ) {
                printf( "\nPayload len: %d\n", payloadLen );
                printf( "\nPacket ID: %d\n", dataPacketCounter );
                printf( "\nPacket Pointer: %d\n", dataPacketCounter * TWSG_MAX_PAYLOAD_SEC_LEN );
                printf( "\ntest: %d %d %d %d\n", buffer[2], seqSegFlag, ( uint8_t )seqSegFlag << 6, ( uint8_t )( dataPacketCounter >> 16 ) );
            }

            /** Transmit Data */ 
            LI2_RTNS rtn = Li2_Rtn( Li2_TXDataPacket( &( _Comms->Li2 ), buffer, ( TWSG_PRI_HDR_LEN + TWSG_SEC_HDR_LEN + payloadLen ) ), "Li2_TXDataPacket", "TWSG_SendBuffer" );
            if ( rtn != LI2_OK ) return 2;

            /** Increament Packet Counter */
            dataPacketCounter++;
        }

        if ( _Comms->config.General.debugFlag ) printf( "\nPacket Count: %d\n", i );
    }

    /** */
    return 0;
}