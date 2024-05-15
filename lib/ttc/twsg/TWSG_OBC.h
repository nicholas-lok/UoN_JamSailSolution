/*****************************************************************//**
 *  @file   TWSG_OBC.h
 *  @brief
 *
 *  @author N. Lok
 *  @date   08/March/2024
 *********************************************************************/
#ifndef TWSG_OBC_H_INCLUDED
#define TWSG_OBC_H_INCLUDED

/** */
#include "TWSG_OBC_Structs.h"

/** */
int8_t TWSG_SendBeacon( TWSG_Comms_System *_Comms, TWSG_OBC_Beacon *_Payload );
int8_t TWSG_SendHandshake( TWSG_Comms_System *_Comms, TWSG_OBC_Handshake *_Payload );
int8_t TWSG_SendHousekeeping( TWSG_Comms_System *_Comms, TWSG_OBC_Housekeep_Payload *_Payload );
int8_t TWSG_SendBuffer( TWSG_Comms_System *_Comms, TWSG_OBC_Buff_Down_Payload *_Payload );

#endif // TWSG_OBC_H_INCLUDED
