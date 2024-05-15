/*****************************************************************//**
 *  @file   Li2_CDI.h
 *  @brief  
 * 
 *  @author N. Lok
 *  @date   08/March/2024
 *********************************************************************/
#ifndef LI2_H_INCLUDED
#define LI2_H_INCLUDED

/** Standard Libs */
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

/** Additional Li2 Libs */
#include "Li2_Structs.h"

#ifdef SEG_GROUND		/** Windows */
	#include "Li2_Serial_Ground.h"						
#elif SEG_SPACE			/** Abacus */
	#include "Li2_Serial_Space.h"
#else
	#error "Li2: No Preprocessor Macros Specified for Segment!"
#endif


/** Sync Characters */
#define LI2_CDI_SYNC_0				'H'
#define LI2_CDI_SYNC_1				'e'

/** Command Direction */
#define LI2_CDI_I_DIR				0x10    /**< Message Into Radio */
#define LI2_CDI_O_DIR				0x20    /**< Message Out of Radio */

/** ACK and Not ACK Values */
#define LI2_CDI_ACK					0x0A0A
#define LI2_CDI_NACK				0x0FFF

/** CDI Lengths */
#define LI2_CDI_MAX_FULL_TX_LEN		281		/**< Everything including header */
#define LI2_CDI_SYNC_LEN			2
#define LI2_CDI_HEADER_LEN			8
#define LI2_CDI_MAX_AX_PAYLOAD_LEN	273		/**< Includes payload chcksums when RX */
#define LI2_CDI_MAX_PAYLOAD_LEN		255
#define LI2_CDI_CHCKSUM_LEN			2

/** Lithium Error Codes Enum */
typedef enum {
	LI2_OK,
	LI2_ERR_SERIAL_FAIL,
	LI2_ERR_WRONG_CMD,
	LI2_ERR_INVALID_CDI_CMD_USAGE,
	LI2_ERR_INVALID_HEADER,
	LI2_ERR_INVALID_HEADER_CKSUM,
	LI2_ERR_INVALID_PAYLOAD_CKSUM,
	LI2_ERR_MISSING_BYTES,
	LI2_ERR_NACK,
	LI2_ERR_OUT_OF_SYNC,
	LI2_ERR_NO_RESPONSE
} LI2_RTNS;

/** */
typedef struct {
	bool debugFlag;
	bool buffDebugFlag;
	bool emulatorFlag;
	bool beaconStatus;
	uint8_t beaconInterval;
} LI2_SETUP;

/** Main Lithium Radio Struct */
typedef struct {
	/** General Config Members */
	float firmRev;
	uint8_t powerAmpLevel;
	uint8_t functionConfig;
	uint8_t functionConfig2;
	LI2_SETUP setup;

	/** Serial Members */
	Serial_Port *SerialPort;
	uint8_t bufferIn[LI2_CDI_MAX_FULL_TX_LEN];
	uint8_t bufferOut[LI2_CDI_MAX_FULL_TX_LEN];

	/** Config Struct Members */
	Li2_TXRX_Config TXRXConfig;
	Li2_RF_Config RFConfig;
	Li2_Beacon_Config BeaconConfig;

	/** Data Struct Members */
	Li2_Telemetry_Packet Telem;
	Li2_AX25_Header AX25Header;
	Li2_RTC_Packet RTC;
	Li2_UART_Status_Bits UARTStatusBits;
} Li2_Radio;

/** @brief Initialise Lithium Radio.
 * 
 *	Attempts to initialise serial port, set event handlers, establish comms and get lithium data.
 *
 *  @param[in] *_Li2                Lithium object
 *  @param[in] *_serialPortName		COM name for Serial Port
 *  @param[in] _serialBaudRate      Baud Rate for Serial Port
 *	@param[in] _segment				Defines where lithium is being used, space or ground
 *  @return	Li2_OK if sucessful, Error Code (Negative) if failed
 */
LI2_RTNS Li2_Init( Li2_Radio *Li2, const char *serialPortName, uint32_t serialBaudRate, LI2_SETUP setup );

/** @brief Transmits a data packet via CDI.
 *
 *  @param[in] *_Li2
 *  @param[in] _dataTX
 *  @param[in] _len
 *  @return
 */
LI2_RTNS Li2_TXDataPacket( Li2_Radio *Li2, const uint8_t *dataTX, uint8_t len );

/** @brief .
 *
 *  @param[in]	*_Li2
 *  @param[out] _dataRX
 *  @return Li2_OK if sucessful, Error Code (Negative) if failed
 */
LI2_RTNS Li2_RXDataPacket( Li2_Radio *Li2, uint8_t *dataRX );

/** @brief Send NoOp CDI Command.
 * 
 *	Attempts to transmit a NoOp CDI command and waits for an acknowledgement.
 * 
 *  @param[in] _Li2
 *  @return Li2_OK if sucessful, Error Code (Negative) if failed
 */
LI2_RTNS Li2_NoOp( Li2_Radio *Li2 );

/** @brief .
 *
 *  @param _Li2
 *  @return
 */
LI2_RTNS Li2_Reset( Li2_Radio *Li2 );

/** @brief .
 *
 *  @param _Li2
 *  @return
 */
LI2_RTNS Li2_GetTXRXConfig( Li2_Radio *Li2 );

/** @brief .
 *
 *  @param _Li2
 *  @return
 */
LI2_RTNS Li2_GetTelemetry( Li2_Radio *Li2 );

/**	@brief
 * 
 */
LI2_RTNS Li2_GetFirmRev( Li2_Radio *Li2 );

LI2_RTNS Li2_SetTXRXConfig( Li2_Radio *Li2, const Li2_TXRX_Config *TXRXConfig );
LI2_RTNS Li2_SetRFConfig( Li2_Radio *Li2, const Li2_RF_Config *RFConfig );
LI2_RTNS Li2_SetBeaconConfig( Li2_Radio *Li2, const Li2_Beacon_Config *BeaconConfig );
LI2_RTNS Li2_SetBeaconData( Li2_Radio *Li2, const uint8_t *data, uint8_t len );
LI2_RTNS LI2_SetPowerAmp( Li2_Radio *Li2, const uint8_t power );

LI2_RTNS Li2_SendCDICmd( Li2_Radio *Li2, LI2_CDI_CMDS cmd, const uint8_t *payload, uint16_t payloadLen );
LI2_RTNS Li2_ReceiveCDICmd( Li2_Radio *Li2, LI2_CDI_CMDS expectCmd );

/** @brief Attempts to read the first 8 bytes waiting to find a valid header.
 *
 *  @param[in]  *_Li2       Lithium object.
 *  @param[out] *_bufferRX  Buffer to receive header bytes
 *  @param[in]  _expectCmd  Expected command
 *  @return
 */
LI2_RTNS Li2_ReadHeader( Li2_Radio *Li2, uint8_t *bufferRX, LI2_CDI_CMDS expectCmd );
LI2_RTNS Li2_ReadPayload( Li2_Radio *Li2, uint8_t *bufferRX, uint16_t payloadLen );
LI2_RTNS Li2_CheckPayloadSize( Li2_Radio *Li2, uint8_t dir, LI2_CDI_CMDS cmd, uint16_t payloadSize );

LI2_RTNS Li2_RawWrite( Serial_Port *SerialPort, const uint8_t *dataTX, uint16_t len );
LI2_RTNS Li2_RawRead( Serial_Port *SerialPort, uint8_t *dataRX, uint16_t len );

LI2_RTNS Li2_SendCDIAck( Li2_Radio *Li2, LI2_CDI_CMDS cmd, uint8_t ackStatus );

void Li2_CalcCDIChecksums( uint8_t *checksums, const uint8_t *data, uint16_t len );

void Li2_AX25Decode( Li2_AX25_Header *AX25Header, const uint8_t *data );
void Li2_PrintBuffer( const uint8_t* buffer, uint16_t len, const char *bufferName );
void Li2_PrintTelem( const Li2_Telemetry_Packet *Telem );
void Li2_PrintTXRXConfig( const Li2_TXRX_Config *TXRXConfig );
void Li2_PrintUARTStatusBits( const Li2_UART_Status_Bits *UARTStatusBits );
void Li2_PrintAX25Header( const Li2_AX25_Header *AX25Header );
const char *Li2_GetCDIStr( LI2_CDI_CMDS cmd );

uint8_t Li2_Rtn( LI2_RTNS returnCode, const char *func, const char *caller );
/** @} */ // end of Li2CDIFunc

#endif // LI2_H_INCLUDED
