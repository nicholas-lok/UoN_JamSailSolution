/*****************************************************************//**
 *  @file   main.c
 *  @brief
 *
 *  @author N. Lok
 *  @date   08/March/2024
 *********************************************************************/
/** */
#include <stdio.h>
#include <windows.h>

/** */
#include "../../lib/obc/config/config.h"
#include "../../lib/obc/transfer_buff/transfer_buff.h"
#include "../../lib/ttc/twsg/TWSG_Space.h"
#include "../../lib/utils/win/img_reader/img_reader.h"

/** */
Config Config_Space;
Transfer_Buff TransferBuff_Space;
TWSG_Comms_System TWSG_Space;

/** */
int main() {
    /** */
    if ( Config_Init( &Config_Space, "../../config/Config_Space.ini" ) ) abort();
    if ( TransferBuff_Init( &TransferBuff_Space, Config_Space.TransferBuff.size ) != 0 ) abort();
    if ( TWSG_Init( &TWSG_Space, Config_Space, &TransferBuff_Space ) != 0 ) abort();
    
    // Beacon Test
    printf( "\n-------------\n" );
    TWSG_OBC_Beacon beacon;
    memset( &beacon, 0, 64 );
    beacon.callSign[0] = 254;
    beacon.checksum = 4000;
    printf( "\n->Sending Beacon Packet\n" );
    TWSG_SendBeacon( &TWSG_Space, &beacon );

    printf( "\n-------------\n" );
    //uint8_t* imgBuffer = ( uint8_t * )malloc( 500000000 );
    uint32_t test = IR_Read( "../../../TestingData/testImage1.jpg", TransferBuff_Space.buff, TransferBuff_Space.maxSize, &TWSG_Space.transferBuff->dataInBuffSize );
    printf( "\n-------------%ld\n", TransferBuff_Space.dataInBuffSize );

    TWSG_OBC_Buff_Down_Payload payload;
    payload.Telemetry.Meta.fileType = 1;
    payload.Telemetry.Meta.fileHash = 1234;
    payload.Telemetry.Meta.totalPacketCount = TransferBuff_Space.dataInBuffSize;
    TWSG_SendBuffer( &TWSG_Space, &payload );
}