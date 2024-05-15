/*****************************************************************//**
 *  @file   TWSG_OBC_Structs.h
 *  @brief
 *
 *  @author N. Lok
 *  @date   08/March/2024
 *********************************************************************/
#ifndef TWSG_OBC_STRUCTS_H_INCLUDED
#define TWSG_OBC_STRUCTS_H_INCLUDED

/** */
#include "TWSG.h"

/** */
#define TWSG_OBC_BEACON_LEN             60
#define TWSG_OBC_HANDSHAKE_LEN          9
#define TWSG_OBC_BUFF_DOWN_META_LEN     6

 /** Packet Names */
typedef enum {
    TWSG_OBC_BEACON = 1,
    TWSG_OBC_HANDSHAKE,
    TWSG_OBC_HOUSEKEEP,
    TWSG_OBC_SCHEDULE_TSK,
    TWSG_OBC_ABORT_TSK,
    TWSG_OBC_BUFF_CPY,
    TWSG_OBC_BUFF_DOWN,
    TWSG_OBC_BUFF_UP,
    TWSG_OBC_RESTART
} TWSG_OBC_PACKET_NAMES;

/** Packet Types */
typedef enum {
    TWSG_OBC_BUFF_DOWN_META,
    TWSG_OBC_BUFF_DOWN_DATA
} TWSG_OBC_BUFF_DOWN_SUB_NAMES;

/**
 * @struct
 * Beacon Packet Struct ~~~WIP
 * - Only RP
 * - 60 Bytes
 */
typedef struct {
    char callSign[8];
    uint16_t rebootCounter;
    uint32_t uptime;
    uint32_t timestamp;
    int8_t MCUTemp;
    int8_t FPGATemp;
    int16_t magnetometer[3];
    int16_t gyroscope[3];
    uint16_t cpuCurrent;
    int8_t radioTemp;
    uint16_t epsVBat;
    uint16_t epsCurrentSolar;
    uint16_t epsCurrentConsum;
    uint16_t epsVpanel01;
    uint16_t epsVpanel02;
    uint16_t epsVpanel03;
    uint16_t epsCurrent01;
    uint16_t epsCurrent02;
    uint16_t epsCurrent03;
    uint16_t epsBatTemp;
    uint16_t satelliteErrorFlags;
    uint8_t satelliteOperationStatus;
    uint16_t checksum;
} TWSG_OBC_Beacon;

/**
 * @struct
 * Handshake Packet Struct ~~~WIP
 * - RQ & RP
 */
typedef struct {
    char callSign[8];
    TWSG_ACK_FLAG ack;
} TWSG_OBC_Handshake;

/**
 * @struct
 * Beacon Packet Struct ~~~WIP
 * - RQ & RP
 */
typedef struct {

    uint16_t checksum;
} TWSG_OBC_Housekeep_Payload;

/**
 * @struct
 * Scheduling Packet Struct ~~~WIP
 * - RQ & RP
 */
typedef struct {
    union {
        TWSG_SUB_SYS_ID subSysID;
    };
} TWSG_OBC_Schedule_Tsk_Payload;

/**
 * @struct
 * Housekeeping Packet Struct ~~~WIP
 */
typedef struct {
    uint8_t taskID;
    uint8_t sequence[2];
    uint16_t checksum;
} TWSG_OBC_Abort_Tsk_Payload;

/**
 * @struct
 * Buffer Copy Packet Struct ~~~WIP
 * - RQ & RP
 */
typedef struct {
    TWSG_SUB_SYS_ID subSysID;
} TWSG_OBC_Buff_Cpy_Payload;

/**
 * @struct
 * Buffer Download Packet Struct ~~~WIP
 * - RQ & RP
 */
typedef union {
    struct {
        uint32_t startPacketID : 22;
        uint32_t numOfPackets : 22;
    } Telecmd;

    struct {
        /**
         * @struct
         * Start Buffer Download Packet Struct ~~~WIP
         */
        struct {
            uint8_t fileType;
            uint8_t fileHash;
            uint32_t totalPacketCount;
        } Meta;
    } Telemetry;
} TWSG_OBC_Buff_Down_Payload;

/**
 * @struct
 * Buffer Upload Packet Struct ~~~WIP
 */
typedef union {
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
        uint8_t data[TWSG_MAX_PAYLOAD_SEC_LEN];
    } Normal;
} TWSG_OBC_Buff_Up_Payload;

/**
 * @struct
 * Scheduling Packet Struct ~~~WIP
 */
typedef struct {
    uint8_t sequence[2];
    uint16_t checksum;
} TWSG_OBC_Restart_Payload;

#endif // TWSG_OBC_STRUCTS_H_INCLUDED
