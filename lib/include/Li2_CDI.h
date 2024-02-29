#ifndef LI2_CDI_H_INCLUDED
#define LI2_CDI_H_INCLUDED

// Standard Libs
#include <stdio.h>
#include <stdint.h>
#include <windows.h>

// Additional Libs
#include "../src/libserialport/libserialport.h"
#include "Li2_Config_Stuctures.h"

// Sync Characters
#define CDI_SYNC_0				'H'
#define CDI_SYNC_1				'e'

// Command Direction
#define CDI_I_Direction			0x10    // Radio In
#define CDI_O_Direction			0x20    // Radio Out

// ACK and Not ACK Values
#define CDI_ACK					0x0A0A
#define CDI_NACK				0xFFFF

// Serial Info
#define SERIAL_BAUDRATE			9600
#define SERIAL_READ_TIMEOUT		10000
#define SERIAL_WRITE_TIMEOUT	10000

// 
#define CDI_MAX_PACKET_LEN			265

// List of Commands
typedef enum {
	CDI_NO_OP = 0x01,
	CDI_RESET_SYSTEM,
	CDI_TRANSMIT_DATA,
	CDI_RECEIVE_DATA,
	CDI_GET_TRANSCEIVER_CONFIG,
	CDI_SET_TRANSCEIVER_CONFIG,
	CDI_TELEMETRY_QUERY,
	CDI_WRITE_FLASH,
	CDI_RF_CONFIG,
	CDI_BEACON_DATA = 0x10,
	CDI_BEACON_CONFIG,
	CDI_READ_FIRMWARE_REV,
	CDI_WRITE_OVER_AIR_KEY,
	CDI_FIRMWARE_UPDATE,
	CDI_FIRMWARE_PACKET,
	CDI_FAST_PA_SET = 0x20
} CDI_CMDS;

typedef enum {
	LI2_OK,
	LI2_INIT_FAIL = -1,
	LI2_SERIAL_INIT_FAIL = -2,
	LI2_SERIAL_WRITE_FAIL = -3,
	LI2_SERIAL_READ_FAIL = -4,
	LI2_INVALID_CDI_CMD = -5,
	LI2_INVALID_READ = -6,
	LI2_INVALID_ACK = -7,
	LI2_NACK_ERR = -8
} LI2_RETURNS;

// Serial Port Lib Typedef Imports
typedef struct sp_port Serial_Port;
typedef enum sp_return SERIAL_RETURNS;

// Telemetry Packet Struct
typedef struct {
	uint16_t op_counter;
	int16_t msp430_temp;
	uint8_t time_count[3];
	uint8_t rssi;
	uint32_t bytes_received;
	uint32_t bytes_transmitted;
} Li2_Telemetry_Packet;

//
typedef struct {
	// Serial Structs & Members
	Serial_Port *SerialPort;
	uint8_t bufferIn[CDI_MAX_PACKET_LEN];
	uint8_t bufferOut[CDI_MAX_PACKET_LEN];
	uint8_t dataAvailable;
	uint16_t bufferInPos;

	//unsigned long lastTimeReceived;

	// Config Structs
	Li2_General_Config Config;
	Li2_RF_Config RFConfig;
	Li2_Beacon_Config BeaconConfig;

	// Li-2 Packet Structs
	Li2_Telemetry_Packet Telem;

} Li2_Radio;

extern Li2_Radio Li2;

LI2_RETURNS Serial_Init(Serial_Port** _Port, const char* _portName, uint32_t _baudRate);
LI2_RETURNS Li2_Init( Li2_Radio *_Li2, const char *_serialPortName, uint32_t _serialBaudRate );
LI2_RETURNS Li2_SendCDICmd( const Li2_Radio *_Li2, CDI_CMDS _cmd, const uint8_t *_payload, SIZE_T _payloadLen);
LI2_RETURNS Li2_RawSend( const Li2_Radio *_Li2, const uint8_t *_dataTX, SIZE_T _len);
LI2_RETURNS Li2_ReadCDIAck( Li2_Radio *_Li2, CDI_CMDS _cmd );
void Li2_CalcCDIChecksums( const uint8_t *data, uint16_t len, uint8_t *CK );
int8_t Serial_Check(SERIAL_RETURNS _result, const char* _caller);
int8_t Li2_Check(LI2_RETURNS _result, const char* _caller);

#endif // LI2_CDI_H_INCLUDED
