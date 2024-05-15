/*****************************************************************//**
 *  @file   timer.h
 *  @brief
 *
 *  @author N. Lok
 *  @date   08/March/2024
 *********************************************************************/
#ifndef TIMER_H_INCLUDED
#define TIMER_H_INCLUDED

#include "time.h"

 /** @brief Get the current time and date in the form of a char array.
  *
  *  @return Constant char array that holds the current time and date
  */
const char *Timer_GetDateTime( void );

long Timer_GetTimeMS( void );

#endif // TIMER_H_INCLUDED