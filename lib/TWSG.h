#ifndef TWSG_H_INCLUDED
#define TWSG_H_INCLUDED

typedef struct telecmd_packet_type {
    uint8_t cmd_id;
    uint8_t packet_id[3];
    uint8_t payload[249];
    uint16_t checksum;
} TELECOMMAND_PACKET_type;

#endif // TWSG_H_INCLUDED
