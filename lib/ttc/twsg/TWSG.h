/*****************************************************************//**
 *  @file   TWSG.h
 *  @brief
 *
 *  @author N. Lok
 *  @date   08/March/2024
 *********************************************************************/
#ifndef TWSG_H_INCLUDED
#define TWSG_H_INCLUDED

/** Standard Libs */
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>

/** Additional Libs */
#include "../../ttc/li2/Li2.h"
#include "../../obc/transfer_buff/transfer_buff.h"
#include "../../obc/config/config_structs.h"

/** */
#define TWSG_MAX_PACKET_LEN             255
#define TWSG_PRI_HDR_LEN                2
#define TWSG_SEC_HDR_LEN                3
#define TWSG_MAX_PAYLOAD_SEC_LEN        250
#define TWSG_MAX_PAYLOAD_NO_SEC_LEN     253

/** */
#define TWSG_WAIT_TIMEOUT               10000
#define TWSG_NO_PACKET_SUB_NAME         0

//#define strdup _strdup

/** */
typedef enum {
    TWSG_TELECMD,       /** Telecmd  */
    TWSG_TELEMETRY      /** Telemetry */
} TWSG_PACKET_TYPE;

/** */
typedef enum {
    TWSG_NO_SEC_HDR,
    TWSG_SEC_HDR
} TWSG_SEC_HDR_FLAG;

/** */
typedef enum {
    TWSG_NACK,
    TWSG_ACK
} TWSG_ACK_FLAG;

/** Sub-Systems */
typedef enum {
    TWSG_OBC_ID,
    TWSG_LI2_ID,
    TWSG_ADCS_ID,
    TWSG_EPS_ID,
    TWSG_GNSS_ID,
    TWSG_SBAND_ID,
    TWSG_SOLAR_ID
} TWSG_SUB_SYS_ID;

/** Segement Enum */
typedef enum {
    TWSG_SEG_GROUND,
    TWSG_SEG_SPACE
} TWSG_SEG;

/** */
typedef enum {
    TWSG_CONTINUE_FLAG,
    TWSG_START_FLAG,
    TWSG_END_FLAG,
    TWSG_NO_SEG_FLAG
} TWSG_SEQ_SEG_FLAG;

/**
 * @struct Raw Packet Struct
 * Plain packet not to be used unless necessary
 */
typedef struct {
    uint8_t data[TWSG_MAX_PACKET_LEN];
} TWSG_Raw_Packet;

/**  */
typedef struct {
    uint8_t src : 3;
    uint8_t des : 3;
} TWSG_APID;

/**
 * @struct Primary Header Struct
 *
 * Essentially acts as the packet identifier.
 * 2 Byte
 */
typedef struct {
    uint8_t packetType : 1;
    uint8_t SecHdrFlag : 1;
    TWSG_APID APID;
    uint8_t packetName : 4;
    uint8_t packetSubName : 4;
} TWSG_Pri_Header;

/**
 * @struct Secondary Header Struct
 *
 * Included if packet sequence control is need for
 * segmented data.
 *
 * 3 Bytes
 */
typedef struct {
    uint8_t seqFlag : 2;
    uint32_t packetCount : 22;
} TWSG_Sec_Header;

/**
 * @struct
 *
 */
typedef struct {
    Li2_Radio Li2;
    Config config;
    Transfer_Buff *transferBuff;
} TWSG_Comms_System;

/** */
int8_t TWSG_Init( TWSG_Comms_System *Comms, Config Config, Transfer_Buff *TransferBuff );
int8_t TWSG_WaitForPacket( TWSG_Comms_System *_Comms, uint8_t _filterPacket );

void TWSG_SetPriHeader( TWSG_Comms_System *_Comms, uint8_t *_buffer, TWSG_SUB_SYS_ID _subSysID,
                        TWSG_PACKET_TYPE _packetType, TWSG_SEC_HDR_FLAG _secFlag,
                        uint8_t _packetName, uint8_t _packetSubName );
int8_t TWSG_ReadPriHeader( TWSG_Comms_System *_Comms, uint8_t *_buffer, TWSG_Pri_Header *_PriHeader );
int8_t TWSG_ReadSecHeader( TWSG_Comms_System *_Comms, uint8_t *_buffer, TWSG_Sec_Header *_SecHeader );

#endif // TWSG_H_INCLUDED