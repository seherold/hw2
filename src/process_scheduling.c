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


int compareByArrival(const void *a, const void *b) // creates a custom comparator function to compare based on arrival time
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

	size_t numPCBs = dyn_array_size(ready_queue); // gets the number of processes

	if (dyn_array_sort(ready_queue, compareByArrival) == false) // sort by arrival
	{
		return false; // if dyn_array_sort fails, the algorithm fails
	}

	while (dyn_array_size(ready_queue) > 0) // for all of the processes in the queue
	{

		ProcessControlBlock_t processToRun; // make a local pcb variable to store the process we want to run

		if (dyn_array_extract_front(ready_queue, &processToRun) == false) // grabs the pcb and removes it from the ready_queue
		{
			return false; // if dyn_array_extract_front fails, the algorithm fails
		} // now we have the process we want to run

		if (currentTime <= processToRun.arrival) // ensures that the process has arrived
		{
			currentTime = processToRun.arrival; // we don't necessarily care what the first arrival time is, we've already sorted by arrival so the one with the shortest arrival time should be first
		}

		uint32_t waitTime = currentTime - processToRun.arrival; // time between arrival of the process and the first time the process is scheduled to run on the CPU which is the current time right before we run the process
    	totalWaitingTime += waitTime;

		while(processToRun.remaining_burst_time > 0) // this moves the process through units of time until it is completed
		{
			virtual_cpu(&processToRun); // decrement remaining_burst_time
			currentTime++; // keep current time tracker up to date
			totalRunTime++; // sums up all the burst times
		}

		uint32_t turnAroundTime = currentTime - processToRun.arrival; // the time a process takes to complete (from arrival to completion), the current time after a process completes is it's completion time
		totalTurnAroundTime += turnAroundTime;
	}

	result->average_waiting_time = (float)totalWaitingTime/numPCBs;
	result->average_turnaround_time = (float)totalTurnAroundTime/numPCBs;
	result->total_run_time = totalRunTime;

	return true;
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

	if (dyn_array_sort(ready_queue, compareByArrival) == false) // this will be used to get the next arrival time if there are gaps in arrival time
	{
		return false; // if this operation fails, scheduling algorithm fails
	}

	int minBTIndex = -1;
	int noProcessArrivedFlag = 1;

	while (dyn_array_size(ready_queue) > 0) // while we still have processes to run
	{

		ProcessControlBlock_t* pcbWithMinBT = NULL;
		minBTIndex = -1;
		noProcessArrivedFlag = 1;

		for (size_t i = 0; i < dyn_array_size(ready_queue); i++)
		{
			ProcessControlBlock_t* pcb = (ProcessControlBlock_t *)dyn_array_at(ready_queue,i);

			if (currentTime >= pcb->arrival) // if a process has arrived, the current time is greater than or equal to the process arrival time
			{
				if(pcbWithMinBT == NULL || pcb->remaining_burst_time < pcbWithMinBT->remaining_burst_time) // ensures that at least one process gets selected
				{
					minBTIndex = i;
					pcbWithMinBT = pcb;
					noProcessArrivedFlag = 0;
				}
			}
		}

		if (noProcessArrivedFlag)
		{
			ProcessControlBlock_t* pcbNextArrived = (ProcessControlBlock_t *)dyn_array_front(ready_queue); 
			currentTime = pcbNextArrived->arrival; // set currentTime to the next smallest arrival time you have in the ready_queue, this is the process that has arrived next, we want to start running it
		}
		else // the alternative is one or more processes have arrived
		{
			ProcessControlBlock_t processToRun; // temporary variable to hold pcb we want to run

			if (dyn_array_extract(ready_queue, minBTIndex, &processToRun) == false)
			{
				return false; // scheduling algorithm fails
			}

			// now we can run the process and calculate the times
			uint32_t waitTime = currentTime - processToRun.arrival; // time between arrival of the process and the first time the process is scheduled to run on the CPU which is the current time right before we run the process
    		totalWaitingTime += waitTime;

			while(processToRun.remaining_burst_time > 0) // this moves the process through units of time until it is completed
			{
				virtual_cpu(&processToRun); // decrement remaining_burst_time
				currentTime++; // keep current time tracker up to date
				totalRunTime++; // sums up all the burst times
			}

			uint32_t turnAroundTime = currentTime - processToRun.arrival; // the time a process takes to complete (from arrival to completion), the current time after a process completes is it's completion time
			totalTurnAroundTime += turnAroundTime;
		}
	}
	
	result->average_waiting_time = (float)totalWaitingTime/numPCBs;
	result->average_turnaround_time = (float)totalTurnAroundTime/numPCBs;
	result->total_run_time = totalRunTime;

	return true;
}



// Runs the Priority algorithm over the incoming ready_queue
// \param ready queue a dyn_array of type ProcessControlBlock_t that contain be up to N elements
// \param result used for shortest job first stat tracking \ref ScheduleResult_t
// \return true if function ran successful else false for an error
bool priority(dyn_array_t *ready_queue, ScheduleResult_t *result) 
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

	if (dyn_array_sort(ready_queue, compareByArrival) == false) // this will be used to get the next arrival time if there are gaps in arrival time
	{
		return false; // if this operation fails, scheduling algorithm fails
	}

	int highestPIndex = -1;
	int noProcessArrivedFlag = 1;

	while (dyn_array_size(ready_queue) > 0) // while we still have processes to run
	{

		ProcessControlBlock_t* pcbWithMaxP = NULL;
		highestPIndex = -1;
		noProcessArrivedFlag = 1;

		for (size_t i = 0; i < dyn_array_size(ready_queue); i++)
		{
			ProcessControlBlock_t* pcb = (ProcessControlBlock_t *)dyn_array_at(ready_queue,i);

			if (currentTime >= pcb->arrival) // if a process has arrived, the current time is greater than or equal to the process arrival time
			{
				if(pcbWithMaxP == NULL || pcb->priority < pcbWithMaxP->priority) // ensures that at least one process gets selected
				{
					highestPIndex = i;
					pcbWithMaxP = pcb;
					noProcessArrivedFlag = 0;
				}
			}
		}

		if (noProcessArrivedFlag)
		{
			ProcessControlBlock_t* pcbNextArrived = (ProcessControlBlock_t *)dyn_array_front(ready_queue); 
			currentTime = pcbNextArrived->arrival; // set currentTime to the next smallest arrival time you have in the ready_queue, this is the process that has arrived next, we want to start running it
		}
		else // the alternative is one or more processes have arrived
		{
			ProcessControlBlock_t processToRun; // temporary variable to hold pcb we want to run

			if (dyn_array_extract(ready_queue, highestPIndex, &processToRun) == false)
			{
				return false; // scheduling algorithm fails
			}

			// now we can run the process and calculate the times
			uint32_t waitTime = currentTime - processToRun.arrival; // time between arrival of the process and the first time the process is scheduled to run on the CPU which is the current time right before we run the process
    		totalWaitingTime += waitTime;

			while(processToRun.remaining_burst_time > 0) // this moves the process through units of time until it is completed
			{
				virtual_cpu(&processToRun); // decrement remaining_burst_time
				currentTime++; // keep current time tracker up to date
				totalRunTime++; // sums up all the burst times
			}

			uint32_t turnAroundTime = currentTime - processToRun.arrival; // the time a process takes to complete (from arrival to completion), the current time after a process completes is it's completion time
			totalTurnAroundTime += turnAroundTime;
		}
	}
	
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
			currentTime = currentTime > pcb->arrival ? pcb->arrival : currentTime;
	    }
	
	}

	if (dyn_array_sort(ready_queue, compareByArrival) == false) //Sorts the queue by arrival time
	{
		return false;
	}
	
	dyn_array_t* work_queue = dyn_array_create((int)numPCB, sizeof(ProcessControlBlock_t), NULL); //Creates array to hold working elements, (these are only ones that have arrived)
	dyn_array_t* int_queue = dyn_array_create((int)numPCB, sizeof(uint32_t), NULL); //Creates an array to hold ints that keep track of 
	
	
	for(size_t i = 0; i < numPCB; i++){ //Puts the first elements into the working array
	    ProcessControlBlock_t* pcb = (ProcessControlBlock_t*)malloc(sizeof(ProcessControlBlock_t));
	    dyn_array_extract_front(ready_queue, pcb); //gets the element
	    if(currentTime == pcb->arrival){
	    
	         if(dyn_array_push_back(work_queue, (void *)pcb)){ //tries to push the process in the back of the queue
	         
	                uint32_t j = 0;
	                uint32_t *pointer = &j;
	                if(dyn_array_push_back(int_queue, (void *)pointer)){ //tries to push the process in the back of the queue
	                
	                }
	                else {

	                    return false;
	                
	                }
	            }
	            else{
	       
	                return false;
	        
	               }
	    
	    }
	    else{
	        
	                    if(dyn_array_push_back(ready_queue, (void *)pcb)){ //tries to push the process in the back of the queue
	         
	                    }
	                    else{
	         
	                        return false;
	         
	                    }
	    
	                }  
	
	}
	
	
	while(dyn_array_size(ready_queue) != 0 || dyn_array_size(work_queue) != 0){ //Keeps the loop going why the size is greater than zero
	
	    size_t Check = dyn_array_size(work_queue); //Gets the size of the array
	    size_t RCheck = dyn_array_size(ready_queue); //Gets the size of the array
	    
	    if(Check > 0){
	    
	    ProcessControlBlock_t* pcb = (ProcessControlBlock_t*)malloc(sizeof(ProcessControlBlock_t));
	    dyn_array_extract_front(work_queue, pcb); //Gets the first element
	    uint32_t* inter = (uint32_t*)malloc(sizeof(uint32_t));
	    dyn_array_extract_front(int_queue, inter); //Gets the first element
	    
	    
	    
	    
	    
	    uint32_t waitTime = 0; //Create a local wait time
	    
	    
	    if(!(pcb->started)) {  //If the process has not started  
	    
	         waitTime = currentTime - pcb->arrival; //Wait time is the current minus arrival
	        
	    }
	    else{
	    
	        waitTime = currentTime - (pcb->arrival + (*inter * quantum)); //Other wise wait time is current minus arrival minus the number of times processes times the quantum (C - (A + (Q*T)))
	    }

		//printf("\nCurrent time: %d\n", currentTime);
		//printf("\nWait time: %d\n", waitTime);
		//printf("\nRemaining burst time: %d\n", pcb->remaining_burst_time);
		//printf("\nitter: %d\n", *inter);

	        
	        (*inter)++;
	        if(pcb->remaining_burst_time > quantum){ //If burst time is greater than the quantum
	        
	            pcb-> started = true; //Set the started to true
	        
	        
	            for(size_t j = 0; j < quantum; j++){ //Create a for loop for times to run the CPU
	                
	                virtual_cpu(pcb); //Run the CPU
	                
		            currentTime++;//Increment the times
		            (result->total_run_time)++;
		            
		            
		            for(size_t p = 0; p < RCheck; p++){ //Goes through ready queue to add new arrivals
		            
		                ProcessControlBlock_t* pcb3 = (ProcessControlBlock_t*)malloc(sizeof(ProcessControlBlock_t));
	                    dyn_array_extract_front(ready_queue, pcb3); //Gets the first element of ready queue
	                    if(pcb3->arrival == currentTime){
	                    
	                        if(dyn_array_push_back(work_queue, (void *)pcb3)){ //tries to push the process in the back of the queue
	         
	                            uint32_t m = 0;
	                            uint32_t *pointer = &m;
	                            if(dyn_array_push_back(int_queue, (void *)pointer)){ //tries to push the process in the back of the queue
	                
	                            }
	                            else {
	                
	                                return false;
	                
	                            }
	                        }
	                        else{
	       
	                            return false;
	        
	                        }
	                    
	                    }
	                    else{
	                    
	                        if(dyn_array_push_back(ready_queue, (void *)pcb3)){ //tries to push the process in the back of the queue
	                
	                        }
	                        else {
	                
	                            return false;
	                
	                        }
	                    
	                    }
		                
		            
		            }
		            
		            
	            }
	            if(dyn_array_push_back(work_queue, (void *)pcb)){ //Try to push the process back on the stack
	            
	                if(dyn_array_push_back(int_queue, (void *)inter)){ //Try to push the int back on the stack
	            
	  
	                }
	                else{
	            
	                    return false; //If theres an error adding it back, return false;
	                }
	  
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
				    
				    for(size_t p = 0; p < RCheck; p++){ //Goes through ready queue to add new arrivals
		            
		                ProcessControlBlock_t* pcb3 = (ProcessControlBlock_t*)malloc(sizeof(ProcessControlBlock_t)); 
	                    dyn_array_extract_front(ready_queue, pcb3); //Gets the first element of ready queue
	                    if(pcb3->arrival == currentTime){
	                    
	                        if(dyn_array_push_back(work_queue, (void *)pcb3)){ //tries to push the process in the back of the queue
	         
	                            uint32_t j = 0;
	                            uint32_t *pointer = &j;
	                            if(dyn_array_push_back(int_queue, (void *)pointer)){ //tries to push the process in the back of the queue
	                
	                            }
	                            else {
	                
	                                return false;
	                
	                            }
	                        }
	                        else{
	       
	                            return false;
	        
	                        }
	                    
	                    }
	                    else{
	                    
	                        if(dyn_array_push_back(ready_queue, (void *)pcb3)){ //tries to push the process in the back of the queue
	                
	                        }
	                        else {
	                
	                            return false;
	                
	                        }
	                    
	                    }
		                
		            
		            }
				    
			    }
			    
			    totalTurnAroundTime += currentTime - pcb->arrival; //Add the turnaround time to the result varible
			   
			    totalWaitingTime += waitTime; //Add the wait time to the result varible
				//printf("\nCompletion Time: %d\n", currentTime);
			    //printf("\nTotal Waiting Time: %d\n", totalWaitingTime);
				//printf("\nTotal Run Time: %ld\n",  result->total_run_time);
				//printf("\nTotal TAT: %d\n", totalTurnAroundTime);
	            
	        }
	    
	    }
	    else{
	    
	            ProcessControlBlock_t* pcb = (ProcessControlBlock_t *)dyn_array_at(ready_queue,0); //Gets the first element
	            currentTime = pcb->arrival; //Sets the current time to the current processes arrival
	            
	            
	            
	            
	            for(size_t k = 0; k < RCheck; k++){ //Puts the first elements into the working array
	                ProcessControlBlock_t* pcb2 = (ProcessControlBlock_t*)malloc(sizeof(ProcessControlBlock_t)); 
	                dyn_array_extract_front(ready_queue, pcb2); //gets the element
	                if(currentTime == pcb2->arrival){
	    
	                    if(dyn_array_push_back(work_queue, (void *)pcb2)){ //tries to push the process in the back of the queue
	         
	                        uint32_t j = 0;
	                        uint32_t *pointer = &j;
	                
	                            if(dyn_array_push_back(int_queue, (void *)pointer)){ //tries to push the process in the back of the queue
	                
	                            }
	                            else {
	                
	                                return false;
	                
	                            }
	                   }
	                    else{
	       
	                        return false;
	        
	                    }
	    
	                }
	                else{
	        
	                    if(dyn_array_push_back(ready_queue, (void *)pcb2)){ //tries to push the process in the back of the queue
	         
	                    }
	                    else{
	         
	                        return false;
	         
	                    }
	    
	                }  
	
	            }
	    
	    
	    
	        }
	    }
	
	//Figure out the results
	
	result->average_waiting_time = (float)totalWaitingTime/numPCB;
	result->average_turnaround_time = (float)totalTurnAroundTime/numPCB;
	
	return true;
	
	
}


// Reads the PCB burst time values from the binary file into ProcessControlBlock_t remaining_burst_time field
// for N number of PCB burst time stored in the file.
// \param input_file the file containing the PCB burst times
// \return a populated dyn_array of ProcessControlBlocks if function ran successful else NULL for an error
dyn_array_t *load_process_control_blocks(const char *input_file) 
{
	if (input_file == NULL) // check for invalid parameters
	{
		return NULL;
	}

	FILE* fptr = fopen(input_file, "rb"); // opening file given to function

	if (fptr == NULL) // if opening the file fails

	{
		return NULL; // load_process_control_blocks fails and returns NULL
	}

	uint32_t numPCBs;

	if (fread(&numPCBs, sizeof(uint32_t), 1, fptr) == 1) // if the read of the first uint32 was successful we have the number of processes
	{

		dyn_array_t* pcbArray = dyn_array_create(numPCBs, sizeof(ProcessControlBlock_t), NULL); // creating the dyn_array we are about to fill with processes created from the file

		if (pcbArray == NULL) // if dyn_array_create fails
		{
			fclose(fptr); // close the file
			return NULL; // load_process_control_blocks fails and returns NULL
		}

		for (uint32_t i = 0; i < numPCBs; i++) // for the number of processes we think are in the file
		{
			ProcessControlBlock_t pcb; // create a new pcb

			if (fread(&pcb.remaining_burst_time, sizeof(uint32_t), 1, fptr) == 1 &&
			fread(&pcb.priority, sizeof(uint32_t), 1, fptr) == 1 &&
			fread(&pcb.arrival, sizeof(uint32_t), 1, fptr) == 1) // read the data in the pcb struct, if all the reads were successful
			{
				if(dyn_array_push_back(pcbArray,&pcb) == false) // try to put the pcb onto the array, if this fails
				{
					dyn_array_destroy(pcbArray); // clean up allocations
					fclose(fptr); // close the file
					return NULL; // load_process_control_blocks fails and returns NULL
				}
			}
			else // reading in the data to the pcb failed
			{
				dyn_array_destroy(pcbArray); // clean up allocations
				fclose(fptr); // close the file
				return NULL; // load_process_control_blocks fails and returns NULL
			}
		}

		// the following code checks that the file does not data for more than N pcbs
		uint32_t extraData;

		if (fread(&extraData, sizeof(uint32_t), 1, fptr) == 1)
		{
			//binary file contains more data than it should
			dyn_array_destroy(pcbArray); // clean up allocations
			fclose(fptr); // close the file
			return NULL; // load_process_control_blocks fails and returns NULL
		}
		
		fclose(fptr); // close the file
		return pcbArray; // if we get to here and haven't returned NULL we have successfully populated dyn_array with ProcessControlBlocks
	}
	else // reading for N failed
	{
		fclose(fptr); // close the file
		return NULL; // load_process_control_blocks fails and returns NULL
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

    if (dyn_array_sort(ready_queue, compareByArrival) == false)
	{
		return false;
	}

	ProcessControlBlock_t* pcb = (ProcessControlBlock_t*)dyn_array_front(ready_queue);
	if (pcb == NULL)
	{
		return false;
	}
	
	//Intialize variables to return to result
    uint32_t current_time = 0;
    uint32_t total_waiting_time = 0;
    uint32_t total_turnaround_time = 0;
    
    //Get size of the array and create arrays to keep track of times
    size_t length = dyn_array_size(ready_queue);
    
	uint32_t original_burst_time[length];
    
    size_t num_completed = 0;
    uint32_t waiting_time[length];
    //get the total time needed to run all processes and the remaining burst time for each process
    for (size_t i = 0; i < length; i++) 
    {
        if(pcb[i].priority <= 0)
        {
            return false;
        }
		original_burst_time[i] = pcb[i].remaining_burst_time;
        result->total_run_time += pcb[i].remaining_burst_time;
    }
    
    
    
    //Loop until all processes are completed 
    while(num_completed < length)
    {
		//Reset variables 
        uint32_t  min_remaining_time = UINT32_MAX;
        uint32_t shortest_loc = 0;
        bool shortest_found = false;
		//Check to see what processes have arrived and aren't finished then which has the least remaining time left
        for(size_t i = 0; i < length; i++)
        {
            if (pcb[i].arrival <= current_time && pcb[i].remaining_burst_time > 0)
            {
                if(pcb[i].remaining_burst_time < min_remaining_time)
                {
                    min_remaining_time = pcb[i].remaining_burst_time;
                    shortest_loc = i;
                    shortest_found = true;
					
                }
                
            }
        }
        
        //if nothing has arrived advance time
        if (!shortest_found)
        {
            current_time++;
            continue;
        }
        //Start the process if it hasn't started
        if(!pcb[shortest_loc].started)
        {
            pcb[shortest_loc].started = true;
        }
		//Advance the waiting time for all processes that have arrived and not finished that don't have the shortest time
		for (size_t i = 0; i < length; i++)
        {
            if (pcb[i].arrival <= current_time && pcb[i].remaining_burst_time > 0 && i != shortest_loc)
            {
                waiting_time[i]++;
            }
        }
		
        pcb[shortest_loc].remaining_burst_time--;
        current_time++;
		//If the processes is finished update the number of completed processes and calculate TAT and WT
        if (pcb[shortest_loc].remaining_burst_time == 0) 
        {
            num_completed++;
			int turnAroundTime = current_time - pcb[shortest_loc].arrival;
            total_turnaround_time += current_time - pcb[shortest_loc].arrival;
			total_waiting_time += turnAroundTime - original_burst_time[shortest_loc];
        }
    }
    //Calculate averages and return true
    result->average_waiting_time = (float)total_waiting_time / length;
    result->average_turnaround_time = (float)total_turnaround_time / length;
	return true;
}