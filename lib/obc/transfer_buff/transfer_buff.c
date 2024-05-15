/*****************************************************************//**
 *  @file   transfer_buff.h
 *  @brief
 *
 *  @author N. Lok
 *  @date   08/March/2024
 *********************************************************************/
#include "transfer_buff.h"

int8_t TransferBuff_Init( Transfer_Buff *_TransferBuff, uint32_t _size ) {
	/**  */
	_TransferBuff->buff = ( uint8_t * )malloc( _size );

	/**  */
	_TransferBuff->maxSize = _size;

	/**  */
	return 0;
}

int8_t TransferBuff_Write( Transfer_Buff *_TransferBuff, uint8_t *_buffer, uint32_t _dataSize ) {
	/**  */
	if ( _TransferBuff->maxSize > _dataSize ) return 1;

	/**  */
	memcpy(  _TransferBuff->buff, _buffer, _dataSize );
	_TransferBuff->dataInBuffSize = _dataSize;

	/**  */
	return 0;
}

int8_t TransferBuff_Read( Transfer_Buff *_TransferBuff, uint8_t *_buffer, uint32_t _dataSize ) {
	/**  */
	if ( _TransferBuff->maxSize > _dataSize ) return 2;

	/**  */
	memcpy( _buffer, _TransferBuff->buff, _dataSize );

	/**  */
	return 0;
}