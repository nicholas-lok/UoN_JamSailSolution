/*****************************************************************//**
 *  @file   Li2_Serial_Ground.h
 *  @brief  
 * 
 *  @author N. Lok
 *  @date   08/March/2024
 *********************************************************************/
#ifndef LI2_SERIAL_GROUND_H_INCLUDED
#define LI2_SERIAL_GROUND_H_INCLUDED

/** Std Libs */
#include <stdio.h>
#include <stdint.h>

/**
 * Serial & Time Libs
 * 
 * Change lib to relavant lib for segement
 * - Ground Segement: OS specific lib
 * - Space Segement: ABACUS OBC lib
 */
#include "../../utils/win/libserialport/libserialport.h"
#include "../../utils/win/timer/timer.h"

/** Serial Config Compiler Directives for Lithium */
#define SERIAL_BAUDRATE			9600
#define SERIAL_READ_TIMEOUT		5000		/**< 5 Second */
#define SERIAL_WRITE_TIMEOUT	5000		/**< 5 Second */

/**  */
#define SP_TIMEOUT				-5

/** 
 * Typedef Imports
 * 
 * Imported from libserialport, needs changing according to lib being
 * used.
 */
typedef struct sp_port Serial_Port;			/**< Serial Port Object */
typedef enum sp_return SERIAL_RTNS;			/**< Serial Returns Enum */

/** @brief Initiates the serial port.
 *
 *  Opens the specified serial port, configures its settings, sets its baud rate and
 *  flushs both buffers.
 *
 *  @param _Port        Pointer to where the serial object is to be stored.
 *  @param _portName    Name of the com port the serial port is assigned to.
 *  @param _baudRate    Baud rate of the serial port.
 *  @return LI2_OK on success, or Error Code (Negative) on failure.
 */
SERIAL_RTNS Serial_Init( Serial_Port **Port, const char *portName, uint32_t baudRate );

/**  */
SERIAL_RTNS Serial_WaitTXReady( Serial_Port *Port );

/**  */
SERIAL_RTNS Serial_WaitRXReady( Serial_Port *Port );

/**  */
SERIAL_RTNS Serial_Write( Serial_Port *Port, uint8_t *dataTX, uint16_t len );

/**  */
SERIAL_RTNS Serial_Read( Serial_Port *Port, uint8_t *dataRX, uint16_t len );

/**  */
SERIAL_RTNS Serial_Drain( Serial_Port *Port );

/**  */
SERIAL_RTNS Serial_TXWaiting( Serial_Port *Port );

/**  */
SERIAL_RTNS Serial_RXWaiting( Serial_Port *Port );

/** @brief Helper function for error handling.
 *
 *  @param _result
 *  @param _funcName
 *  @param _callerName
 *  @return The plain integer error code or in some cases the number of bytes read or written
 */
uint8_t Serial_Rtn( SERIAL_RTNS returnCode, const char *func, const char *caller );

#endif // LI2_SERIAL_GROUND_H_INCLUDED