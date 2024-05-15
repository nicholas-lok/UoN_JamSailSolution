/*****************************************************************//**
 *  @file   config_structs.h
 *  @brief
 *
 *  @author N. Lok
 *  @date   08/March/2024
 *********************************************************************/
#ifndef CONFIG_H_INCLUDED
#define CONFIG_H_INCLUDED

 /** Standard Libs */
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

/** Additional Libs */
#include "config_structs.h"
#include "../../utils/win/inih/ini.h"

/** */
int8_t Config_Init( Config *_Config, const char *_configDir );

static int INIHandler( void *user, const char *section, const char *name, const char *value );

#endif // CONFIG_H_INCLUDED

