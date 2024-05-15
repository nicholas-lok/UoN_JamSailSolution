/*****************************************************************//**
 *  @file   Li2.c
 *  @brief  
 * 
 *  @author N. Lok
 *  @date   08/March/2024
 *********************************************************************/
#include "Li2.h"

LI2_RTNS Li2_Init( Li2_Radio *_Li2, const char *_serialPortName, uint32_t _serialBaudRate, LI2_SETUP _setup ) {
    /** Set Setup Flags in Lithium object */
    _Li2->setup = _setup;
    if ( _Li2->setup.debugFlag )printf( "\n->Starting Lithium Initialization ...\n" ); /** Debug */

    /** Attempt to Initialise the Radio Serial Port */
    if ( Serial_Rtn( Serial_Init( &(_Li2->SerialPort), _serialPortName, _serialBaudRate ), "Serial_Init", "Li2_Init" ) != LI2_OK )
        return LI2_ERR_SERIAL_FAIL;

    /** Skip Lithium tests if emulating */
    if ( _Li2->setup.emulatorFlag ) return LI2_OK;

    /** Send Noop to check comms */
    LI2_RTNS rtn = Li2_Rtn( Li2_NoOp( _Li2 ), "Li2_NoOp", "Li2_Init" );
    if ( rtn != LI2_OK ) return rtn;
    
    /** Get Firmware Revision */
    rtn = Li2_Rtn( Li2_GetFirmRev( _Li2 ), "Li2_GetFirmRev", "Li2_Init" );
    if ( rtn != LI2_OK )  return rtn;
    float firmRev;
    memcpy( &firmRev, &( _Li2->firmRev ), 4 );
    if ( _Li2->setup.debugFlag ) printf( "\n\n->Firm Rev: %.2f\n", _Li2->firmRev );    /** Debug */

    /** Get Tranceiver Configuration Settings */
    rtn = Li2_Rtn( Li2_GetTXRXConfig( _Li2 ), "Li2_GetTXRXConfig", "Li2_Init" );
    if ( rtn != LI2_OK )  return rtn;
    if ( _Li2->setup.debugFlag ) Li2_PrintTXRXConfig( &( _Li2->TXRXConfig ) );         /** Debug */

    /** Set Beacon Config */
    if ( _Li2->setup.beaconStatus ) {
        /** Set Beacon Data */
        char *data = "UoN JamSail CubeSat";
        memcpy( ( char * )_Li2->BeaconConfig.beacon_data, data, sizeof( data ) );
        _Li2->BeaconConfig.beacon_data_len = sizeof( data );
        rtn = Li2_Rtn( Li2_SetBeaconData( _Li2, &( _Li2->BeaconConfig.beacon_data ), 
                                            _Li2->BeaconConfig.beacon_data_len ), "Li2_SetBeaconConfig", "Li2_Init" );
        if ( rtn != LI2_OK )  return rtn;
        if ( _Li2->setup.debugFlag ) printf( "\n\n->Beacon Data: %s\n", _Li2->BeaconConfig.beacon_data );    /** Debug */
        
        /** Set Beacon Interval */
        _Li2->BeaconConfig.beacon_interval = _Li2->setup.beaconInterval;
        rtn = Li2_Rtn( Li2_SetBeaconConfig( _Li2, &( _Li2->BeaconConfig ) ), "Li2_SetBeaconConfig", "Li2_Init" );
        if ( rtn != LI2_OK )  return rtn;
        if ( _Li2->setup.debugFlag ) printf( "\n\n->Beacon Interval: %d (%f Sec)\n", _Li2->BeaconConfig.beacon_interval, 
                                                                                    _Li2->BeaconConfig.beacon_interval * 2.5f );    /** Debug */
    }

    /** Get Telemetry Data */
    rtn = Li2_Rtn( Li2_GetTelemetry( _Li2 ), "Li2_GetTelemetry", "Li2_Init" );
    if ( rtn != LI2_OK )  return rtn;
    if ( _Li2->setup.debugFlag ) Li2_PrintTelem( &( _Li2->Telem ) );                   /** Debug */

    /** All good */
    if ( _Li2->setup.debugFlag ) printf( "\n->Lithium Initiated Successfully\n" );     /** Debug */
    return LI2_OK;
}

LI2_RTNS Li2_TXDataPacket( Li2_Radio *_Li2, const uint8_t *_dataTX, uint8_t _len ) {
    /**  */
    LI2_RTNS rtn = Li2_Rtn( Li2_SendCDICmd( _Li2, LI2_CDI_TRANSMIT_DATA, _dataTX, _len ), "Li2_SendCDICmd", "Li2_TXDataPacket");
    if ( rtn != LI2_OK ) return rtn;

    /**  */
    if ( _Li2->setup.debugFlag ) printf( "\n->Waiting for ACK\n" );

    /**  */
    return Li2_Rtn( Li2_ReceiveCDICmd( _Li2, LI2_CDI_TRANSMIT_DATA ), "Li2_ReceiveCDICmd", "Li2_TXDataPacket" );
}

LI2_RTNS Li2_RXDataPacket( Li2_Radio *_Li2, uint8_t *_dataRX ) {
    return Li2_Rtn( Li2_ReceiveCDICmd( _Li2, LI2_CDI_RECEIVE_DATA ), "Li2_ReceiveCDICmd", "Li2_RXDataPacket" );
}

LI2_RTNS Li2_NoOp( Li2_Radio *_Li2 ) {
    /** Send No Op CDI command */
    LI2_RTNS rtn = Li2_Rtn( Li2_SendCDICmd( _Li2, LI2_CDI_NO_OP, NULL, 0 ), "Li2_SendCDICmd", "Li2_NoOp" );
    if ( rtn != LI2_OK ) return rtn;

    /**  */
    if ( _Li2->setup.debugFlag ) printf( "\n->Waiting for ACK\n" );

    /** Check for an acknowledgement */
    return Li2_Rtn( Li2_ReceiveCDICmd( _Li2, LI2_CDI_NO_OP ), "Li2_ReceiveCDICmd", "Li2_NoOp" );
}

LI2_RTNS Li2_Reset( Li2_Radio *_Li2 ) {
    /** Reset Radio */
    LI2_RTNS rtn = Li2_Rtn( Li2_SendCDICmd( _Li2, LI2_CDI_RESET_SYSTEM, NULL, 0 ), "Li2_SendCDICmd", "Li2_Reset");
    if ( rtn != LI2_OK ) return rtn;

    /**  */
    if ( _Li2->setup.debugFlag ) printf( "\n->Waiting for ACK\n" );

    /** Rtn for an acknowledgement */
    return Li2_Rtn( Li2_ReceiveCDICmd( _Li2, LI2_CDI_RESET_SYSTEM ), "Li2_ReceiveCDICmd", "Li2_Reset" );
}

LI2_RTNS Li2_GetTXRXConfig( Li2_Radio *_Li2 ) {
    /** Send CDI Command to get config */
    LI2_RTNS rtn = Li2_Rtn( Li2_SendCDICmd( _Li2, LI2_CDI_GET_TRANSCEIVER_CONFIG, NULL, 0 ), "Li2_SendCDICmd", "Li2_GetTXRXConfig" );
    if ( rtn != LI2_OK ) return rtn;

    /**  */
    if ( _Li2->setup.debugFlag ) printf( "\n->Waiting for Response\n" );

    /** Rtn for an acknowledgement */
    rtn = Li2_Rtn( Li2_ReceiveCDICmd( _Li2, LI2_CDI_GET_TRANSCEIVER_CONFIG ), "Li2_ReceiveCDICmd", "Li2_GetTXRXConfig" );
    if ( rtn != LI2_OK ) return rtn;

    /** Store in struct */
    _Li2->TXRXConfig.interface_baud_rate = _Li2->bufferIn[8];
    _Li2->TXRXConfig.tx_power_amp_level = _Li2->bufferIn[8 + 1];
    _Li2->TXRXConfig.rx_rf_baud_rate = _Li2->bufferIn[8 + 2];
    _Li2->TXRXConfig.tx_rf_baud_rate = _Li2->bufferIn[8 + 3];
    _Li2->TXRXConfig.rx_modulation = _Li2->bufferIn[8 + 4];
    _Li2->TXRXConfig.tx_modulation = _Li2->bufferIn[8 + 5];
    memcpy( &_Li2->TXRXConfig.rx_freq, &_Li2->bufferIn[8 + 6], 4 );
    memcpy( &_Li2->TXRXConfig.tx_freq, &_Li2->bufferIn[8 + 10], 4 );
    memcpy( &_Li2->TXRXConfig.source, &_Li2->bufferIn[8 + 14], 6 );
    memcpy( &_Li2->TXRXConfig.destination, &_Li2->bufferIn[8 + 20], 6 );
    memcpy( &_Li2->TXRXConfig.tx_preamble, &_Li2->bufferIn[8 + 26], 2 );
    memcpy( &_Li2->TXRXConfig.tx_postamble, &_Li2->bufferIn[8 + 28], 2 );
    memcpy( &_Li2->TXRXConfig.function_config, &_Li2->bufferIn[8 + 30], 2 );
    memcpy( &_Li2->TXRXConfig.function_config2, &_Li2->bufferIn[8 + 32], 2 );

    /** All Good */
    return LI2_OK;
}

LI2_RTNS Li2_GetTelemetry( Li2_Radio *_Li2 ) {
    /**  */
    LI2_RTNS rtn = Li2_Rtn( Li2_SendCDICmd( _Li2, LI2_CDI_GET_TELEMETRY, NULL, 0 ), "Li2_SendCDICmd", "Li2_GetTelemetry" );
    if ( rtn != LI2_OK ) return rtn;

    /**  */
    if ( _Li2->setup.debugFlag ) printf( "\n->Waiting for Response\n" );

    /**  */
    rtn = Li2_Rtn( Li2_ReceiveCDICmd( _Li2, LI2_CDI_GET_TELEMETRY ), "Li2_ReceiveCDICmd", "Li2_GetTelemetry" );
    if ( rtn != LI2_OK ) return rtn;

    /**  */
    memcpy( &_Li2->Telem.op_counter, &_Li2->bufferIn[8], 2 );
    memcpy( &_Li2->Telem.msp430_temp, &_Li2->bufferIn[8 + 2], 2 );
    _Li2->Telem.time_count[0] = _Li2->bufferIn[8 + 4];
    _Li2->Telem.time_count[1] = _Li2->bufferIn[8 + 5];
    _Li2->Telem.time_count[2] = _Li2->bufferIn[8 + 6];
    _Li2->Telem.rssi = _Li2->bufferIn[8 + 7];
    memcpy( &_Li2->Telem.bytes_received, &_Li2->bufferIn[8 + 8], 4 );
    memcpy( &_Li2->Telem.bytes_transmitted, &_Li2->bufferIn[8 + 12], 4 );

    /**  */
    if ( _Li2->firmRev == 4.01 ) {
        _Li2->Telem.rssi_lastpacket = _Li2->bufferIn[8 + 16];
        _Li2->Telem.rtc_alarm_flag = _Li2->bufferIn[8 + 17];
    }

    /**  */
    return LI2_OK;
}

LI2_RTNS Li2_GetFirmRev( Li2_Radio *_Li2 ) {
    LI2_RTNS rtn = Li2_Rtn( Li2_SendCDICmd( _Li2, LI2_CDI_READ_FIRMWARE_REV, NULL, 0 ), "Li2_SendCDICmd", "Li2_GetFirmRev" );
    if ( rtn != LI2_OK ) return rtn;

    /**  */
    if ( _Li2->setup.debugFlag ) printf( "\n->Waiting for Response\n" );

    /**  */
    rtn = Li2_Rtn( Li2_ReceiveCDICmd( _Li2, LI2_CDI_READ_FIRMWARE_REV ), "Li2_ReceiveCDICmd", "Li2_GetFirmRev" );
    if ( rtn != LI2_OK ) return rtn;
    
    /**  */
    memcpy( &(_Li2->firmRev), &(_Li2->bufferIn[8]), 4 );

    /**  */
    return LI2_OK;
}

LI2_RTNS Li2_SetTXRXConfig( Li2_Radio *_Li2, const Li2_TXRX_Config *_TXRXConfig ) {
    /**  */
    uint8_t payload[34];
    payload[0] = _TXRXConfig->interface_baud_rate;
    payload[1] = _TXRXConfig->tx_power_amp_level;
    payload[2] = _TXRXConfig->rx_rf_baud_rate;
    payload[3] = _TXRXConfig->tx_rf_baud_rate;
    payload[4] = _TXRXConfig->rx_modulation;
    payload[5] = _TXRXConfig->tx_modulation;
    memcpy( &payload[6], &_TXRXConfig->rx_freq, 4 );
    memcpy( &payload[10], &_TXRXConfig->tx_freq, 4 );
    memcpy( &payload[14], &_TXRXConfig->source, 6 );
    memcpy( &payload[20], &_TXRXConfig->destination, 6 );
    memcpy( &payload[26], &_TXRXConfig->tx_preamble, 2 );
    memcpy( &payload[28], &_TXRXConfig->tx_postamble, 2 );
    memcpy( &payload[30], &_TXRXConfig->function_config, 2 );
    memcpy( &payload[32], &_TXRXConfig->function_config2, 2 );

    /**  */
    LI2_RTNS rtn = Li2_Rtn( Li2_SendCDICmd( _Li2, LI2_CDI_SET_TRANSCEIVER_CONFIG, payload, 34 ), "Li2_SendCDICmd", "Li2_SetTXRXConfig" );
    if ( rtn != LI2_OK ) return rtn;

    /**  */
    if ( _Li2->setup.debugFlag ) printf( "\n->Waiting for Response\n" );

    /**  */
    rtn = Li2_Rtn( Li2_ReceiveCDICmd( _Li2, LI2_CDI_SET_TRANSCEIVER_CONFIG ), "Li2_ReceiveCDICmd", "Li2_SetTXRXConfig" );
    if ( rtn != LI2_OK ) return rtn;
    
    /**  */
    _Li2->TXRXConfig.interface_baud_rate = _TXRXConfig->interface_baud_rate;
    _Li2->TXRXConfig.tx_power_amp_level = _TXRXConfig->tx_power_amp_level;
    _Li2->TXRXConfig.rx_rf_baud_rate = _TXRXConfig->rx_rf_baud_rate;
    _Li2->TXRXConfig.tx_rf_baud_rate = _TXRXConfig->tx_rf_baud_rate;
    memcpy( &_Li2->TXRXConfig.rx_freq, &_TXRXConfig->rx_freq, 4 );
    memcpy( &_Li2->TXRXConfig.tx_freq, &_TXRXConfig->tx_freq, 4 );
    memcpy( &_Li2->TXRXConfig.source, &_TXRXConfig->source, 6 );
    memcpy( &_Li2->TXRXConfig.destination, &_TXRXConfig->destination, 6 );
    memcpy( &_Li2->TXRXConfig.tx_preamble, &_TXRXConfig->tx_preamble, 2 );
    memcpy( &_Li2->TXRXConfig.tx_postamble, &_TXRXConfig->tx_postamble, 2 );
    memcpy( &_Li2->TXRXConfig.function_config, &_TXRXConfig->function_config, 2 );
    memcpy( &_Li2->TXRXConfig.function_config2, &_TXRXConfig->function_config2, 2 );

    /**  */
    return LI2_OK;
}

LI2_RTNS Li2_SetRFConfig( Li2_Radio *_Li2, const Li2_RF_Config *_RFConfig ) {
    /**  */
    uint8_t payloadLen = ( _Li2->firmRev == 4.01 ) ? LI2_RF_STRUCT_LEN : LI2_RF_STRUCT_LEN_3_41;

    /**  */
    uint8_t *payload = ( uint8_t * )malloc( payloadLen );
    payload[0] = _RFConfig->front_end_level;
    payload[1] = _RFConfig->tx_power_amp_level;
    memcpy( &payload[2], &_RFConfig->tx_frequency_offset, 4 );
    memcpy( &payload[6], &_RFConfig->rx_frequency_offset, 4 );

    /**  */
    if ( _Li2->firmRev == 4.01 ) {
        payload[10] = _RFConfig->tx_frequency_deviation;
        payload[11] = _RFConfig->rx_frequency_deviation;
        payload[12] = _RFConfig->pre_transmit_delay;
        payload[13] = _RFConfig->post_transmit_delay;
    }

    /**  */
    LI2_RTNS rtn = Li2_Rtn( Li2_SendCDICmd( _Li2, LI2_CDI_SET_RF_CONFIG, payload, 10 ), "Li2_SendCDICmd", "Li2_SetRFConfig" );
    if ( rtn != LI2_OK ) return rtn;

    /**  */
    if ( _Li2->setup.debugFlag ) printf( "\n->Waiting for Response\n" );

    /**  */
    rtn = Li2_Rtn( Li2_ReceiveCDICmd( _Li2, LI2_CDI_SET_RF_CONFIG ), "Li2_ReceiveCDICmd", "Li2_SetRFConfig" );
    if ( rtn != LI2_OK ) return rtn;

    /**  */
    _Li2->RFConfig.front_end_level = _RFConfig->front_end_level;
    _Li2->RFConfig.tx_power_amp_level = _RFConfig->tx_power_amp_level;
    memcpy( &_Li2->RFConfig.tx_frequency_offset, &_RFConfig->tx_frequency_offset, 4 );
    memcpy( &_Li2->RFConfig.rx_frequency_offset, &_RFConfig->rx_frequency_offset, 4 );

    /**  */
    if ( _Li2->firmRev == 4.01 ) {
        _Li2->RFConfig.tx_frequency_deviation = _RFConfig->tx_frequency_deviation;
        _Li2->RFConfig.rx_frequency_deviation = _RFConfig->rx_frequency_deviation;
        _Li2->RFConfig.pre_transmit_delay = _RFConfig->pre_transmit_delay;
        _Li2->RFConfig.post_transmit_delay = _RFConfig->post_transmit_delay;
    }

    /**  */
    return LI2_OK;
}

LI2_RTNS Li2_SetBeaconConfig( Li2_Radio *_Li2, const Li2_Beacon_Config *_BeaconConfig ) {
    /**  */
    LI2_RTNS rtn = Li2_Rtn( Li2_SendCDICmd( _Li2, LI2_CDI_SET_BEACON_CONFIG, &_BeaconConfig->beacon_interval, 1 ), "Li2_SendCDICmd", "Li2_SetBeaconConfig" );
    if ( rtn != LI2_OK ) return rtn;

    /**  */
    if ( _Li2->setup.debugFlag ) printf( "\n->Waiting for Response\n" );

    /**  */
    rtn = Li2_Rtn( Li2_ReceiveCDICmd( _Li2, LI2_CDI_SET_BEACON_CONFIG ), "Li2_ReceiveCDICmd", "Li2_SetBeaconConfig" );
    if ( rtn != LI2_OK ) return rtn;

    /**  */
    _Li2->BeaconConfig.beacon_interval = _BeaconConfig->beacon_interval;

    /** All Good */
    return LI2_OK;
}

LI2_RTNS Li2_SetBeaconData( Li2_Radio *_Li2, const uint8_t *_data, uint8_t _len ) {
    /**  */
    LI2_RTNS rtn = Li2_Rtn( Li2_SendCDICmd( _Li2, LI2_CDI_SET_BEACON_DATA, _data, _len ), "Li2_SendCDICmd", "Li2_SetBeaconData" );
    if ( rtn != LI2_OK ) return rtn;

    /**  */
    if ( _Li2->setup.debugFlag ) printf( "\n->Waiting for Response\n" );

    /**  */
    rtn = Li2_Rtn( Li2_ReceiveCDICmd( _Li2, LI2_CDI_SET_BEACON_DATA ), "Li2_ReceiveCDICmd", "Li2_SetBeaconData" );
    if ( rtn != LI2_OK ) return rtn;

    /**  */
    memcpy( _Li2->BeaconConfig.beacon_data, _data, _len );
    _Li2->BeaconConfig.beacon_data_len = _len;

    /** All Good */
    return LI2_OK;
}

LI2_RTNS LI2_SetPowerAmp( Li2_Radio *_Li2, uint8_t _power ) {
    /**  */
    LI2_RTNS rtn = Li2_Rtn( Li2_SendCDICmd( _Li2, LI2_CDI_FAST_PA_SET, &(_power), 1 ), "Li2_SendCDICmd", "LI2_SetPowerAmp" );
    if ( rtn != LI2_OK ) return rtn;

    /**  */
    if ( _Li2->setup.debugFlag ) printf( "\n->Waiting for Response\n" );

    /**  */
    rtn = Li2_Rtn( Li2_ReceiveCDICmd( _Li2, LI2_CDI_FAST_PA_SET ), "Li2_ReceiveCDICmd", "LI2_SetPowerAmp" );
    if ( rtn != LI2_OK ) return rtn;

    /**  */
    _Li2->powerAmpLevel = _power;

    /** All Good */
    return LI2_OK;
}

LI2_RTNS Li2_SendCDICmd( Li2_Radio *_Li2, LI2_CDI_CMDS _cmd, const uint8_t *_payload, uint16_t _payloadSize ) {
    /**  */
    if ( _Li2->setup.debugFlag ) {
        printf( "\n\n-------------\n" );
        printf( "\n->Sending %s Command %s\n", Li2_GetCDIStr( _cmd ), _Li2->setup.emulatorFlag ? "Response" : "" );
    }

    /**  */
    _Li2->bufferOut[0] = LI2_CDI_SYNC_0;
    _Li2->bufferOut[1] = LI2_CDI_SYNC_1;
    _Li2->bufferOut[2] = _Li2->setup.emulatorFlag ? LI2_CDI_O_DIR : LI2_CDI_I_DIR;
    _Li2->bufferOut[3] = _cmd;
    _Li2->bufferOut[4] = _payloadSize >> 8;
    _Li2->bufferOut[5] = _payloadSize;

    /**  */
    LI2_RTNS rtn = Li2_Rtn( Li2_CheckPayloadSize( _Li2, _Li2->bufferOut[2] ,_cmd, _payloadSize ), "Li2_CheckPayloadSize", "Li2_SendCDICmd" );
    if ( rtn != LI2_OK ) return rtn;

    /** */
    uint16_t sendLen = 8;

    /**  */
    Li2_CalcCDIChecksums( &(_Li2->bufferOut[6]), &(_Li2->bufferOut[2]), 4 );

    /** Debug */
    if ( _Li2->setup.debugFlag ) Li2_PrintBuffer( _Li2->bufferOut, LI2_CDI_HEADER_LEN, "TX Header" );

    /**  */
    if ( _payloadSize > 0 ) {
        /**  */
        for ( auto i = 0; i < _payloadSize; i++ ) {
            _Li2->bufferOut[8 + i] = _payload[i];
        }

        /**  */
        Li2_CalcCDIChecksums( &(_Li2->bufferOut[8 + _payloadSize]), &(_Li2->bufferOut[2]), ( 6 + _payloadSize ) );
        sendLen += ( _payloadSize + 2 );
    }

    /** Debug */
    if ( _Li2->setup.debugFlag && ( _cmd == LI2_CDI_TRANSMIT_DATA ) ) {
        if ( _Li2->setup.buffDebugFlag ) Li2_PrintBuffer( &_Li2->bufferOut[8], _payloadSize, "TX Payload" );
        printf( "\nTX Payload Size: %d\n", _payloadSize );
    }

    /** Blocks RX ready */
    if ( Serial_Rtn( Serial_WaitTXReady( _Li2->SerialPort ), "Serial_WaitTXReady", "Li2_SendCDICmd" ) != LI2_OK )
        return LI2_ERR_NO_RESPONSE;

    /** Write to serial port */
    return Li2_Rtn( Li2_RawWrite( _Li2->SerialPort, _Li2->bufferOut, sendLen ), "Li2_RawWrite", "Li2_SendCDICmd" );
}

LI2_RTNS Li2_ReceiveCDICmd( Li2_Radio *_Li2, LI2_CDI_CMDS _expectCmd ) {
    /** Attempts to read a header for an expect command with valid sync characters */
    LI2_RTNS rtn = Li2_Rtn( Li2_ReadHeader( _Li2, _Li2->bufferIn, _expectCmd ), "Li2_ReadHeader", "Li2_ReceiveCDICmd" );
    if ( rtn != LI2_OK ) return rtn;

    /** Merge payload size bytes into one variable */
    uint16_t payloadLen = ( ( ( uint16_t )( _Li2->bufferIn[4] ) << 8 ) | _Li2->bufferIn[5] );
    if ( _Li2->setup.debugFlag ) printf( "\nPayload Size: %d\n", payloadLen );

    /** Check if Payload Size is an Acknowledgment or a Length */
    if ( ( payloadLen & 0x0FFF ) == LI2_CDI_NACK ) {
        /** Update Packed UART Bits */
        _Li2->UARTStatusBits.TXBufferFull = _Li2->bufferIn[4] & 0x1000;
        _Li2->UARTStatusBits.GPIO_A_State = _Li2->bufferIn[4] & 0x2000;
        _Li2->UARTStatusBits.GPIO_B_State = _Li2->bufferIn[4] & 0x4000;
        _Li2->UARTStatusBits.Pin_12_State = _Li2->bufferIn[4] & 0x8000;
        if ( _Li2->setup.debugFlag ) Li2_PrintUARTStatusBits( &( _Li2->UARTStatusBits ) );
        if ( _Li2->setup.debugFlag ) printf( "\n\n->NACK for %s Received\n", Li2_GetCDIStr( _Li2->bufferIn[3] ) );

        /** NACK Received - BAD */
        return LI2_ERR_NACK;
    } else if ( ( payloadLen & 0x0FFF ) == LI2_CDI_ACK ) {
        /** Update Packed UART Bits */
        _Li2->UARTStatusBits.TXBufferFull = _Li2->bufferIn[4] & 0x1000;
        _Li2->UARTStatusBits.GPIO_A_State = _Li2->bufferIn[4] & 0x2000;
        _Li2->UARTStatusBits.GPIO_B_State = _Li2->bufferIn[4] & 0x4000;
        _Li2->UARTStatusBits.Pin_12_State = _Li2->bufferIn[4] & 0x8000;
        if ( _Li2->setup.debugFlag ) Li2_PrintUARTStatusBits( &( _Li2->UARTStatusBits ) );
        if ( _Li2->setup.debugFlag ) printf( "\n\n->ACK for %s Received\n", Li2_GetCDIStr( _Li2->bufferIn[3] ) );

        /** ACK Received - All Good */
        return LI2_OK;
    } else if ( payloadLen == 0 ) {
        /** No Payload, so return */
        return LI2_OK;
    } else if ( payloadLen <= LI2_CDI_MAX_AX_PAYLOAD_LEN ) {
        /** Check payload size received in the header */
        Li2_CheckPayloadSize( _Li2, _Li2->bufferIn[2], _Li2->bufferIn[3], payloadLen );

        /** Read CDI CMD payload */
        return Li2_Rtn( Li2_ReadPayload( _Li2, _Li2->bufferIn, payloadLen ), "Li2_ReadPayload", "Li2_ReceiveCDICmd" );
    } else {
        return LI2_ERR_INVALID_HEADER;
    }
}

LI2_RTNS Li2_ReadHeader( Li2_Radio *_Li2, uint8_t *_bufferRX, LI2_CDI_CMDS _expectCmd ) {
    /** Blocks until bytes have been received */
    if ( Serial_Rtn( Serial_WaitRXReady( _Li2->SerialPort ), "Serial_WaitRXReady", "Li2_ReadHeader") != LI2_OK )
        return LI2_ERR_NO_RESPONSE;

    /** Sync serial communcation using sync characters */
    LI2_RTNS rtn;
    uint16_t timeoutCounter = 0;
    while (1) {
        /** Checks if there's still data waiting */
        if ( ( timeoutCounter++ ) < LI2_CDI_MAX_FULL_TX_LEN ) {
            /** Reads the first two bytes waiting to the RX buffer */
            rtn = Li2_Rtn( Li2_RawRead( _Li2->SerialPort, _bufferRX, LI2_CDI_SYNC_LEN ), "Li2_RawRead", "Li2_ReadHeader" );
            if ( rtn != LI2_OK ) return rtn;

            /** Debug - Print Sync Chars Read */
            if ( _Li2->setup.debugFlag ) printf( "\nSyncing: %c %c\n", _bufferRX[0], _bufferRX[1] );

            /** Checks for sync characters */
            if ( ( _bufferRX[0] == LI2_CDI_SYNC_0 ) && ( _bufferRX[1] == LI2_CDI_SYNC_1 ) ) {
                if ( _Li2->setup.debugFlag ) printf( "\n->Synced\n" );
                /** Read remain header bytes */
                rtn = Li2_Rtn( Li2_RawRead( _Li2->SerialPort, &(_bufferRX[2]), ( LI2_CDI_HEADER_LEN - LI2_CDI_SYNC_LEN ) ), "Li2_RawRead", "Li2_ReadHeader" );
                if ( rtn != LI2_OK ) return rtn;

                /** Check direction byte is correct */
                uint8_t dirCheck = _Li2->setup.emulatorFlag ? LI2_CDI_I_DIR : LI2_CDI_O_DIR;
                if ( _bufferRX[2] != dirCheck ) return LI2_ERR_INVALID_CDI_CMD_USAGE;

                /** Checks the expected cmd matches the cmd received or 0 for _expectCmd indicates don't care. */
                if ( ( _bufferRX[3] != _expectCmd ) && ( _expectCmd != 0 ) ) return LI2_ERR_WRONG_CMD;

                /** Check Header Checksums */
                uint8_t checksums[2];
                Li2_CalcCDIChecksums( checksums, &(_bufferRX[2]), 4 );
                if ( ( checksums[0] != _bufferRX[6] ) || ( checksums[1] != _bufferRX[7] ) ) return LI2_ERR_INVALID_HEADER_CKSUM;

                /** Good Header */
                if ( _Li2->setup.debugFlag ) {
                    Li2_PrintBuffer( _bufferRX, LI2_CDI_HEADER_LEN, "RXHeader" );
                    printf( "\n->Header RX: %s --\n", Li2_GetCDIStr( _bufferRX[3] ) );
                }
                return LI2_OK;
            }
        } else {
            /** Couldn't find any sync characters */
            return LI2_ERR_OUT_OF_SYNC;
        }
    }
}

LI2_RTNS Li2_ReadPayload( Li2_Radio *_Li2, uint8_t *_bufferRX, uint16_t _payloadLen ) {
    /** Blocks until bytes have been received */
    if ( Serial_Rtn( Serial_WaitRXReady( _Li2->SerialPort ), "Serial_WaitRXReady", "Li2_ReadHeader" ) != LI2_OK )
        return LI2_ERR_NO_RESPONSE;

    /** Read payload and payload checksum into correct place in buffer */
    if ( Li2_Rtn( Li2_RawRead( _Li2->SerialPort, &(_bufferRX[8]), ( _payloadLen + LI2_CDI_CHCKSUM_LEN ) ), "Li2_RawRead", "Li2_ReadPayload" ) != LI2_OK )
        return LI2_ERR_SERIAL_FAIL;

    /** Decode AX25 Header, if present */
    if ( _bufferRX[3] == LI2_CDI_RECEIVE_DATA ) {
        Li2_AX25Decode( &( _Li2->AX25Header ), &( _bufferRX[8] ) );

        /** Debug */
        if ( _Li2->setup.debugFlag && ( _bufferRX[3] == LI2_CDI_RECEIVE_DATA ) ) {
            Li2_PrintAX25Header( &( _Li2->AX25Header ) );
            if ( _Li2->setup.buffDebugFlag ) Li2_PrintBuffer( &_bufferRX[LI2_CDI_HEADER_LEN + LI2_AX25_HDR_STRUCT_LEN],
                                                                   ( _payloadLen - LI2_AX25_HDR_STRUCT_LEN - LI2_CDI_CHCKSUM_LEN ),
                                                                   "RXPayload" );
        }
    }

    /** Check Payload Checksum */
    uint8_t checksums[2];
    Li2_CalcCDIChecksums( checksums, &(_bufferRX[2]), ( ( LI2_CDI_HEADER_LEN - LI2_CDI_CHCKSUM_LEN ) + _payloadLen ) );
    if ( ( checksums[0] != _bufferRX[LI2_CDI_HEADER_LEN + _payloadLen] ) || ( checksums[1] != _bufferRX[LI2_CDI_HEADER_LEN + _payloadLen + 1] ) )
        return LI2_ERR_INVALID_PAYLOAD_CKSUM;

    /** All Good */
    return LI2_OK;
}

LI2_RTNS Li2_CheckPayloadSize( Li2_Radio *_Li2, uint8_t _dir, LI2_CDI_CMDS _cmd, uint16_t _payloadSize ) {
    /**  */
    uint8_t telemStructLen = ( _Li2->firmRev == 4.01f ) ? LI2_TELEM_STRUCT_LEN : LI2_TELEM_STRUCT_LEN_3_41;
    uint8_t RFStructLen = ( _Li2->firmRev == 4.01f ) ? LI2_RF_STRUCT_LEN : LI2_RF_STRUCT_LEN_3_41;

    /** Debug */
    if ( _Li2->setup.debugFlag ) printf( "\nChecking Payload Size: %d\n", _payloadSize );

    /** Check command received in the header */
    if ( _dir == LI2_CDI_I_DIR ) {
        switch ( _cmd ) {
            case LI2_CDI_TRANSMIT_DATA:
            case LI2_CDI_SET_BEACON_DATA:
            case LI2_CDI_FIRMWARE_PACKET:
                if ( _payloadSize > LI2_CDI_MAX_PAYLOAD_LEN ) return LI2_ERR_INVALID_CDI_CMD_USAGE;
                break;
            case LI2_CDI_SET_TRANSCEIVER_CONFIG:
                if ( _payloadSize != LI2_TXRX_STRUCT_LEN ) return LI2_ERR_INVALID_CDI_CMD_USAGE;
                break;
            case LI2_CDI_SET_RF_CONFIG: /** Two different versions */
                if ( _payloadSize != RFStructLen ) return LI2_ERR_INVALID_CDI_CMD_USAGE;
                break;
            case LI2_CDI_SET_BEACON_CONFIG:
                if ( _payloadSize != LI2_BEACON_CONFIG_STRUCT_LEN ) return LI2_ERR_INVALID_CDI_CMD_USAGE;
                break;
            case LI2_CDI_WRITE_OVER_AIR_KEY:
            case LI2_CDI_WRITE_FLASH:
            case LI2_CDI_FIRMWARE_UPDATE:
                if ( _payloadSize != 16 ) return LI2_ERR_INVALID_CDI_CMD_USAGE;
                break;
            case LI2_CDI_FAST_PA_SET:
                if ( _payloadSize != 1 ) return LI2_ERR_INVALID_CDI_CMD_USAGE;
                break;
            default:                    /** No Payload CDI Command */
                if ( _payloadSize != 0 ) return LI2_ERR_INVALID_CDI_CMD_USAGE;
        }
    } else {
        switch ( _cmd ) {
            case LI2_CDI_RECEIVE_DATA:
                if ( _payloadSize > LI2_CDI_MAX_AX_PAYLOAD_LEN ) return LI2_ERR_INVALID_CDI_CMD_USAGE;
                break;
            case LI2_CDI_READ_FIRMWARE_REV:
                if ( _payloadSize != LI2_FIRM_REV_STRUCT_LEN ) return LI2_ERR_INVALID_CDI_CMD_USAGE;
                break;
            case LI2_CDI_GET_TRANSCEIVER_CONFIG:
                if ( _payloadSize != LI2_TXRX_STRUCT_LEN ) return LI2_ERR_INVALID_CDI_CMD_USAGE;
                break;
            case LI2_CDI_GET_TELEMETRY: /** Two different versions */
                if ( _payloadSize != telemStructLen ) return LI2_ERR_INVALID_CDI_CMD_USAGE;
                break;
            default:                    /** No Payload CDI Command */
                if ( _payloadSize != 0 ) return LI2_ERR_INVALID_CDI_CMD_USAGE;
        }
    }

    /** All Good */
    return LI2_OK;
}

LI2_RTNS Li2_RawWrite( Serial_Port *_Port, const uint8_t *_dataTX, uint16_t _len ) {
    /** Writes to the serial port */
    if ( Serial_Rtn( Serial_Write( _Port, _dataTX, _len ), "Serial_Write", "Li2_RawWrite" ) < 0 ) {
        return LI2_ERR_SERIAL_FAIL;
    }

    /** Wait until all bytes have been transmitted to OS */
    if ( Serial_Rtn( Serial_WaitTXReady( _Port ), "Serial_WaitTXReady", "Li2_RawWrite" ) != SP_OK ) return LI2_ERR_SERIAL_FAIL;

    /** Waits for OS buffered bytes to be TXed over hardware */
    if ( Serial_Rtn( Serial_Drain( _Port ), "Serial_Drain", "Li2_RawWrite" ) != SP_OK ) return LI2_ERR_SERIAL_FAIL;

    /** All Good */
    return LI2_OK;
}

LI2_RTNS Li2_RawRead( Serial_Port *_SerialPort, uint8_t *_dataRX, uint16_t _len ) {
    uint16_t counter = 0;
    int8_t bytesRead;

    /** Reads for as many bytes specified by the header */
    for ( auto counter = 0; counter < _len; ++counter ) {
        bytesRead = Serial_Rtn( Serial_Read( _SerialPort, &(_dataRX[counter]), 1 ), "Serial_Read", "Li2_RawRead" );
        if ( bytesRead < 0 ) {
            return LI2_ERR_SERIAL_FAIL;
        } else if ( bytesRead == 0 ) {
            if ( counter == 0 )
                return LI2_ERR_NO_RESPONSE;
            else
                return LI2_ERR_MISSING_BYTES;
        }
    }

    /** All Good */
    return LI2_OK;
}

LI2_RTNS  Li2_SendCDIAck( Li2_Radio *_Li2, LI2_CDI_CMDS _cmd, uint8_t _ackStatus ) {
    /**  */
    if ( _Li2->setup.debugFlag ) printf( "\n->Sending Ack for %s\n", Li2_GetCDIStr( _cmd ) );

    /**  */
    _Li2->bufferOut[0] = LI2_CDI_SYNC_0;
    _Li2->bufferOut[1] = LI2_CDI_SYNC_1;
    _Li2->bufferOut[2] = LI2_CDI_O_DIR;
    _Li2->bufferOut[3] = _cmd;
    _Li2->bufferOut[4] = _ackStatus;
    _Li2->bufferOut[5] = _ackStatus;

    /**  */
    Li2_CalcCDIChecksums( &( _Li2->bufferOut[6] ), &( _Li2->bufferOut[2] ), 4 );

    /** Blocks RX ready */
    if ( Serial_Rtn( Serial_WaitTXReady( _Li2->SerialPort ), "Serial_WaitTXReady", "Li2_SendCDICmd" ) != LI2_OK )
        return LI2_ERR_NO_RESPONSE;

    /** Write to serial port */
    return Li2_Rtn( Li2_RawWrite( _Li2->SerialPort, _Li2->bufferOut, LI2_CDI_HEADER_LEN ), "Li2_RawWrite", "Li2_SendCDICmd" );
}

void Li2_CalcCDIChecksums( uint8_t *_checksums, const uint8_t *_data, uint16_t _len ) {
    _checksums[0] = 0;
    _checksums[1] = 0;

    for ( auto i = 0; i < _len; i++ ) {
        _checksums[0] = _checksums[0] + _data[i];
        _checksums[1] = _checksums[1] + _checksums[0];
    }
}

void Li2_AX25Decode( Li2_AX25_Header *_AX25Header, const uint8_t *_data ) {
    for ( auto i = 0; i < 7; i++ ) {
        _AX25Header->sourceAddr[i] = _data[i] >> 1;
        _AX25Header->destAddr[i] = _data[i + 7] >> 1;
    }
    _AX25Header->controlField = _data[7];
    _AX25Header->protocolID = _data[14];
}

void Li2_PrintBuffer( const uint8_t *_buffer, uint16_t _len, const char *_bufferName ) {
    printf( "\n%s: ", _bufferName );
    for ( auto i = 0; i < _len; ++i ) {
        printf( "%02x ", _buffer[i] );
    }
    printf( "\n" );
}

void Li2_PrintTelem( const Li2_Telemetry_Packet *_Telem ) {
    printf( "\n--- Telem --- " );
    uint32_t time = 0;
    memcpy( &time, &(_Telem->time_count), 3 );
    printf( "\nRadio Tick: %d", time );
    printf( "\nOperations Count: %d", _Telem->op_counter );
    printf( "\nProcessor Temp: %d degrees", _Telem->msp430_temp );
    printf( "\nRSSI: -%d dBm", _Telem->rssi );
    printf( "\nRX Count: %d  Byte", _Telem->bytes_received );
    printf( "\nTX Count: %d  Byte\n", _Telem->bytes_transmitted );
}

void Li2_PrintTXRXConfig ( const Li2_TXRX_Config *_TXRXConfig ) {
    const char *baudRate[] = {
        "1200"
        "9600",
        "19200",
        "38400",
        "76800"
    };
    const char *modulation[] = {
        "GFSK",
        "AFSK",
        "BPSK"
    };

    printf( "\n--- TXRX Config --- " );
    printf( "\nUART Baud Rate: %s", baudRate[_TXRXConfig->interface_baud_rate + 1] );
    printf( "\nTX Power Amplification Level: %d", _TXRXConfig->tx_power_amp_level );
    printf( "\nRX RF Baud Rate: %s", baudRate[_TXRXConfig->rx_rf_baud_rate] );
    printf( "\nTX RF Baud Rate: %s", baudRate[_TXRXConfig->tx_rf_baud_rate] );
    printf( "\nRX RF Modulation: %s", modulation[_TXRXConfig->rx_modulation] );
    printf( "\nTX RF Modulation: %s", modulation[_TXRXConfig->tx_modulation] );
    printf( "\nRX Frequency: %d Hz", _TXRXConfig->rx_freq );
    printf( "\nTX Frequency: %d Hz", _TXRXConfig->tx_freq );
    printf( "\nSource: %s", _TXRXConfig->source );
    printf( "\nDestination: %s", _TXRXConfig->destination );
    printf( "\nTX Preamble Length: %d Bytes", _TXRXConfig->tx_preamble );
    printf( "\nTX Postamble Length: %d Bytes", _TXRXConfig->tx_postamble );
    printf( "\nFunction Config: %04x", _TXRXConfig->function_config );
    printf( "\nFunction Config 2: %04x\n", _TXRXConfig->function_config2 );
}

void Li2_PrintUARTStatusBits( const Li2_UART_Status_Bits *_UARTStatusBits ) {
    printf( "\n--- UART Status Bits --- " );
    printf( "\nTX Buffer Status: %s", _UARTStatusBits->TXBufferFull ? "Full" : "Not Full" );
    printf( "\nGPIO A State: %s", _UARTStatusBits->GPIO_A_State ? "High" : "Low" );
    printf( "\nGPIO B State: %s", _UARTStatusBits->GPIO_B_State ? "High" : "Low" );
    printf( "\nPin 12 State: %s\n", _UARTStatusBits->Pin_12_State ? "High" : "Low" );
}

void Li2_PrintAX25Header( const Li2_AX25_Header *_AX25Header ) {
    printf( "\n--- AX25 Header --- " );
    printf( "\nSource Address: %s", _AX25Header->sourceAddr );
    printf( "\nDestination Address: %s", _AX25Header->destAddr );
    printf( "\nControl Field: %02x", _AX25Header->controlField );
    printf( "\nProtocol ID: %02x\n", _AX25Header->protocolID );
}

const char *Li2_GetCDIStr( LI2_CDI_CMDS _cmd ) {
    const char *cmdStr = "";
    switch ( _cmd ) {
        case LI2_CDI_NO_OP:
            cmdStr = "No Operation";
            break;
        case LI2_CDI_RESET_SYSTEM:
            cmdStr = "Reset System";
            break;
        case LI2_CDI_TRANSMIT_DATA:
            cmdStr = "Transmit Data";
            break;
        case LI2_CDI_RECEIVE_DATA:
            cmdStr = "Receive Data";
            break;
        case LI2_CDI_GET_TRANSCEIVER_CONFIG:
            cmdStr = "Get Transceiver Config";
            break;
        case LI2_CDI_SET_TRANSCEIVER_CONFIG:
            cmdStr = "Set Transceiver Config";
            break;
        case LI2_CDI_GET_TELEMETRY:
            cmdStr = "Get Telemetry";
            break;
        case LI2_CDI_WRITE_FLASH:
            cmdStr = "Write Flash";
            break;
        case LI2_CDI_SET_RF_CONFIG:
            cmdStr = "Set RF Config";
            break;
        case LI2_CDI_SET_BEACON_DATA:
            cmdStr = "Set Beacon Data";                 // 0x10
            break;
        case LI2_CDI_SET_BEACON_CONFIG:
            cmdStr = "Set Beacon Config";
            break;
        case LI2_CDI_READ_FIRMWARE_REV:
            cmdStr = "Read Firmware Revision";
            break;
        case LI2_CDI_WRITE_OVER_AIR_KEY:
            cmdStr = "Write Over Air Key";
            break;
        case LI2_CDI_FIRMWARE_UPDATE:
            cmdStr = "Firmware Update";
            break;
        case LI2_CDI_FIRMWARE_PACKET:
            cmdStr = "Firmware Packet";
            break;
        case LI2_CDI_WRITE_KEY_A_128:
            cmdStr = "Write Key A 128";
            break;
        case LI2_CDI_WRITE_KEY_B_128:
            cmdStr = "Write Key B 128";
            break;
        case LI2_CDI_WRITE_KEY_A_256:
            cmdStr = "Write Key A 256";
            break;
        case LI2_CDI_WRITE_KEY_B_256:
            cmdStr = "Write Key B 256";
            break;
        case LI2_CDI_FAST_PA_SET:
            cmdStr = "Fast PA Set";                 // 0x20
            break;
        case LI2_CDI_INVALIDATE_FLASH:
            cmdStr = "Invalidate Flash";
            break;
        case LI2_CDI_TOGGLE_IO_DIRECT:
            cmdStr = "Toggle IO Direct";
            break;
        case LI2_CDI_TRANSMIT_DATA_NO_HEADER:
            cmdStr = "Transmit Data No Header";     // 0x31
            break;
        case LI2_CDI_TRANSMIT_BEACON_DATA:
            cmdStr = "Transmit Beacon Data";
            break;
        case LI2_CDI_GET_RTC:
            cmdStr = "Get RTC";                     // 0x41
            break;
        case LI2_CDI_SET_RTC:
            cmdStr = "Set RTC";
            break;
        case LI2_CDI_ALARM_RTC:
            cmdStr = "Alarm RTC";
            break;
    }
    return cmdStr;
}

uint8_t Li2_Rtn( LI2_RTNS _returnCode, const char *_func, const char *_caller ) {
    /** Check return code */
    char *errMsg = "\n[%s][F:@%s|C:@%s] Unknown Return Code.";
    switch ( _returnCode ) {
        case LI2_ERR_SERIAL_FAIL:
            errMsg = "\n[%s][F:@%s|C:@%s] Failure with Serial Port, See Serial Errors.";
            break;
        case LI2_ERR_WRONG_CMD:
            errMsg = "\n[%s][F:@%s|C:@%s] Radio didn't respond with expect CDI Command.";
            break;
        case LI2_ERR_INVALID_CDI_CMD_USAGE:
            errMsg = "\n[%s][F:@%s|C:@%s] CDI command has invalid parameters.";
            break;
        case LI2_ERR_INVALID_HEADER:
            errMsg = "\n[%s][F:@%s|C:@%s] Radio responded with an Invalid CDI Header.";
            break;
        case LI2_ERR_INVALID_HEADER_CKSUM:
            errMsg = "\n[%s][F:@%s|C:@%s] Radio reponded with an Invalid CDI Header checksums.";
            break;
        case LI2_ERR_INVALID_PAYLOAD_CKSUM:
            errMsg = "\n[%s][F:@%s|C:@%s] Radio reponded with an Invalid CDI Payload checksums.";
            break;
        case LI2_ERR_MISSING_BYTES:
            errMsg = "\n[%s][F:@%s|C:@%s] Radio reponses has missing bytes.";
            break;
        case LI2_ERR_NACK:
            errMsg = "\n[%s][F:@%s|C:@%s] Radio responded with a NACK.";
            break;
        case LI2_ERR_OUT_OF_SYNC:
            errMsg = "\n[%s][F:@%s|C:@%s] Radio bytes are out of sync.";
            break;
        case LI2_ERR_NO_RESPONSE:
            errMsg = "\n[%s][F:@%s|C:@%s] Radio isn't responding.";
            break;
        case LI2_OK:
            return _returnCode;
        default:
            break;
    }

    fprintf( stderr, errMsg, Timer_GetDateTime(), _func, _caller);
    return _returnCode;
}