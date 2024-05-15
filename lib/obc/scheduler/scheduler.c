/*****************************************************************//**
 *  @file   scheduler.c
 *  @brief
 *
 *  @author N. Lok
 *  @date   08/March/2024
 *********************************************************************/
#include "scheduler.h"

SCHEDULER_RTNS Scheduler_Init( void ) {


	return SCHEDULER_OK;
}

void Scheduler_Tick( void ) {
	/** Loop through all tasks */
	for ( auto id = 0; id < SCHEDULER_MAX_TASKS; id++ ) {
		/** Decrement task timer and check if task timer elapsed */
		if ( --Scheduler_Tasks[id].timeRemaining == 0 ) {
			/** Call function for scheduled task */
			Scheduler_Tasks[id].callback();

			/** Reset task timer, if task is cyclic, else deactivate task */
			if ( Scheduler_Tasks[id].cycleTime > 0 ) {
				Scheduler_Tasks[id].timeRemaining = Scheduler_Tasks[id].cycleTime;
			} else {
				Scheduler_Tasks[id].active = false;
			}
		}
	}
}

void Scheduler_GetTasks( bool *_statuses, uint32_t *_times ) {
	/** Loop through all tasks */
	for ( auto id = 0; id < SCHEDULER_MAX_TASKS; id++ ) {
		/** Store status and remaining times */
		_statuses[id] = Scheduler_Tasks[id].active;
		_times[id] = Scheduler_Tasks[id].timeRemaining;
	}
}

uint8_t Scheduler_RtnCheck( SCHEDULER_RTNS _returnCode, const char *_func, const char *_caller ) {
	/** Check return code */
	char *errMsg = "\n[%s][F:@%s|C:@%s] Unknown Return Code.";
	switch ( _returnCode ) {
		case SCHEDULER_INIT_FAIL:
			errMsg = "\n[%s][F:@%s|C:@%s] Couldn't initialise scheduler.";
			break;
		case SCHEDULER_SLOT_NOT_FREE:
			errMsg = "\n[%s][F:@%s|C:@%s] Slot specified is not free.";
			break;
		case SCHEDULER_FULL:
			errMsg = "\n[%s][F:@%s|C:@%s] All Full.";
			break;
		case SCHEDULER_TASK_DONE:
			errMsg = "\n[%s][F:@%s|C:@%s] Task(s) removed might have been triggered before removal.";
			break;
		case SCHEDULER_OK:
			return _returnCode;
		default:
			break;
	}

	/** Print Error Message */
	printf( stderr, errMsg, Time_get(), _func, _caller );
	return _returnCode;
}

static SCHEDULER_RTNS Scheduler_AddTaskObj( int8_t _taskID, Scheduler_Task *task ) {
	/** Finds the first free task if no ID is specified, else assign to specified task */
	if ( _taskID == SCHEDULER_NO_ID ) { 
		/** Loop through all tasks */
		for ( auto id = 0; id < SCHEDULER_MAX_TASKS; id++ ) {
			/** Check if free slot */
			if ( !Scheduler_Tasks[id].active ) {
				/** Assign task to free slot and return id */
				Scheduler_Tasks[id] = *task;
				return SCHEDULER_OK;
			}
		}
	} else {								
		/** Check if slot is free */
		if ( !Scheduler_Tasks[_taskID].active ) {
			/** Assign task to specified slot */
			Scheduler_Tasks[_taskID] = *task;
			return SCHEDULER_OK;
		} else {
			/** Slot not avaliable */
			return SCHEDULER_SLOT_NOT_FREE;
		}
	}

	/** All Full */
	return SCHEDULER_FULL;
}

SCHEDULER_RTNS Scheduler_AddTask( int8_t *_taskID, void ( *_callback )( void ), uint32_t _seconds ) {
	/** Initialise task object */
	Scheduler_Task task = { true , _seconds, 0, _callback };

	/** Call add task helper */
	return Scheduler_AddTaskObj( *_taskID, &task );
}

SCHEDULER_RTNS Scheduler_AddCyclicTask( uint8_t *_taskID, void ( *_callback )( void ), uint32_t _seconds ) {
	/** Initialise task object with cycle time */
	Scheduler_Task task = { true , _seconds, _seconds, _callback };

	/** Call add task helper */
	return Scheduler_AddTaskObj( *_taskID, &task );
}

SCHEDULER_RTNS Scheduler_RemoveTask( uint8_t _taskID ) {
	/** Initialise var */
	bool cleanRemove = true;

	/** Checks if task has been triggered */
	if ( Scheduler_Tasks[_taskID].timeRemaining == 0 ) cleanRemove = false;

	/** Deactivate task */
	Scheduler_Tasks[_taskID].active = false;

	/** If task didn't triggered while removing, all ok, else not fully clean remove */
	if ( cleanRemove ) {
		return SCHEDULER_OK;
	} else {
		return SCHEDULER_TASK_DONE;
	}
}

SCHEDULER_RTNS Scheduler_RemoveAllTask( void ) {
	/** Initialise var */
	bool cleanRemove = true;

	/** Loop through all tasks */
	for ( auto id = 0; id < SCHEDULER_MAX_TASKS; id++ ) {
		/** Checks if task has been triggered */
		if ( Scheduler_Tasks[id].timeRemaining == 0 ) cleanRemove = false;

		/** Deactivate task */
		Scheduler_Tasks[id].active = false;
	}

	/** If no tasks triggered while removing, all ok, else not fully clean remove */
	if ( cleanRemove ) {
		return SCHEDULER_OK;
	} else {
		return SCHEDULER_TASK_DONE;
	}
}
