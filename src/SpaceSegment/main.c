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
#include "../../lib/ttc/twsg/TWSG_Space.h"
#include "../../lib/utils/win/img_reader/img_reader.h"

/** */
TWSG_Comms_System TWSG_Space;

void CALLBACK f( HWND hwnd, UINT uMsg, UINT timerId, DWORD dwTime ) {
    printf( "Hello\n" );
}

/** */
int main() {
    if ( TWSG_Init( &TWSG_Space, "TWSG_Config_Space.ini" ) != 0 ) abort();
    
    // Beacon Test
    printf( "\n-------------\n" );
    TWSG_OBC_Beacon beacon;
    memset( &beacon, 0, 64 );
    beacon.callSign[0] = 254;
    beacon.checksum = 4000;
    printf( "\n->Sending Beacon Packet\n" );
    //TWSG_SendBeacon( &TWSG_Space, &beacon );

    printf( "\n-------------\n" );
    uint8_t* extBuffer = ( uint8_t * )malloc( 500000000 );
    uint32_t test = IR_Read( "../../../TestingData/testImage.jpg", extBuffer, 500000000, &TWSG_Space.dataInBufferSize );
    printf( "\n-------------%ld\n", TWSG_Space.dataInBufferSize );

    TWSG_OBC_Buff_Down_Payload payload;
    payload.Telemetry.Meta.fileType = 1;
    payload.Telemetry.Meta.fileHash = 1234;
    payload.Telemetry.Meta.totalPacketCount = TWSG_Space.dataInBufferSize;
    //TWSG_SendBuffer( &TWSG_Space, &payload );
}