/*****************************************************************//**
 *  @file   scheduler.h
 *  @brief
 *
 *  @author N. Lok
 *  @date   08/March/2024
 *********************************************************************/
#ifndef SCHEDULER_H_INCLUDED
#define SCHEDULER_H_INCLUDED

/** Standard Libs */
#include <stdiO.h>
#include <stdint.h>
#include <stdbool.h>

/** Additional Libs */
#include "../../utils/win/timer/timer.h"

/** */
#define SCHEDULER_MAX_TASKS		16
#define SCHEDULER_NO_ID			-1

/** */
typedef void ( *Scheduler_Callback )( void );

/** */
typedef enum {
	SCHEDULER_OK,
	SCHEDULER_INIT_FAIL,
	SCHEDULER_SLOT_NOT_FREE,
	SCHEDULER_FULL,
	SCHEDULER_TASK_DONE
} SCHEDULER_RTNS;

/** */
typedef struct {
	bool active;
	uint32_t timeRemaining;
	uint32_t cycleTime;
	Scheduler_Callback callback;
} Scheduler_Task;

/** */
static Scheduler_Task Scheduler_Tasks[SCHEDULER_MAX_TASKS];

/** */
SCHEDULER_RTNS Scheduler_Init( void );
void Scheduler_Tick( void );

void Scheduler_GetTasks( bool *_statuses, uint32_t *_times );
uint8_t Scheduler_RtnCheck( SCHEDULER_RTNS returnCode, const char *func, const char *caller );

static SCHEDULER_RTNS Scheduler_AddTaskObj( int8_t _taskID, Scheduler_Task *task );
SCHEDULER_RTNS Scheduler_AddTask( uint8_t *taskID, void ( *callback )( void ), uint32_t seconds );
SCHEDULER_RTNS Scheduler_AddCyclicTask( uint8_t *taskID, void ( *callback )( void ), uint32_t seconds );

SCHEDULER_RTNS Scheduler_RemoveTask( uint8_t taskID );
SCHEDULER_RTNS Scheduler_RemoveAllTask( void );

#endif // SCHEDULER_H_INCLUDED