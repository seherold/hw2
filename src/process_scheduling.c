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


int compareByArrival(const void *a, const void *b) 
{
	ProcessControlBlock_t * PCB1 = (ProcessControlBlock_t *)a;
	ProcessControlBlock_t * PCB2 = (ProcessControlBlock_t *)b;
	return (PCB1->arrival - PCB2->arrival);
}

// Runs the First Come First Served Process Scheduling algorithm over the incoming ready_queue
// \param ready queue a dyn_array of type ProcessControlBlock_t that contain be up to N elements
// \param result used for first come first served stat tracking \ref ScheduleResult_t
// \return true if function ran successful else false for an error
bool first_come_first_serve(dyn_array_t *ready_queue, ScheduleResult_t *result) 
{
	if (ready_queue == NULL || result == NULL || dyn_array_size(ready_queue) == 0) // check for invalid parameters or no processes to be scheduled
	{
		return false;
	}

	uint32_t currentTime = 0;
	uint32_t totalTurnAroundTime = 0;
	uint32_t totalWaitingTime = 0;

	if (dyn_array_sort(ready_queue, compareByArrival)) // sorting by arrival was successfull
	{
		size_t numPCBs = dyn_array_size(ready_queue);

		for (size_t i = 0; i < numPCBs; i++) // for all of the processes in the queue
		{
			ProcessControlBlock_t* pcb = (ProcessControlBlock_t *)dyn_array_at(ready_queue,i);

			if (currentTime < pcb->arrival) // ensures that the process has arrived
			{
				currentTime = pcb->arrival;
			}

			// before executing on virtual_cpu, remaining_burst_time should be the burst time of the process

			uint32_t completionTime = currentTime + pcb->remaining_burst_time;

			uint32_t turnAroundTime = completionTime - pcb->arrival;
			totalTurnAroundTime += turnAroundTime;

			uint32_t waitTime = turnAroundTime - pcb->remaining_burst_time;
			totalWaitingTime += waitTime;
			
			while(pcb->remaining_burst_time > 0) // this moves the process through units of time until it is completed
			{
				virtual_cpu(pcb);
				currentTime++;
			}
		}

		result->average_waiting_time = (float)totalWaitingTime/numPCBs;
		result->average_turnaround_time = (float)totalTurnAroundTime/numPCBs;
		result->total_run_time = currentTime;

		return true;
	}
	else
	{
		return false;
	}
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

	if (fptr == NULL)

	{
		return NULL;
	}

	uint32_t numPCBs;

	if (fread(&numPCBs, sizeof(uint32_t), 1, fptr) == 1)
	{
		dyn_array_t* pcbArray = dyn_array_create(numPCBs, sizeof(ProcessControlBlock_t), NULL);

		if (pcbArray == NULL)
		{
			fclose(fptr);
			return NULL;
		}

		for (uint32_t i = 0; i < numPCBs; i++)
		{
			ProcessControlBlock_t pcb;

			if (fread(&pcb.remaining_burst_time, sizeof(uint32_t), 1, fptr) == 1 &&
			fread(&pcb.priority, sizeof(uint32_t), 1, fptr) == 1 &&
			fread(&pcb.arrival, sizeof(uint32_t), 1, fptr) == 1)
			{
				if(dyn_array_push_back(pcbArray,&pcb) == false)
				{
					dyn_array_destroy(pcbArray);
					fclose(fptr);
					return NULL;
				}
			}
			else
			{
				dyn_array_destroy(pcbArray);
				fclose(fptr);
				return NULL;
			}
		}
		fclose(fptr);
		return pcbArray;
	}
	else
	{
		fclose(fptr);
		return NULL;
	}
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
