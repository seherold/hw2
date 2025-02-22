#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
	char* file = argv[1];
	dyn_array_t* ready_queue = load_process_control_blocks(file);
	if (ready_queue == NULL)
	{
		printf("Error loading file\n");
		return EXIT_FAILURE;
	}

	ScheduleResult_t result = {.average_waiting_time = 0, .average_turnaround_time = 0, .total_run_time = 0};

	// Execute your scheduling algorithm to collect the statistics
	char* algorithm = argv[2];
	bool schedulingSuccess;

	if (strncmp(algorithm, FCFS, 4) == 0 && algorithm[4] == '\0')
	{
		schedulingSuccess = first_come_first_serve(ready_queue, &result);
	}
	else if (strncmp(algorithm, SJF, 3) == 0 && algorithm[3] == '\0')
	{
		schedulingSuccess = shortest_job_first(ready_queue, &result);
	}
	else if (strncmp(algorithm, P, 1) == 0 && algorithm[1] == '\0')
	{
		schedulingSuccess = priority(ready_queue, &result);
	}
	else if (strncmp(algorithm, RR, 2) == 0 && algorithm[2] == '\0')
	{
		if (argc < 4)
		{
			dyn_array_destroy(ready_queue);
			return EXIT_FAILURE;
		}
		size_t quantum = (size_t)argv[3];
		schedulingSuccess = round_robin(ready_queue, &result, quantum);
	}
	else if (strncmp(algorithm, SRT, 3) == 0 && algorithm[3] == '\0')
	{
		schedulingSuccess = shortest_remaining_time_first(ready_queue, &result);
	}
	else // algorithm string doesn't match any known algorithm
	{
		dyn_array_destroy(ready_queue);
		printf("Invalid algorithm string\n");
		return EXIT_FAILURE;
	}


	if (schedulingSuccess == false) // if something goes wrong in running the scheduling algorithm
	{
		dyn_array_destroy(ready_queue);
		printf("Scheduling algorithm failed\n");
		return EXIT_FAILURE;
	}


	// Clean up any allocations
	dyn_array_destroy(ready_queue);

	// Report your times to STDOUT using either printf or fprintf (copy these to the README.md file)
	if (printf("Average Waiting Time: %f\n", result.average_waiting_time) < 0 ||
	printf("Average Turnaround Time: %f\n", result.average_turnaround_time) < 0 ||
	printf("Total Run Time: %lu\n", result.total_run_time) < 0) // error writing to STDOUT
	{
		return EXIT_FAILURE;
	}

	FILE *fptr = fopen("../README.md", "a");
    if (fptr == NULL) 
    {
        return EXIT_FAILURE; 
    } 
   
	if (fprintf(fptr, "Average Waiting Time: %f\n", result.average_waiting_time) < 0 ||
	fprintf(fptr, "Average Turnaround Time: %f\n", result.average_turnaround_time) < 0 ||
	fprintf(fptr, "Total Run Time: %lu\n\n", result.total_run_time) < 0) // error writing to README.md
	{
		return EXIT_FAILURE;
	}

    fclose(fptr);

	return EXIT_SUCCESS;
}
