/*****************************************************************//**
 *  @file   img_reader.h
 *  @brief	Windows specific image reader that opens images and reads
 *          them byte by byte and puts it in a buffer. Also writes an
 *          to an image using bytes given to by a specified buffer.
 *
 *  @author N. Lok
 *  @date   08/March/2024
 *********************************************************************/
#ifndef IMG_READER_H_INCLUDED
#define IMG_READER_H_INCLUDED

/** Standard Libs */
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <windows.h>

/** */
typedef enum {
	IM_OK,
	IM_OPEN_FAIL,
	IM_CLOSE_FAIL,
	IM_READ_FAIL,
	IM_WRITE_FAIL,
	IM_BUFF_FULL
} IM_RTNS;

/** */
IM_RTNS IR_Read( const char *_imageLocation, uint8_t *_buffer, uint32_t bufferLen, uint32_t *imageSize );
IM_RTNS IR_Write( const char *image, uint8_t *buffer, uint32_t imageSize );

#endif // IMG_READER_H_INCLUDED