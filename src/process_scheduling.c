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
	uint32_t totalRunTime = 0;
	uint32_t totalTurnAroundTime = 0;
	uint32_t totalWaitingTime = 0;

	size_t numPCBs = dyn_array_size(ready_queue);

	if (dyn_array_sort(ready_queue, compareByArrival) == false)
	{
		return false;
	}

	while (dyn_array_size(ready_queue) > 0) // for all of the processes in the queue
	{

		ProcessControlBlock_t processToRun;

		if (dyn_array_extract_front(ready_queue, &processToRun) == false) // grabs the pcb and removes it from the ready_queue
		{
			return false;
		} // now we have the process we want to run

		if (processToRun.priority == 0) // priority = 0 is not a valid priority, we have bad data, scheduling algorithm fails
		{
			return false;
		}

		if (currentTime <= processToRun.arrival) // ensures that the process has arrived
		{
			currentTime = processToRun.arrival; // we don't necessarily care here what the first arrival time is, we've already sorted by arrival so the one with the shortest arrival time should be first
		}

		uint32_t waitTime = currentTime - processToRun.arrival;
    	totalWaitingTime += waitTime;

		while(processToRun.remaining_burst_time > 0) // this moves the process through units of time until it is completed
		{
			virtual_cpu(&processToRun);
			currentTime++;
			totalRunTime++;
		}

		uint32_t turnAroundTime = currentTime - processToRun.arrival;
		totalTurnAroundTime += turnAroundTime;
	}

	result->average_waiting_time = (float)totalWaitingTime/numPCBs;
	result->average_turnaround_time = (float)totalTurnAroundTime/numPCBs;
	result->total_run_time = totalRunTime;

	return true;
}

int compareByBurstTime(const void *a, const void *b) 
{
	ProcessControlBlock_t * PCB1 = (ProcessControlBlock_t *)a;
	ProcessControlBlock_t * PCB2 = (ProcessControlBlock_t *)b;
	return (PCB1->remaining_burst_time - PCB2->remaining_burst_time);
}

// Runs the Shortest Job First Scheduling algorithm over the incoming ready_queue
// \param ready queue a dyn_array of type ProcessControlBlock_t that contain be up to N elements
// \param result used for shortest job first stat tracking \ref ScheduleResult_t
// \return true if function ran successful else false for an error
bool shortest_job_first(dyn_array_t *ready_queue, ScheduleResult_t *result) 
{
	if (ready_queue == NULL || result == NULL || dyn_array_size(ready_queue) == 0) // check for invalid parameters or no processes to be scheduled
	{
		return false;
	}

	uint32_t currentTime = 0;
	uint32_t totalRunTime = 0;
	uint32_t totalTurnAroundTime = 0;
	uint32_t totalWaitingTime = 0;

	size_t numPCBs = dyn_array_size(ready_queue);

	if (dyn_array_sort(ready_queue, compareByArrival) == false)
	{
		return false;
	}

	dyn_array_t* arrived_queue = dyn_array_create(dyn_array_size(ready_queue), sizeof(ProcessControlBlock_t), NULL);
	if (arrived_queue == NULL)
	{
		return false;
	}

	while (dyn_array_size(ready_queue) > 0)
	{
		dyn_array_clear(arrived_queue); // starting with cleared arrived_queue

		for (size_t i = 0; i < dyn_array_size(ready_queue); i++)
		{
			ProcessControlBlock_t* pcb = (ProcessControlBlock_t *)dyn_array_at(ready_queue,i);

			if (currentTime >= pcb->arrival) // ensures that the process has arrived
			{
				if(dyn_array_push_back(arrived_queue, pcb) == false)
				{
					dyn_array_destroy(arrived_queue);
					return false;
				}
			}
		} // now have all the processes that have arrived

		if (dyn_array_size(arrived_queue) == 0) //what if no processes have arrived
		{
			ProcessControlBlock_t* pcbFirstArrived = (ProcessControlBlock_t *)dyn_array_front(ready_queue); 
			currentTime = pcbFirstArrived->arrival; // set currentTime to the smallest arrival time you have in the ready_queue, this is the process that has arrived next, we want to start running it
		}
		else // one or more processes have arrived
		{
			if (dyn_array_sort(arrived_queue, compareByBurstTime) == false)
			{
				dyn_array_destroy(arrived_queue);
				return false;
			}

			ProcessControlBlock_t processToRun;

			if (dyn_array_extract_front(arrived_queue, &processToRun) == false)
			{
				dyn_array_destroy(arrived_queue);
				return false;
			} // now we have the process we want to run

			if (processToRun.priority == 0) // priority = 0 is not a valid priority, we have bad data, scheduling algorithm fails
			{
				dyn_array_destroy(arrived_queue);
				return false;
			}

			// need to remove process we are about to run from ready_queue

			for (size_t i = 0; i < dyn_array_size(ready_queue); i++)
			{
				ProcessControlBlock_t* pcbToRemove = (ProcessControlBlock_t *)dyn_array_at(ready_queue,i);

				if (pcbToRemove->remaining_burst_time == processToRun.remaining_burst_time && 
					pcbToRemove->priority == processToRun.priority &&
					pcbToRemove->arrival == processToRun.arrival &&
					pcbToRemove->started == processToRun.started)
				{
					if(dyn_array_erase(ready_queue, i)  == false)
					{
						dyn_array_destroy(arrived_queue);
						return false;
					}
				}
			}

			// now we can run the process and calculate the times
			uint32_t waitTime = currentTime - processToRun.arrival;
    		totalWaitingTime += waitTime;

			while(processToRun.remaining_burst_time > 0) // this moves the process through units of time until it is completed
			{
				virtual_cpu(&processToRun);
				currentTime++;
				totalRunTime++;
			}

			uint32_t turnAroundTime = currentTime - processToRun.arrival;
			totalTurnAroundTime += turnAroundTime;
		}
	}

	dyn_array_destroy(arrived_queue);
	
	result->average_waiting_time = (float)totalWaitingTime/numPCBs;
	result->average_turnaround_time = (float)totalTurnAroundTime/numPCBs;
	result->total_run_time = totalRunTime;

	return true;
}


int compareByPriority(const void *a, const void *b) 
{
	ProcessControlBlock_t * PCB1 = (ProcessControlBlock_t *)a;
	ProcessControlBlock_t * PCB2 = (ProcessControlBlock_t *)b;
	return (PCB1->priority - PCB2->priority);
}

// Runs the Priority algorithm over the incoming ready_queue
// \param ready queue a dyn_array of type ProcessControlBlock_t that contain be up to N elements
// \param result used for shortest job first stat tracking \ref ScheduleResult_t
// \return true if function ran successful else false for an error
bool priority(dyn_array_t *ready_queue, ScheduleResult_t *result) 
{
	//do we need to actively prevent starvation?
	//UNUSED(ready_queue);
	//UNUSED(result);
	//return false;
	
	if (ready_queue == NULL || result == NULL || dyn_array_size(ready_queue) == 0) // check for invalid parameters or no processes to be scheduled
	{
		return false;
	}

	uint32_t currentTime = 0;
	uint32_t totalRunTime = 0;
	uint32_t totalTurnAroundTime = 0;
	uint32_t totalWaitingTime = 0;

	size_t numPCBs = dyn_array_size(ready_queue);

	if (dyn_array_sort(ready_queue, compareByArrival) == false)
	{
		return false;
	}

	dyn_array_t* arrived_queue = dyn_array_create(dyn_array_size(ready_queue), sizeof(ProcessControlBlock_t), NULL);
	if (arrived_queue == NULL)
	{
		return false;
	}

	while (dyn_array_size(ready_queue) > 0)
	{
		dyn_array_clear(arrived_queue); // starting with cleared arrived_queue

		for (size_t i = 0; i < dyn_array_size(ready_queue); i++)
		{
			ProcessControlBlock_t* pcb = (ProcessControlBlock_t *)dyn_array_at(ready_queue,i);

			if (currentTime >= pcb->arrival) // ensures that the process has arrived
			{
				if(dyn_array_push_back(arrived_queue, pcb) == false)
				{
					dyn_array_destroy(arrived_queue);
					return false;
				}
			}
		} // now have all the processes that have arrived

		if (dyn_array_size(arrived_queue) == 0) //what if no processes have arrived
		{
			ProcessControlBlock_t* pcbFirstArrived = (ProcessControlBlock_t *)dyn_array_front(ready_queue); 
			currentTime = pcbFirstArrived->arrival; // set currentTime to the smallest arrival time you have in the ready_queue, this is the process that has arrived next, we want to start running it
		}
		else // one or more processes have arrived
		{
			if (dyn_array_sort(arrived_queue, compareByPriority) == false)
			{
				dyn_array_destroy(arrived_queue);
				return false;
			}

			ProcessControlBlock_t processToRun;

			if (dyn_array_extract_front(arrived_queue, &processToRun) == false)
			{
				dyn_array_destroy(arrived_queue);
				return false;
			} // now we have the process we want to run

			if (processToRun.priority == 0) // priority = 0 is not a valid priority, we have bad data, scheduling algorithm fails
			{
				dyn_array_destroy(arrived_queue);
				return false;
			}

			// need to remove process we are about to run from ready_queue

			for (size_t i = 0; i < dyn_array_size(ready_queue); i++)
			{
				ProcessControlBlock_t* pcbToRemove = (ProcessControlBlock_t *)dyn_array_at(ready_queue,i);

				if (pcbToRemove->remaining_burst_time == processToRun.remaining_burst_time && 
					pcbToRemove->priority == processToRun.priority &&
					pcbToRemove->arrival == processToRun.arrival &&
					pcbToRemove->started == processToRun.started)
				{
					if(dyn_array_erase(ready_queue, i)  == false)
					{
						dyn_array_destroy(arrived_queue);
						return false;
					}
				}
			}

			// now we can run the process and calculate the times
			uint32_t waitTime = currentTime - processToRun.arrival;
    		totalWaitingTime += waitTime;

			while(processToRun.remaining_burst_time > 0) // this moves the process through units of time until it is completed
			{
				virtual_cpu(&processToRun);
				currentTime++;
				totalRunTime++;
			}

			uint32_t turnAroundTime = currentTime - processToRun.arrival;
			totalTurnAroundTime += turnAroundTime;
		}
	}

	dyn_array_destroy(arrived_queue);
	
	result->average_waiting_time = (float)totalWaitingTime/numPCBs;
	result->average_turnaround_time = (float)totalTurnAroundTime/numPCBs;
	result->total_run_time = totalRunTime;

	return true;
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
	
	/*
	if (ready_queue == NULL || result == NULL || quantum == 0 || dyn_array_size(ready_queue) == 0) // check for null parameters, bad paramater or no processes to be scheduled
	{
		return false;
	}
	
	uint32_t currentTime = UINT32_MAX;
	uint32_t totalTurnAroundTime = 0;
	uint32_t totalWaitingTime = 0;
	size_t numPCB = dyn_array_size(ready_queue);
	
	
	for(int i = 0; i < numPCB; i++){
	
	    if(dyn_array_at(ready_queue,i) == NULL){
	    
	        return false;
	    }
	    else{
	    
	        ProcessControlBlock_t* pcb = (ProcessControlBlock_t *)dyn_array_at(ready_queue,i);
	        currentTime = currentTime < pcb->arrival ? pcb->arrival : currentTime;
	    }
	
	}
	
	size_t itter = 0;
	
	while(dyn_array_size(ready_queue) != 0){
	
	    size_t Check = dyn_array_size(ready_queue);
	   
	    
	    
	    ProcessControlBlock_t* pcb = (ProcessControlBlock_t *)dyn_array_at(ready_queue,0);
	    if(pcb->arrival > currentTime){
	    
	        if(itter < Check){
	            if(dyn_array_push_back(ready_queue, (void *)pcb)){
	                dyn_array_pop_front(ready_queue);
	                itter++;
	            }
	            else{
	        
	                return false;
	            }
	        }
	        else{
	            for(int i = 0; i < Check; i++){
	                ProcessControlBlock_t* pcb = (ProcessControlBlock_t *)dyn_array_at(ready_queue,i);
	                currentTime = currentTime < pcb->arrival ? pcb->arrival : currentTime;
	            }
	            itter = 0;
	        }
	    }
	    else{
	    
	     uint32_t waitTime = 0;
	    if(!(pcb->started)) {    
	    
	         waitTime = currentTime - pcb->arrival;
	        
	    }
	    else{
	    
	        waitTime = currentTime - (pcb->arrival + (quantum)); //This is wrong, need to adjust using something in pcb (Discuss with Group later)
	    }
	    
	    totalWaitingTime += waitTime;
	        
	        
	        if(pcb->remaining_burst_time > quantum){
	            for(int i < 0; i < quantum; i++){
	                pcb-> started = true;
	                virtual_cpu(pcb);
		            currentTime++;
		            
	            }
	        }
	        else{
	            while(pcb->remaining_burst_time > 0) // this moves the process through units of time until it is completed
			    {
				    virtual_cpu(pcb);
				    currentTime++;
			    }
			    totalTurnAroundTime += currentTime - pcb->arrival;
	            dyn_array_pop_front(ready_queue);
	            
	        }
	    }
	}
	
	//Figure out the results
	
	result->average_waiting_time = totalWaitingTime/numPCB;
	result->average_turnaround_time = totalTurnAroundTime/numPCB;
	result->total_run_time = currentTime;
	
	*/
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
		/*if (numPCBs < 0) // always false, waiting on response from Bipin about "bad" data
		{
			fclose(fptr);
			return NULL;	
		}*/

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
				/*if (pcb.remaining_burst_time < 0 || pcb.priority <= 0 || pcb.arrival < 0) // always false, waiting on response from Bipin about "bad" data
				{
					dyn_array_destroy(pcbArray);
					fclose(fptr);
					return NULL;	
				}*/

				if (pcb.priority == 0)
				{
					dyn_array_destroy(pcbArray);
					fclose(fptr);
					return NULL;
				}

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

		uint32_t extraData;

		if (fread(&extraData, sizeof(uint32_t), 1, fptr) == 1)
		{
			//binary file contains more data than it should
			dyn_array_destroy(pcbArray);
			fclose(fptr);
			return NULL;
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
