/*****************************************************************//**
 *  @file   config_structs.h
 *  @brief
 *
 *  @author N. Lok
 *  @date   08/March/2024
 *********************************************************************/
#ifndef CONFIG_STRUCTS_H_INCLUDED
#define CONFIG_STRUCTS_H_INCLUDED

 /** Standard Libs */
#include <stdint.h>
#include <stdbool.h>

/**
 * @struct
 *
 */
typedef struct {
    struct {
        bool debugFlag;
    } General;
    struct {
        uint8_t segment : 1;
        uint8_t groundID : 3;
        uint32_t wait_timeout;
    } TWSG;
    struct {
        uint32_t baudRate;
        const char *serialPort;
        bool buffDebugFlag;
        bool beaconStatus;
        uint8_t beaconInterval;
    } Li2;
    struct {
        uint32_t size;
    } TransferBuff;
} Config;

#endif // CONFIG_STRUCTS_H_INCLUDED