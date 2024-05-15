/*****************************************************************//**
 *  @file   transfer_buff.h
 *  @brief
 *
 *  @author N. Lok
 *  @date   08/March/2024
 *********************************************************************/
#ifndef TRANSFER_BUFF_H_INCLUDED
#define TRANSFER_BUFF_H_INCLUDED

/** Standard Libs */
#include <stdint.h>
#include <string.h>
#include <stdlib.h> 

/**  */
typedef struct {
    uint8_t *buff;
    uint32_t maxSize;
    uint32_t dataInBuffSize;
} Transfer_Buff;

/**  */
int8_t TransferBuff_Init( Transfer_Buff *_TransferBuff, uint32_t _size );
int8_t TransferBuff_Write( Transfer_Buff *_TransferBuff, uint8_t *_buffer, uint32_t _dataSize );
int8_t TransferBuff_Read( Transfer_Buff *_TransferBuff, uint8_t *_buffer, uint32_t _dataSize );

#endif // TRANSFER_BUFF_H_INCLUDED