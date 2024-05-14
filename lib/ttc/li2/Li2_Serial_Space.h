/*****************************************************************//**
 *  @file   Li2_Serial_Space.h
 *  @brief  
 * 
 *  @author N. Lok
 *  @date   08/March/2024
 *********************************************************************/
#ifndef LI2_SERIAL_SPACE_H_INCLUDED
#define LI2_SERIAL_SPACE_H_INCLUDED

/** Std Libs */
#include <stdio.h>
#include <stdint.h>

/**
 * Serial & Time Libs
 */
#include "../../utils/win/libserialport/libserialport.h"
#include "../../utils/win/timer/timer.h"

/** Serial Config Compiler Directives for Lithium */
#define SERIAL_BAUDRATE			9600
#define SERIAL_READ_TIMEOUT		10000		/**< 1 Second */
#define SERIAL_WRITE_TIMEOUT	10000		/**< 1 Second */

/** 
 * Typedef Imports
 * 
 * Imported from libserialport, needs changing according to lib being
 * used.
 */
typedef struct sp_port Serial_Port;			/**< Serial Port Object */
typedef struct sp_event_set Serial_Event;	/**< Serial Event Object */
typedef enum sp_return SERIAL_RTNS;		/**< Serial Returns Enum */
typedef enum sp_event SERIAL_EVENT_TYPE;	/**< Serial Event Type Enum */

extern sp_wait(  );

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

/** @brief Assigns an Event type to a given Event object for a Serial Port.
 *
 *  @param _Port        Serial Port object
 *  @param _event       Event object
 *  @param _eventType   Event type
 *  @return LI2_OK on success, or Error Code (Negative) on failure.
 */
SERIAL_RTNS Serial_AddEvent( Serial_Port *Port, Serial_Event **event, SERIAL_EVENT_TYPE eventType );

/** @brief Helper function for error handling.
 *
 *  @param _result
 *  @param _funcName
 *  @param _callerName
 *  @return The plain integer error code or in some cases the number of bytes read or written
 */
uint8_t Serial_Rtn( SERIAL_RTNS returnCode, const char *func, const char *caller );

#endif // LI2_SERIAL_SPACE_H_INCLUDED