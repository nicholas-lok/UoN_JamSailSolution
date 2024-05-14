/*****************************************************************//**
 *  @file   main.c
 *  @brief
 *
 *  @author N. Lok
 *  @date   08/March/2024
 *********************************************************************/
/** */

/** */
#include "../../lib/ttc/twsg/TWSG_Ground.h"
#include "../../lib/utils/win/img_reader/img_reader.h"

/** */
TWSG_Comms_System TWSG_Ground;

/** */
int main() {
    /** */
    if ( TWSG_Init( &TWSG_Ground, "TWSG_Config_Ground.ini" ) != 0 ) abort();

    /** */
    if ( TWSG_WaitForPacket( &TWSG_Ground, TWSG_OBC_BEACON ) != 0 ) abort();

    TWSG_OBC_Buff_Down_Payload payload;
    TWSG_ReceiveBuffDown( &TWSG_Ground, &payload );

    uint8_t test = IR_Write( "../../../TestingData/logo-annotated_CPY.jpg", TWSG_Ground.transferBuffer, TWSG_Ground.dataInBufferSize );

    //// Handshake Test
    //TWSG_OBC_Handshake_Payload payload1;
    //memset( &payload1, 0, 8 );
    //payload1.ack = 1;
    //printf( "\n->Sending Handshake Packet\n" );
    //TWSG_OBC_SendHandshake( &TWSG_Space, &payload1 );

    return 0;
}

//03/03/2024 GMT Standard Time