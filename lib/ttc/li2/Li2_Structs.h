/*****************************************************************//**
 *  @file   Li2_Structs.h
 *  @brief  
 * 
 *  @author N. Lok
 *  @date   08/March/2024
 *********************************************************************/
#ifndef LI2_STRUCTS_H_INCLUDED
#define LI2_STRUCTS_H_INCLUDED

/** Std Includes */
#include <stdint.h>

/** @defgroup Li2FuncConfig Radio Configuration Discrete Behaviours
 *  Contains compiler definitions for 1st config struct options and settings
 *  @{
 */
/**	GPIO_B, Pin #10
 * 
 *	0000 0000 0000 xx00 Off Logic Low
 *	0000 0000 0000 xx01 2.0 second Toggle
 *	0000 0000 0000 xx10 TX Packet Toggle
 *	0000 0000 0000 xx11 Rx Packet Toggle
 */
#define LI2_CFG_P10_BIT_POS					0
#define LI2_CFG_P10_OFF						0
#define LI2_CFG_P10_TWO_SEC_TOG				1
#define LI2_CFG_P10_TX_TOG					2
#define LI2_CFG_P10_RX_TOG					3

/** Config Pin 1, Pin #14
 *
 *	0000 0000 0000 00xx Off Logic Low
 *	0000 0000 0000 01xx Tx/Rx Switch
 *	0000 0000 0000 10xx 2.0 hz WDT
 *	0000 0000 0000 11xx Rx Packet Toggle
 */
#define LI2_CFG_P14_BIT_POS					2
#define LI2_CFG_P14_OFF						0
#define LI2_CFG_P14_TXRX					1
#define LI2_CFG_P14_WDT						2
#define LI2_CFG_P14_RX_TOG					3

/**	External Event, Pin #12
 *
 *	0000 0000 xx00 xxxx Off Logic Low
 *	0000 0000 xx01 xxxx Enable
 *	0000 0000 xx0X xxxx Pattern A
 *	0000 0000 xx1X xxxx Pattern B
 */
#define LI2_CFG_P12_BIT_POS					4
#define LI2_CFG_P12_OFF						0
#define LI2_CFG_P12_ON_A					1
#define LI2_CFG_P12_RESERVED				2
#define LI2_CFG_P12_ON_B					3

/**	CRC Functions
 * 
 *	0000 0000 x1xx xxxx RX CRC - Enable [1]/Disable [0]
 *	0000 0000 1xxx xxxx TX CRC - Enable [1]/Disable [0] (TBD)
 */
#define LI2_CFG_CRC_BIT_POS					6
#define LI2_CFG_CRC_OFF						0
#define LI2_CFG_CRC_RX_ON					1
#define LI2_CFG_CRC_TX_ON					2
#define LI2_CFG_CRC_TXRX_ON					3

/**	Telemetry Functions
 *	
 *	0000 xxx1 xxxx xxxx Telemetry Packet Logging - Enable [1]/Disable [0]
 *	0000 x01x xxxx xxxx Logging Rate - 1/10 Hz [0], 1 Hz [1], 2 Hz [2], 4 Hz [3]
 *	0000 1xxx xxxx xxxx Telemetry Dump - Enable [1]/Disable [0]
 */
#define LI2_CFG_TELEM_BIT_POS				8
#define LI2_CFG_TELEM_LOG_ENABLE_MSK		0b0001
#define LI2_CFG_TELEM_DUMP_ENABLE_MSK		0b1000
#define LI2_CFG_TELEM_RATE_0_1_MSK			0B0000
#define LI2_CFG_TELEM_RATE_1_MSK			0b0010
#define LI2_CFG_TELEM_RATE_2_MSK			0b0100
#define LI2_CFG_TELEM_RATE_4_MSK			0b0110

/**	Beacon Functions
 * 
 *	0xx1 xxxx xxxx xxxx Ping Return - Enable [1]/Disable [0]
 *	0x1x xxxx xxxx xxxx Code Upload - Enable [1]/Disable [0]
 *	01xx xxxx xxxx xxxx System Reset - Enable [1]/Disable [0]
 * 
 *	Factory Defaults Restored
 * 
 *	1xxx xxxx xxxx xxxx Factory settings restore complete flag
 */
#define LI2_CFG_BCN_BIT_POS					12
#define LI2_CFG_BCN_PING_RTN_ENABLE_MSK		0b0001
#define LI2_CFG_BCN_CODE_UP_ENABLE_MSK		0b0010
#define LI2_CFG_BCN_RST_ENABLE_MSK			0b0100
#define LI2_FACTORY_RESET_MSK				0b1000
/** @} */ // end of Li2FuncConfig

/** @defgroup Li2FuncConfig2 Radio Configuration Discrete Behaviours 2
 *  Contains compiler definitions for 1st config struct options and settings
 *  @{
 */
/**  Config Functions 2
 * 
 *   0000 0000 0000 xxx1 Automatic Frequency Control(AFC) - Enable [1]/Disable [0]
 *   0000 0000 0000 xx0x RX CW
 *   0000 0000 0000 x0xx TX CW
 *   0000 0000 0000 0xxx Radio Alarm Transmit - Enable [1]/Disable [0]
 * 
 *	 0000 0000 xxx1 xxxx Encryption A Enabled
 *	 0000 0000 xx1x xxxx Encryption B Enabled
 *	 0000 0000 x1xx xxxx Radio Configration Duration Enabled
 *	 0000 0000 1xxx xxxx No AX25 Header but has 16 bit CRC
 */
#define LI2_CFG2_BIT_POS					0
#define LI2_CFG2_AFC_ENABLE_MSK				0b0001
#define LI2_CFG2_RX_CW_MSK					0b0010
#define LI2_CFG2_TX_CW_MSK					0b0100
#define LI2_CFG2_RADIO_ALARM_ENABLE	_MSK	0b1000

#define LI2_CFG2_BIT_POS2					4
#define LI2_CFG2_ENCRYPT_A_MSK				0b0001
#define LI2_CFG2_ENCRYPT_B_MSK				0b0010
#define LI2_CFG2_CFG_DUR_MSK				0b0100
#define LI2_CFG2_NO_AX25_HDR_MSK			0b1000
/** @} */ // end of Li2FuncConfig2

/** Over the Air CDI Cmds
 * 
 * Controls the radio in space via the radio on the ground
 */
#define LI2_CDI_OA_TELEM_DUMP				0x30		/**< Disabled in default firmware */
#define LI2_CDI_OA_PING_RETURN				0x31		/**< Responds with telemetry structure */
#define LI2_CDI_OA_CODE_UPLOAD				0x32		/**< Disabled in default firmware */
#define LI2_CDI_OA_RADIO_RESET				0x33		/**< Performs soft reset of radio */
#define LI2_CDI_OA_PIN_TOGGLE				0x34		/**< Directly toggles OA Pin */
#define LI2_CDI_OA_BSL_SEQ_INITIATE			0x42		/**< Disabled in default firmware */

/**	Data Struct Lengths */
#define LI2_TXRX_STRUCT_LEN					34
#define LI2_RF_STRUCT_LEN					14
#define LI2_TELEM_STRUCT_LEN				18
#define LI2_AX25_HDR_STRUCT_LEN				16
#define LI2_FIRM_REV_STRUCT_LEN				4
#define LI2_BEACON_CONFIG_STRUCT_LEN		1
#define LI2_RTC_STRUCT_LEN					12

#define LI2_RF_STRUCT_LEN_3_41				10
#define LI2_TELEM_STRUCT_LEN_3_41			16

/** List of CDI Commands Enum */
typedef enum {
	LI2_CDI_NO_OP = 0x01,
	LI2_CDI_RESET_SYSTEM,
	LI2_CDI_TRANSMIT_DATA,
	LI2_CDI_RECEIVE_DATA,
	LI2_CDI_GET_TRANSCEIVER_CONFIG,
	LI2_CDI_SET_TRANSCEIVER_CONFIG,
	LI2_CDI_GET_TELEMETRY,
	LI2_CDI_WRITE_FLASH,
	LI2_CDI_SET_RF_CONFIG,
	LI2_CDI_SET_BEACON_DATA = 0x10,
	LI2_CDI_SET_BEACON_CONFIG,
	LI2_CDI_READ_FIRMWARE_REV,
	LI2_CDI_WRITE_OVER_AIR_KEY,
	LI2_CDI_FIRMWARE_UPDATE,							/**< Disabled in default firmware */
	LI2_CDI_FIRMWARE_PACKET,							/**< Disabled in default firmware */
	LI2_CDI_WRITE_KEY_A_128,							/**< Disabled in default firmware */
	LI2_CDI_WRITE_KEY_B_128,							/**< Disabled in default firmware */
	LI2_CDI_WRITE_KEY_A_256,							/**< Disabled in default firmware */
	LI2_CDI_WRITE_KEY_B_256,							/**< Disabled in default firmware */
	LI2_CDI_FAST_PA_SET = 0x20,
	LI2_CDI_INVALIDATE_FLASH,							/**< Disabled in default firmware */
	LI2_CDI_TOGGLE_IO_DIRECT,
	LI2_CDI_TRANSMIT_DATA_NO_HEADER = 0x31,
	LI2_CDI_TRANSMIT_BEACON_DATA,
	LI2_CDI_GET_RTC = 0x41,
	LI2_CDI_SET_RTC,
	LI2_CDI_ALARM_RTC
} LI2_CDI_CMDS;

/** UART Baud Rate Enum */
typedef enum {
	UART_9600,
	UART_19200,
	UART_38400,
	UART_76800,
	UART_115200
} LI2_UART_BAUD_RATE;

/** RF Baud Rate Enum */
typedef enum {
	RF_1200,
	RF_9600,
	RF_19200,
	RF_38400
} LI2_RF_BAUD_RATE;

/** RF Modulation Type Enum */
typedef enum {
	GFSK,
	AFSK,
	BPSK
} LI2_RF_MODULATION;

/** Tranceiver Config Structure */
typedef struct {
	LI2_UART_BAUD_RATE interface_baud_rate; 	/**< Radio Interface Baud Rate (9600=0x00) */
	uint8_t tx_power_amp_level; 				/**< Tx Power Amp level (min = 0x00 max = 0xFF) */
	LI2_RF_BAUD_RATE rx_rf_baud_rate; 			/**< Radio RX RF Baud Rate (9600=0x00) */
	LI2_RF_BAUD_RATE tx_rf_baud_rate; 			/**< Radio TX RF Baud Rate (9600=0x00) */
	LI2_RF_MODULATION rx_modulation;			/**< (0x00 = GFSK) */
	LI2_RF_MODULATION tx_modulation;			/**< (0x00 = GFSK) */
	uint32_t rx_freq; 							/**< Channel Rx Frequency (ex: 45000000) */
	uint32_t tx_freq; 							/**< Channel Tx Frequency (ex: 45000000) */
	unsigned char source[6]; 					/**< AX25 Mode Source Call Sign (default NOCALL) */
	unsigned char destination[6]; 				/**< AX25 Mode Destination Call Sign (default CQ) */
	uint16_t tx_preamble; 						/**< AX25 Mode Tx Preamble Byte Length (0x00 = 20 flags) */
	uint16_t tx_postamble; 						/**< AX25 Mode Tx Postamble Byte Length (0x00 = 20 flags) */
	uint16_t function_config; 					/**< Radio Configuration Discrete Behaviors */
	uint16_t function_config2;					/**< Radio Configuration Discrete Behaviors #2 */
} Li2_TXRX_Config;

/** Low Level RF Config Structure */
typedef struct {
	uint8_t front_end_level; 					/**< 0 to 63 Value */
	uint8_t tx_power_amp_level; 				/**< 0 to 255 value, non-linear */
	uint32_t tx_frequency_offset; 				/**< Up to 20 kHz */
	uint32_t rx_frequency_offset; 				/**< Up to 20 kHz */
	uint8_t tx_frequency_deviation;				/**< Set for your baud rate options: 0 (2.7 kHz),1 (5.4 kHz),2( 10.8 kHz ), 3 ( 21.6 kHz ), 4 ( 43.2 kHz ) CAUTION */
	uint8_t rx_frequency_deviation;				/**< N/A for release 3.10 */
	uint8_t pre_transmit_delay;					/**< Delay in tens of milliseconds. Default 1 second = 100 */
	uint8_t post_transmit_delay;				/**< Delay in tens of milliseconds. Default 0 (NOT IMPLEMENTED */
} Li2_RF_Config;

/** Beacon Config Structure */
typedef struct {
	uint8_t beacon_interval; 					/**< Value of 0 is off, 2.5 sec delay per LSB */
	uint8_t beacon_data[255];					/**< Extra to store data */
	uint8_t beacon_data_len;					/**< Length of data */
} Li2_Beacon_Config;

/** Telemetry Packet Struct */
typedef struct {
	uint16_t op_counter;
	int16_t msp430_temp;
	uint8_t time_count[3];
	uint8_t rssi;
	uint32_t bytes_received;
	uint32_t bytes_transmitted;
	uint8_t rssi_lastpacket;
	uint8_t rtc_alarm_flag;
} Li2_Telemetry_Packet;

/** AX25 Header Struct */
typedef struct {
	unsigned char sourceAddr[7];
	unsigned char destAddr[7];
	uint8_t controlField;
	uint8_t protocolID;
} Li2_AX25_Header;

/** RTC Packet Struct */
typedef struct {
	unsigned short int year;					/**< Year = 0x2021 */
	unsigned char mon;							/**< Month = 0x10 = October */
	unsigned char day;							/**< Day = 0x07 = 7th */
	unsigned char dow;							/**< Day of week = 0x05 = Friday */
	unsigned char hour;							/**< Hour = 0x11 */
	unsigned char min;							/**< Minute = 0x59 */
	unsigned char sec;							/**< Seconds = 0x30 */
	unsigned char alarm_dow;					/**< RTC Day of week alarm = 0x2 */
	unsigned char alarm_day;					/**< RTC Day Alarm = 0x20 */
	unsigned char alarm_hour;					/**< RTC Hour Alarm */
	unsigned char alarm_min;					/**< RTC Minute Alarm */
} Li2_RTC_Packet;

/** Packed UART Status Struct */
typedef struct {
	bool TXBufferFull;
	bool GPIO_A_State;
	bool GPIO_B_State;
	bool Pin_12_State;
} Li2_UART_Status_Bits;

#endif /**< LI2_STRUCTS_H_ */