/*****************************************************************//**
 *  @file   img_reader.c
 *  @brief  Source code for Image Reader Lib
 *
 *  @author N. Lok
 *  @date   02/05/2024
 *********************************************************************/
#include "img_reader.h"

IM_RTNS IR_Read( const char *_imageLocation, uint8_t *_buffer, uint32_t _bufferLen, uint32_t *imageSize ) {
    FILE *imageFile;
    uint32_t n = 0;
    int c;

    /** Attempt to open image for reading */
    if ( fopen_s( &imageFile, _imageLocation, "rb" ) != 0 ) return IM_OPEN_FAIL;

    /** Read bytes from image */
    while ( ( c = fgetc( imageFile ) ) != EOF ) {        
        _buffer[n++] = c;
        if ( n > _bufferLen ) return IM_BUFF_FULL;
    }

    /** Close image and return image size */
    if ( fclose( imageFile ) != 0 ) return IM_CLOSE_FAIL;
    *imageSize = n;

    /** All good */
    return IM_OK;
}

IM_RTNS IR_Write( const char *_imageLocation, uint8_t *_buffer, uint32_t imageSize ) {
    FILE *imageFile;

    /** Attempt to open image for writting */
    if ( fopen_s( &imageFile, _imageLocation, "wb" ) != 0 ) return IM_OPEN_FAIL;

    /** Write bytes to image */
    for ( uint32_t i = 0; i < imageSize; ++i ) {
        fputc( _buffer[i], imageFile );
    }

    /** Close image */
    if ( fclose( imageFile ) != 0 ) return IM_CLOSE_FAIL;

    /** All good */
    return IM_OK;
}