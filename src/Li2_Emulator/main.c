/*****************************************************************//**
 *  @file   main.c
 *  @brief
 *
 *  @author N. Lok
 *  @date   08/March/2024
 *********************************************************************/
/** */
#include <Windows.h>

/** */
#include "../../lib/ttc/li2/Li2.h"
#include "../../lib/utils/win/timer/timer.h"

/** */
Li2_Radio Li2_Em_Space;
Li2_Radio Li2_Em_Ground;

/** */
LI2_RTNS Li2_Emulate_Response( Li2_Radio *_Li2_RX, Li2_Radio *_Li2_TX );

/** */
int main() {
    /** Debug */
    printf( "->Starting Lithium Emulation...\n" ); 

    /** */
    LI2_SETUP setup = { true, true, true };

    /** */
    if ( Li2_Rtn( Li2_Init( &( Li2_Em_Ground ),
                                  "COM4",
                                  9600,
                                  setup ), "Li2_Init", "main" ) != LI2_OK ) abort();

    /** */
    Li2_Em_Ground.firmRev = 4.01f;

    /** */
    if ( Li2_Rtn( Li2_Init( &( Li2_Em_Space ),
                                  "COM5",
                                  9600,
                                  setup ), "Li2_Init", "main" ) != LI2_OK ) abort();

    /** */
    Li2_Em_Space.firmRev = 4.01f;

    /** */
    printf( "\n->Lithium Emulation Initiated Successfully\n" );

    /** */
    uint8_t len_ground = 0;
    uint8_t len_space = 0;

    /** */
    while ( 1 ) {

        /** */
        len_ground = Serial_RXWaiting( Li2_Em_Ground.SerialPort );
        len_space = Serial_RXWaiting( Li2_Em_Space.SerialPort );

        /** */
        if ( len_ground > 0 ) {
            printf( "\n->Recieving %d Bytes from Ground ----", len_ground );
            Li2_Rtn( Li2_Emulate_Response( &Li2_Em_Ground, &Li2_Em_Space ), "Li2_Emulate_Response", "main" );
        } 
        
        if ( len_space > 0 ) {
            printf( "\n->Recieving %d Bytes from Space ----", len_space );
            Li2_Rtn( Li2_Emulate_Response( &Li2_Em_Space, &Li2_Em_Ground ), "Li2_Emulate_Response", "main" );
        }
    }
}

LI2_RTNS Li2_Emulate_Response( Li2_Radio *_Li2_RX, Li2_Radio *_Li2_TX ) {
    /** */
    LI2_RTNS rtn = Li2_Rtn( Li2_ReceiveCDICmd( _Li2_RX, ( uint8_t )NULL ), "Li2_ReceiveCDICmd", "Li2_Emulate_Response" );
    if ( rtn != LI2_OK ) return rtn;

    /** */
    uint8_t buffer[LI2_CDI_MAX_AX_PAYLOAD_LEN];
    
    /** */
    switch ( _Li2_RX->bufferIn[3] ) {
        case LI2_CDI_READ_FIRMWARE_REV:
            printf( "\n->Sending Firmware Info Back" );

            /** */
            float firmRev = 4.01f;
            memcpy( buffer, &( firmRev ), 4 );

            /** Send CDI Cmd */
            rtn = Li2_Rtn( Li2_SendCDICmd( _Li2_RX, _Li2_RX->bufferIn[3], buffer, LI2_FIRM_REV_STRUCT_LEN ), "Li2_SendCDICmd", "Li2_Emulate_Response" );
            if ( rtn != LI2_OK ) return rtn;

            /** */
            Li2_PrintBuffer( &_Li2_RX->bufferOut[8], LI2_FIRM_REV_STRUCT_LEN, "Firm");
            break;

        case LI2_CDI_GET_TRANSCEIVER_CONFIG:
            printf( "\nSent TXRX Config ----\n" );

            /** */
            memset( buffer, 0, LI2_TXRX_STRUCT_LEN );
            buffer[0] = 0;
            buffer[1] = 1;
            buffer[2] = 2;
            buffer[3] = 3;
            buffer[4] = 0;
            buffer[5] = 1;

            /** Send CDI Cmd */
            rtn = Li2_Rtn( Li2_SendCDICmd( _Li2_RX, _Li2_RX->bufferIn[3], buffer, LI2_TXRX_STRUCT_LEN ), "Li2_SendCDICmd", "Li2_Emulate_Response" );
            if ( rtn != LI2_OK ) return rtn;

            /** */
            Li2_PrintBuffer( &_Li2_RX->bufferOut[8], LI2_TXRX_STRUCT_LEN, "TXRXConfig" );
            break;

        case LI2_CDI_GET_TELEMETRY:
            printf( "\nSend Telem ----\n" );

            /** */
            memset( buffer, 0, LI2_TELEM_STRUCT_LEN );
            uint16_t op_counter = 123;
            int16_t msp430_temp = -12;
            memcpy( buffer, &( op_counter ), 2 );
            memcpy( buffer, &( msp430_temp ), 2 );

            /** Send CDI Cmd */
            rtn = Li2_Rtn( Li2_SendCDICmd( _Li2_RX, _Li2_RX->bufferIn[3], buffer, LI2_TELEM_STRUCT_LEN ), "Li2_SendCDICmd", "Li2_Emulate_Response" );
            if ( rtn != LI2_OK ) return rtn;

            /** */
            Li2_PrintBuffer( &_Li2_RX->bufferOut[8], LI2_TELEM_STRUCT_LEN, "Telem" );
            break;

        case LI2_CDI_TRANSMIT_DATA:
            printf( "\n->Payload RX --\n" );

            /** */
            memset( buffer, 1, LI2_AX25_HDR_STRUCT_LEN );
            memcpy( &buffer[LI2_AX25_HDR_STRUCT_LEN], &_Li2_RX->bufferIn[8], _Li2_RX->bufferIn[5] );

            /** */
            uint16_t payloadLen = ( ( ( uint16_t )( _Li2_RX->bufferIn[4] ) << 8 ) | _Li2_RX->bufferIn[5] );
            printf( "\nPayload Size: %d\n", payloadLen );
            rtn = Li2_Rtn( Li2_SendCDICmd( _Li2_TX, LI2_CDI_RECEIVE_DATA, buffer, ( LI2_AX25_HDR_STRUCT_LEN + payloadLen ) ), "Li2_SendCDICmd", "Li2_Emulate_Response" );
            if ( rtn != LI2_OK ) return rtn;
        case LI2_CDI_NO_OP:
        case LI2_CDI_RESET_SYSTEM:
        case LI2_CDI_SET_TRANSCEIVER_CONFIG:
        case LI2_CDI_WRITE_FLASH:
        case LI2_CDI_SET_RF_CONFIG:
        case LI2_CDI_SET_BEACON_DATA:
        case LI2_CDI_SET_BEACON_CONFIG:
        case LI2_CDI_WRITE_OVER_AIR_KEY:
        case LI2_CDI_FIRMWARE_UPDATE:
        case LI2_CDI_FIRMWARE_PACKET:
        case LI2_CDI_FAST_PA_SET:
            /** Send Ack */
            rtn = Li2_Rtn( Li2_SendCDIAck( _Li2_RX, _Li2_RX->bufferIn[3], ( uint8_t )LI2_CDI_ACK ), "Li2_SendCDIAck", "Li2_Emulate_Response" );
            if ( rtn != LI2_OK ) return rtn;

            /** */
            Li2_PrintBuffer( _Li2_RX->bufferOut, LI2_CDI_HEADER_LEN, "->Send Ack" );
            break;

        default:
            break;
    }
    /** */
    printf( "\n-------------\n" );
    return LI2_OK;
}