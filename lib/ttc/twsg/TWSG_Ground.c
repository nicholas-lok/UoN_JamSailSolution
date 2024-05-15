/*****************************************************************//**
 *  @file   TWSG_Ground.h
 *  @brief
 *
 *  @author N. Lok
 *  @date   08/March/2024
 *********************************************************************/
#include "TWSG_Ground.h"

int8_t TWSG_GetHandshake( TWSG_Comms_System *_Comms ) {
    /** */
    uint8_t buffer[TWSG_PRI_HDR_LEN];
    TWSG_SetPriHeader( _Comms, buffer, TWSG_OBC_ID, 
                       TWSG_TELECMD, TWSG_NO_SEC_HDR, TWSG_OBC_HANDSHAKE, TWSG_NO_PACKET_SUB_NAME );

    /** */
    Li2_TXDataPacket( &( _Comms->Li2 ), buffer, TWSG_PRI_HDR_LEN );

    return 0;
}

int8_t TWSG_GetHousekeeping( TWSG_Comms_System *_Comms ) {
    /** */
    uint8_t buffer[TWSG_PRI_HDR_LEN];
    TWSG_SetPriHeader( _Comms, buffer, TWSG_OBC_ID, 
                       TWSG_TELECMD, TWSG_NO_SEC_HDR, TWSG_OBC_HOUSEKEEP, TWSG_NO_PACKET_SUB_NAME );

    /** */
    Li2_TXDataPacket( &( _Comms->Li2 ), buffer, TWSG_PRI_HDR_LEN );

    return 0;
}

int8_t TWSG_StartBuffDown( TWSG_Comms_System *_Comms, TWSG_OBC_Buff_Down_Payload *_Payload ) {
    /** */
    uint8_t buffer[TWSG_PRI_HDR_LEN + TWSG_OBC_BUFF_DOWN_INIT_LEN];
    TWSG_SetPriHeader( _Comms, buffer, TWSG_OBC_ID, 
                       TWSG_TELECMD, TWSG_NO_SEC_HDR, TWSG_OBC_BUFF_DOWN, TWSG_NO_PACKET_SUB_NAME );

    /** */
    buffer[2] = _Payload->Telecmd.startPacketID >> 16;
    buffer[3] = _Payload->Telecmd.startPacketID >> 8;
    buffer[4] = _Payload->Telecmd.startPacketID;
    buffer[5] = _Payload->Telecmd.numOfPackets >> 16;
    buffer[6] = _Payload->Telecmd.numOfPackets >> 8;
    buffer[7] = _Payload->Telecmd.numOfPackets;

    /** */
    Li2_TXDataPacket( &( _Comms->Li2 ), buffer, TWSG_PRI_HDR_LEN + TWSG_OBC_BUFF_DOWN_INIT_LEN );

    return 0;
}

int8_t TWSG_ReceiveBuffDown( TWSG_Comms_System *_Comms, TWSG_OBC_Buff_Down_Payload *_Payload ) {
    /** */
    if ( TWSG_WaitForPacket( _Comms, TWSG_OBC_BUFF_DOWN ) != 0 ) return 1;

    memcpy( &_Comms->transferBuff->dataInBuffSize, &_Comms->Li2.bufferIn[LI2_CDI_HEADER_LEN + TWSG_PRI_HDR_LEN + LI2_AX25_HDR_STRUCT_LEN + 2], 4 );

    uint32_t dataCount = 0;
    uint32_t packetCount = 1;
    uint32_t totalPacketCount = ( uint32_t )ceil( _Comms->transferBuff->dataInBuffSize / TWSG_MAX_PAYLOAD_SEC_LEN );
    totalPacketCount += totalPacketCount / 10;

    if ( _Comms->config.General.debugFlag ) {
        printf( "\nNo. of Bytes: %d", _Comms->transferBuff->dataInBuffSize );
        printf( "\nTotal Packets: %d", totalPacketCount );
    }

    do {
        if ( TWSG_WaitForPacket( _Comms, TWSG_OBC_BUFF_DOWN ) == 2 ) printf( "\nbaddd1" );

        if ( _Comms->config.General.debugFlag ) {
            printf( "\nPacket ID: %d\n", packetCount++ );
            printf( "\nSub-Name: %d\n", ( _Comms->Li2.bufferIn[8 + 16 + 1] & 0xF ) );
            printf( "\nSeq Flag: %d\n", ( _Comms->Li2.bufferIn[8 + 16 + 2] >> 6 ) );
        }

        if ( ( _Comms->Li2.bufferIn[8 + 16 + 1] & 0xF ) == TWSG_OBC_BUFF_DOWN_DATA ) {
            uint16_t payloadLen = ( ( ( uint16_t )( _Comms->Li2.bufferIn[4] ) << 8 ) | _Comms->Li2.bufferIn[5] );
            printf( "\nlen: %d\n", payloadLen - 16 - 5 );
            printf( "\nstart: %02x\n", _Comms->Li2.bufferIn[8 + 16 + 5] );
            memcpy( &_Comms->transferBuff->buff[dataCount++ * TWSG_MAX_PAYLOAD_SEC_LEN], &_Comms->Li2.bufferIn[8 + 16 + 5], payloadLen - 16 - 5);
        }
    } while ( ( _Comms->Li2.bufferIn[8 + 16 + 2] >> 6 ) != TWSG_END_FLAG );

    if ( _Comms->config.General.debugFlag ) printf( "\nData Count: %d\n", dataCount );

    return 0;
}