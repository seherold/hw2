#include <stdio.h>
#include <stdlib.h>

#include "dyn_array.h"
#include "processing_scheduling.h"

#define FCFS "FCFS"
#define P "P"
#define RR "RR"
#define SJF "SJF"
#define SRT "SRT"

// Add and comment your analysis code in this function.
// THIS IS NOT FINISHED.
int main(int argc, char **argv) 
{
	if (argc < 3) 
	{
		printf("%s <pcb file> <schedule algorithm> [quantum]\n", argv[0]);
		return EXIT_FAILURE;
	}
	
	// Load process control blocks from binary file passed at the command line into a dyn_array (this is your ready queue)
	char* file = argv[1]; // get binary file
	dyn_array_t* ready_queue = load_process_control_blocks(file); // create ready_queue by loading in the pcbs from the binary file
	if (ready_queue == NULL) // if we couldn't load the pcbs from the file correctly
	{
		printf("Error loading file\n"); // signal error to the user
		return EXIT_FAILURE;
	}

	ScheduleResult_t result = {.average_waiting_time = 0, .average_turnaround_time = 0, .total_run_time = 0};

	// Execute your scheduling algorithm to collect the statistics
	char* algorithm = argv[2]; // get the algorithm string from the command line prompt
	bool schedulingSuccess;

	if (strncmp(algorithm, FCFS, 4) == 0 && algorithm[4] == '\0') // if the algorithm string from the command line prompt is FCFS exactly
	{
		schedulingSuccess = first_come_first_serve(ready_queue, &result); // run first_come_first_serve and collect results
	}
	else if (strncmp(algorithm, SJF, 3) == 0 && algorithm[3] == '\0') // if the algorithm string from the command line prompt is SJF exactly
	{
		schedulingSuccess = shortest_job_first(ready_queue, &result); // run shortest_job_first and collect results
	}
	else if (strncmp(algorithm, P, 1) == 0 && algorithm[1] == '\0') // if the algorithm string from the command line prompt is P exactly
	{
		schedulingSuccess = priority(ready_queue, &result); // run priority and collect results
	}
	else if (strncmp(algorithm, RR, 2) == 0 && algorithm[2] == '\0') // if the algorithm string from the command line prompt is RR exactly
	{
		if (argc < 4) // check that the user gave a quantum, if not, clean up allocations and signal a failure
		{
			printf("Quantum needed for RR scheduling algorithm\n"); // signal error to the user
			dyn_array_destroy(ready_queue); // clean up allocations
			return EXIT_FAILURE;
		}
		size_t quantum = (size_t)argv[3]; // if there is a quantum, collect it
		schedulingSuccess = round_robin(ready_queue, &result, quantum); // run round_robin and collect results
	}
	else if (strncmp(algorithm, SRT, 3) == 0 && algorithm[3] == '\0') // if the algorithm string from the command line prompt is SRT exactly
	{
		schedulingSuccess = shortest_remaining_time_first(ready_queue, &result); // run shortest_remaining_time_first and collect results
	}
	else // algorithm string doesn't match any known algorithm
	{
		dyn_array_destroy(ready_queue); // clean up allocations
		printf("Invalid algorithm string\n"); // signal error to the user
		return EXIT_FAILURE;
	}


	if (schedulingSuccess == false) // if something goes wrong in running the chosen scheduling algorithm
	{
		dyn_array_destroy(ready_queue); // clean up allocations
		printf("Scheduling algorithm failed\n"); // signal error to the user
		return EXIT_FAILURE;
	}


	// No errors with running scheduling algorithm, still need to clean up any allocations
	dyn_array_destroy(ready_queue);

	// Reporting result times to STDOUT
	if (printf("Average Waiting Time: %f\n", result.average_waiting_time) < 0 ||
	printf("Average Turnaround Time: %f\n", result.average_turnaround_time) < 0 ||
	printf("Total Run Time: %lu\n", result.total_run_time) < 0) // error writing to STDOUT
	{
		return EXIT_FAILURE;
	}

	// open the README.md file
	FILE *fptr = fopen("../README.md", "a");
    if (fptr == NULL) // failure in opening README.md
    {
        return EXIT_FAILURE; 
    } 
   
	// write to the README.md file
	if (fprintf(fptr, "Average Waiting Time: %f\n", result.average_waiting_time) < 0 ||
	fprintf(fptr, "Average Turnaround Time: %f\n", result.average_turnaround_time) < 0 ||
	fprintf(fptr, "Total Run Time: %lu\n\n", result.total_run_time) < 0) // error writing to README.md
	{
		fclose(fptr);
		return EXIT_FAILURE;
	}

    fclose(fptr);

	return EXIT_SUCCESS; // if we get to here without hitting an EXIT_FAILURE, we have succeeded
}
