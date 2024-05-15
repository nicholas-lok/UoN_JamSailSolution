/*****************************************************************//**
 *  @file   time.c
 *  @brief
 *
 *  @author N. Lok
 *  @date   08/March/2024
 *********************************************************************/
#include "timer.h"

const char *Timer_GetDateTime( void ) {
    time_t rawTime;
    struct tm TimeStruct;
    static char buffer[30];

    /** Gets current time in string format */
    time( &rawTime );
    localtime_s( &TimeStruct, &rawTime );
    strftime( buffer, sizeof( buffer ), "%d/%m/%Y %X GMT", &TimeStruct );

    return buffer;
}

long Timer_GetTimeMS( void ) {
    struct timespec ts;
    timespec_get( &ts, TIME_UTC );

    //return ( long )( ts.tv_nsec / 1000000 );
    return ( ( ts.tv_sec * 1000000000 ) + ts.tv_nsec )/ 1000000;
}