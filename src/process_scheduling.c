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
	
	if (ready_queue == NULL || result == NULL || quantum == 0 || dyn_array_size(ready_queue) == 0) // check for null parameters, bad paramater or no processes to be scheduled
	{
		return false;
	}
	
	uint32_t currentTime = UINT32_MAX; //Set starting varibles
	uint32_t totalTurnAroundTime = 0;
	uint32_t totalWaitingTime = 0;
	
	size_t numPCB = dyn_array_size(ready_queue); //Counts number of processes
	
	
	for(size_t i = 0; i < numPCB; i++){ //Creates a foor loop to check for null processes and set current time
	
	    if(dyn_array_at(ready_queue,i) == NULL){ 
	    
	        return false;
	    }
	    else{
	        ProcessControlBlock_t* pcb = (ProcessControlBlock_t *)dyn_array_at(ready_queue,i); //Checks the processes and if its arrival time is less than the current time, set the current time to the fastest arrival times
	        if (pcb->priority <= 0) // checks for bad priority
			{
				return false;
			}
			currentTime = currentTime > pcb->arrival ? pcb->arrival : currentTime;
	    }
	
	}
	
	size_t itter = 0; //Creates a itteration variable to use in the loop
	
	while(dyn_array_size(ready_queue) != 0){ //Keeps the loop going why the size is greater than zero
	
	    size_t Check = dyn_array_size(ready_queue); //Gets the size of the array
	   
	    
	    
	    ProcessControlBlock_t* pcb = (ProcessControlBlock_t *)dyn_array_at(ready_queue,0); //Gets the first element
	    
	    if(pcb->arrival > currentTime){ //Check if its arrived so far
	    
	        if(itter < Check){ //If the itterator is less than check
	        
	        
	            if(dyn_array_push_back(ready_queue, (void *)pcb)){ //tries to push the process in the back of the queue
	            
	            
	                dyn_array_pop_front(ready_queue); //Pops the front if successful
	                itter++; //Increment itteration
	                
	            }
	            
	            
	            else{ //if returning it to the queue fails, return false
	        
	                return false;
	            }
	        }
	        else{//If the itterator is equal to check, meaning all processes have been checked
	        
	            currentTime = pcb->arrival; //Sets the current time to the current processes arrival
	            
	            for(size_t i = 0; i < Check; i++){ //Create a foor loop to cycle through all processes
	            
	                pcb = (ProcessControlBlock_t *)dyn_array_at(ready_queue,i);
	                
	                currentTime = currentTime > pcb->arrival ? pcb->arrival : currentTime; //Check all the processes for the earliest arrival time
	            }
	            itter = 0; //Reset the itterater
	        }
	    }
	    
	    
	    else{//If the process has arrived
	    
	    itter = 0; //Reset the itterator
	    
	    
	    uint32_t waitTime = 0; //Create a local wait time
	    
	    
	    if(!(pcb->started)) {  //If the process has not started  
	    
	         waitTime = currentTime - pcb->arrival; //Wait time is the current minus arrival
	        
	    }
	    else{
	    
	        waitTime = currentTime - (pcb->arrival + (pcb->times_processed * quantum)); //Other wise wait time is current minus arrival minus the number of times processes times the quantum (C - (A + (Q*T)))
	    }
	        
	        if(pcb->remaining_burst_time > quantum){ //If burst time is greater than the quantum
	        
	        pcb-> started = true; //Set the started to true
	        
	        pcb->times_processed++; //Increment the processes times processed
	        
	            for(size_t j = 0; j < quantum; j++){ //Create a for loop for times to run the CPU
	                
	                virtual_cpu(pcb); //Run the CPU
	                
		            currentTime++;//Increment the times
		            result->total_run_time++;
	            }
	            if(dyn_array_push_back(ready_queue, (void *)pcb)){ //Try to push the process back on the stack
	            
	                dyn_array_pop_front(ready_queue); //If suceessful remove the first emelement
	            }
	            else{
	            
	                return false; //If theres an error adding it back, return false;
	            }
	            
	        }
	        
	        
	        else{ //If the burst time is less than the quantum
	        
	            while(pcb->remaining_burst_time > 0) // this moves the process through units of time until it is completed
			    {
				    virtual_cpu(pcb); //Run the cpu
				    
				    currentTime++;//Increment the times
				    result->total_run_time++;
			    }
			    
			    totalTurnAroundTime += currentTime - pcb->arrival; //Add the turnaround time to the result varible
			   
			    totalWaitingTime += waitTime; //Add the wait time to the result varible
			    
	            dyn_array_pop_front(ready_queue); //Remove the first element
	            
	        }
	    }
	}
	
	//Figure out the results
	
	result->average_waiting_time = totalWaitingTime/numPCB;
	result->average_turnaround_time = totalTurnAroundTime/numPCB;
	
	return true;
	
	
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
	if(ready_queue == NULL || result == NULL || dyn_array_empty(ready_queue))
    {
        return false;
    }
    //Change this 
    //ProcessControlBlock_t *pcb = (ProcessControlBlock_t *)dyn_array_export(ready_queue); //DEBUG
    if (dyn_array_sort(ready_queue, compareByArrival) == false)
	{
		return false;
	}

	ProcessControlBlock_t* pcb = (ProcessControlBlock_t*)dyn_array_front(ready_queue);
	if (pcb == NULL)
	{
		return false;
	}
	
	
    uint32_t current_time = 0;
    uint32_t total_waiting_time = 0;
    uint32_t total_turnaround_time = 0;
    
    
    size_t length = dyn_array_size(ready_queue);
    uint32_t remaining_time[length];
    bool started[length];
    memset(started, 0, sizeof(started));
    size_t num_completed = 0;
    
    //get the total time needed to run all processes
    for (size_t i = 0; i < length; i++) 
    {
        if(pcb[i].priority <= 0)
        {
            return false;
        }
        remaining_time[i] = pcb[i].remaining_burst_time;
        result->total_run_time += pcb[i].remaining_burst_time;
    }
    
    
    
    
    while(num_completed < length)
    {
        uint32_t  min_remaining_time = UINT32_MAX;
        uint32_t shortest_loc = 0;
        bool shortest_found = false;
        for(size_t i = 0; i < length; i++)
        {
            if (pcb[i].arrival <= current_time && remaining_time[i] > 0)
            {
                if(remaining_time[i] < min_remaining_time)
                {
                    min_remaining_time = remaining_time[i];
                    shortest_loc = i;
                    shortest_found = true;
                }
                
            }
        }
        
        
        if (!shortest_found)
        {
            current_time++;
            continue;
        }
        
        if(!started[shortest_loc])
        {
            started[shortest_loc] = true;
            total_waiting_time += current_time - pcb[shortest_loc].arrival;
            pcb->started = true;
        }
        remaining_time[shortest_loc]--;
        current_time++;
        if (remaining_time[shortest_loc] == 0) 
        {
            num_completed++;
            total_turnaround_time += current_time - pcb[shortest_loc].arrival;
        }
    }//dont count idle time 
    
    result->average_waiting_time = (float)total_waiting_time / length;
    result->average_turnaround_time = (float)total_turnaround_time / length;
    


	return true;
}
