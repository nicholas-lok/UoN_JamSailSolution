#ifndef TWSG_H_INCLUDED
#define TWSG_H_INCLUDED

// Standard Libs
#include <stdio.h>
#include <stdint.h>
#include <windows.h>

// Additional Libs
#include "Li2_CDI.h"

/** Command Types */
typedef enum {
    TWSG_BEACON,
    TWSG_HANDSHAKE,
    TWSG_HOUSEKEEPING,
    TWSG_SCHEDULING,
    TWSG_BUFFER_COPY,
    TWSG_BUFFER_DOWNLOAD,
    TWSG_BUFFER_UPLOAD,
    TWSG_ABORT,
    TWSG_RESTART
} TWSG_CMD_ID;

/** Command Direction */
typedef enum {
    TWSG_RP_DIR,
    TWSG_RQ_DIR = 128
} TWSG_CMD_DIR;

/** Sub-Systems */
typedef enum {
    TWSG_ABACUS,
    TWSG_ADCS,
    TWSG_EPS,
    TWSG_GNSS,
    TWSG_LI2,
    TWSG_SBAND_RADIO,
    TWSG_SOLAR_SAIL
} TWSG_SUB_SYS_ID;

/** Ack/NoAck ~~~WIP */
typedef enum {
    TWSG_ACK,
    TWSG_NACK
} TWSG_ACK_VALUE;


/**
 * @struct
 * Beacon Packet Struct ~~~WIP
 * - Only RP
 */
typedef struct {
    char callSign[8];

    uint16_t checksum;
} TWSG_Beacon_Packet;

/** 
 * @struct 
 * Handshake Packet Struct ~~~WIP
 * - RQ & RP
 */ 
typedef struct {
    char callSign[8];
    TWSG_ACK_VALUE ack;
} TWSG_Handshake_Packet;

/**
 * @struct
 * Beacon Packet Struct ~~~WIP
 * - RQ & RP
 */
typedef struct {

    uint16_t checksum;
} TWSG_Housekeeping_Packet;

/**
 * @struct
 * Scheduling Packet Struct ~~~WIP
 * - RQ & RP
 */
typedef struct {
    TWSG_SUB_SYS_ID subSysID;
} TWSG_Scheduling_Packet;

/**
 * @struct
 * Buffer Copy Packet Struct ~~~WIP
 * - RQ & RP
 */
typedef struct {
    TWSG_SUB_SYS_ID subSysID;
} TWSG_Buffer_Copy_Packet;

/**
 * @struct
 * Buffer Download Packet Struct ~~~WIP
 * - RQ & RP
 */
typedef union {
    uint8_t packetID[3];

    /**
     * @struct
     * Start Buffer Download Packet Struct ~~~WIP
     */
    struct {
        //uint8_t hash; to confirm
        uint8_t fileType;
        uint32_t totalPacketNum;
    } Start;

    /**
     * @struct
     * Buffer Download Packet Struct ~~~WIP
     */
    struct {
        uint8_t data[249];
    } Normal;
} TWSG_Buffer_Download_Packet;

/**
 * @struct
 * Buffer Upload Packet Struct ~~~WIP
 */
typedef union {
    uint8_t packetID[3];

    /**
     * @struct
     * Start Buffer Upload Packet Struct ~~~WIP
     */
    struct {
        //uint8_t hash; to confirm
        uint8_t fileType;
        uint32_t totalPacketNum;
    } Start;

    /**
     * @struct
     * Buffer Upload Packet Struct ~~~WIP
     */
    struct {
        uint8_t data[249];
    } Normal;
} TWSG_Buffer_Upload_Packet;

/**
 * @struct
 * Housekeeping Packet Struct ~~~WIP
 */
typedef struct {
   
    uint8_t data[249];
} TWSG_Abort_Packet;

/**
 * @struct
 * Scheduling Packet Struct ~~~WIP
 */
typedef struct {
    uint8_t packetID[3];
    uint8_t data[249];
} TWSG_Restart_Packet;

/**
 * @struct Raw Packet Struct
 * Plain packet not to be used unless necessary
 */
typedef struct {
    uint8_t data[255];
} TWSG_Raw_Packet;

/** 
 * @union
 * TWSG Packet Union ~~~WIP
 * Allows caller to access all packet types from one identifier
 */
typedef struct {
    TWSG_CMD_ID cmdID;
    TWSG_CMD_DIR cmdDir;
    uint16_t checksum;
    union {
        TWSG_Beacon_Packet Beacon;
        TWSG_Handshake_Packet Handshake;
        TWSG_Housekeeping_Packet Housekeeping;
        TWSG_Scheduling_Packet Scheduling;
        TWSG_Buffer_Copy_Packet BuffCpy;
        TWSG_Buffer_Download_Packet BuffDownload;
        TWSG_Buffer_Upload_Packet BuffUpload;
        TWSG_Abort_Packet Abort;
        TWSG_Restart_Packet Restart;
    };
} TWSG_Packet;

#endif // TWSG_H_INCLUDED
