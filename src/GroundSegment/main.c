/*****************************************************************//**
 *  @file   main.c
 *  @brief
 *
 *  @author N. Lok
 *  @date   08/March/2024
 *********************************************************************/
/** */


/** */
#include "../../lib/obc/config/config.h"
#include "../../lib/obc/transfer_buff/transfer_buff.h"
#include "../../lib/ttc/twsg/TWSG_Ground.h"
#include "../../lib/utils/win/img_reader/img_reader.h"

/** */
Config Config_Ground;
Transfer_Buff TransferBuff_Ground;
TWSG_Comms_System TWSG_Ground;

/** */
int main() {
    /** */
    if ( Config_Init( &Config_Ground, "../../config/Config_Ground.ini" ) != 0 ) abort();
    if ( TransferBuff_Init( &TransferBuff_Ground, Config_Ground.TransferBuff.size ) != 0 ) abort();
    if ( TWSG_Init( &TWSG_Ground, Config_Ground, &TransferBuff_Ground ) != 0 ) abort();

    /** */
    if ( TWSG_WaitForPacket( &TWSG_Ground, TWSG_OBC_BEACON ) != 0 ) abort();

    TWSG_OBC_Buff_Down_Payload payload;
    TWSG_ReceiveBuffDown( &TWSG_Ground, &payload );

    uint8_t test = IR_Write( "../../../TestingData/OUT.jpg", TransferBuff_Ground.buff, TWSG_Ground.transferBuff->dataInBuffSize );



    //// Handshake Test
    //TWSG_OBC_Handshake_Payload payload1;
    //memset( &payload1, 0, 8 );
    //payload1.ack = 1;
    //printf( "\n->Sending Handshake Packet\n" );
    //TWSG_OBC_SendHandshake( &TWSG_Space, &payload1 );

    return 0;
}

//03/03/2024 GMT Standard Time