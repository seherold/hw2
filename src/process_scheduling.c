#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "dyn_array.h"
#include "processing_scheduling.h"


// You might find this handy.  I put it around unused parameters, but you should
// remove it before you submit. Just allows things to compile initially.
#define UNUSED(x) (void)(x)

// private function
void virtual_cpu(ProcessControlBlock_t *process_control_block) 
{
	// decrement the burst time of the pcb
	--process_control_block->remaining_burst_time;
}


// Runs the First Come First Served Process Scheduling algorithm over the incoming ready_queue
// \param ready queue a dyn_array of type ProcessControlBlock_t that contain be up to N elements
// \param result used for first come first served stat tracking \ref ScheduleResult_t
// \return true if function ran successful else false for an error
bool first_come_first_serve(dyn_array_t *ready_queue, ScheduleResult_t *result) 
{
	UNUSED(ready_queue);
	UNUSED(result);
	return false;
}

// Runs the Shortest Job First Scheduling algorithm over the incoming ready_queue
// \param ready queue a dyn_array of type ProcessControlBlock_t that contain be up to N elements
// \param result used for shortest job first stat tracking \ref ScheduleResult_t
// \return true if function ran successful else false for an error
bool shortest_job_first(dyn_array_t *ready_queue, ScheduleResult_t *result) 
{
	UNUSED(ready_queue);
	UNUSED(result);
	return false;
}

// Runs the Priority algorithm over the incoming ready_queue
// \param ready queue a dyn_array of type ProcessControlBlock_t that contain be up to N elements
// \param result used for shortest job first stat tracking \ref ScheduleResult_t
// \return true if function ran successful else false for an error
bool priority(dyn_array_t *ready_queue, ScheduleResult_t *result) 
{
	UNUSED(ready_queue);
	UNUSED(result);
	return false;
}


// Runs the Round Robin Process Scheduling algorithm over the incoming ready_queue
// \param ready queue a dyn_array of type ProcessControlBlock_t that contain be up to N elements
// \param result used for round robin stat tracking \ref ScheduleResult_t
// \param the quantum
// \return true if function ran successful else false for an error
bool round_robin(dyn_array_t *ready_queue, ScheduleResult_t *result, size_t quantum) 
{
	UNUSED(ready_queue);
	UNUSED(result);
	UNUSED(quantum);
	return false;
}


// Reads the PCB burst time values from the binary file into ProcessControlBlock_t remaining_burst_time field
// for N number of PCB burst time stored in the file.
// \param input_file the file containing the PCB burst times
// \return a populated dyn_array of ProcessControlBlocks if function ran successful else NULL for an error
dyn_array_t *load_process_control_blocks(const char *input_file) 
{
	if (input_file == NULL)
	{
		return NULL;
	}

	FILE* fptr = fopen(input_file, "rb");

	if (fptr == NULL) // error opening file
	{
		return NULL;
	}

	uint32_t numberOfPCBs;

	if (fread(&numberOfPCBs, sizeof(uint32_t), 1, fptr) != 1) // error reading number of PCBs, first number in binary file
	{
		fclose(fptr);
		return NULL;
	}

	dyn_array_t* arrayOfPCBs = dyn_array_create(numberOfPCBs, sizeof(ProcessControlBlock_t), NULL);

	if (arrayOfPCBs == NULL)
	{
		fclose(fptr);
		return NULL;
	}

	for (int i = 0; i < numberOfPCBs; i++)
	{
		ProcessControlBlock_t newPCB;

		if (fread(&newPCB.remaining_burst_time, sizeof(uint32_t), 1, fptr) != 1 ||
			fread(&newPCB.priority, sizeof(uint32_t), 1, fptr) != 1 ||
			fread(&newPCB.arrival, sizeof(uint32_t), 1, fptr) != 1)
		{
			free(arrayOfPCBs->array);
			free(arrayOfPCBs);
			fclose(fptr);
			return NULL;
		}

		dyn_array_push_back(arrayOfPCBs, &newPCB);
		
	}
    fclose(fptr);
	return arrayOfPCBs;
}


// Runs the Shortest Remaining Time First Process Scheduling algorithm over the incoming ready_queue
// \param ready queue a dyn_array of type ProcessControlBlock_t that contain be up to N elements
// \param result used for shortest job first stat tracking \ref ScheduleResult_t
// \return true if function ran successful else false for an error
bool shortest_remaining_time_first(dyn_array_t *ready_queue, ScheduleResult_t *result) 
{
	UNUSED(ready_queue);
	UNUSED(result);
	return false;
}
