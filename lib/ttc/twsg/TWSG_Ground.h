/*****************************************************************//**
 *  @file   TWSG_Ground.h
 *  @brief
 *
 *  @author N. Lok
 *  @date   08/March/2024
 *********************************************************************/
#ifndef TWSG_Ground_H_INCLUDED
#define TWSG_Ground_H_INCLUDED

/** */
#include "TWSG.h"
#include "TWSG_OBC.h"

/** */
#define TWSG_OBC_BUFF_DOWN_INIT_LEN 6

/** */
extern TWSG_Comms_System TWSG_Ground;

/** */
int8_t TWSG_GetHandshake( TWSG_Comms_System *_Comms );
int8_t TWSG_GetHousekeeping( TWSG_Comms_System *_Comms );
int8_t TWSG_StartBuffDown( TWSG_Comms_System *_Comms, TWSG_OBC_Buff_Down_Payload *_Payload );
int8_t TWSG_ReceiveBuffDown( TWSG_Comms_System *_Comms, TWSG_OBC_Buff_Down_Payload *_Payload );

#endif // TWSG_Ground_H_INCLUDED