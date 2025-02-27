#include <fcntl.h>
#include <stdio.h>
#include <pthread.h>
#include "gtest/gtest.h"
#include "../include/processing_scheduling.h"

// Using a C library requires extern "C" to prevent function mangling
extern "C"
{
#include <dyn_array.h>
}

#define NUM_PCB 30
#define QUANTUM 5 // Used for Robin Round for process as the run time limit


/*
*  FCFS UNIT TEST CASES
**/

/*
*  Tests related to invalid parameters
**/

// if ready_queue == NULL
TEST (first_come_first_serve, ReadyQueueNULL) 
{
	ScheduleResult_t result = {.average_waiting_time = 0, .average_turnaround_time = 0, .total_run_time = 0};
	EXPECT_EQ(false,first_come_first_serve(NULL, &result));

	EXPECT_EQ(result.total_run_time, 0UL);
	EXPECT_NEAR(result.average_waiting_time, 0.00, 0.01);
	EXPECT_NEAR(result.average_turnaround_time, 0.00, 0.01);
}


// if result == NULL
TEST (first_come_first_serve, ResultNULL) 
{
	ProcessControlBlock_t newPCB1 = {.remaining_burst_time = 5, .priority = 1, .arrival = 0, .started = false};
	ProcessControlBlock_t newPCB2 = {.remaining_burst_time = 3, .priority = 1, .arrival = 0, .started = false};
	ProcessControlBlock_t newPCB3 = {.remaining_burst_time = 8, .priority = 1, .arrival = 0, .started = false};
	
	dyn_array_t* ready_queue = dyn_array_create(3, sizeof(ProcessControlBlock_t), NULL);
	
	dyn_array_push_back(ready_queue, &newPCB1);
	dyn_array_push_back(ready_queue, &newPCB2);
	dyn_array_push_back(ready_queue, &newPCB3);
	
	EXPECT_EQ(false,first_come_first_serve(ready_queue, NULL));
	
	dyn_array_destroy(ready_queue);
}


/*
*  Test related to empty queue
**/

// if dyn_array_size(ready_queue) == 0
TEST (first_come_first_serve, ZeroSizeArray) 
{
	dyn_array_t* ready_queue = dyn_array_create(3, sizeof(ProcessControlBlock_t), NULL);

	// Not adding any ProcessControlBlock_t to ready_queue, no processes to schedule

	ScheduleResult_t result = {.average_waiting_time = 0, .average_turnaround_time = 0, .total_run_time = 0};
	
	EXPECT_EQ(false,first_come_first_serve(ready_queue, &result));

	EXPECT_EQ(result.total_run_time, 0UL);
	EXPECT_NEAR(result.average_waiting_time, 0.00, 0.01);
	EXPECT_NEAR(result.average_turnaround_time, 0.00, 0.01);
	
	dyn_array_destroy(ready_queue);
}

/*
*  Tests related to one process -- how does FCFS handle just one process?
**/

TEST (first_come_first_serve, OneProcess)
{
	ProcessControlBlock_t newPCB1 = {.remaining_burst_time = 5, .priority = 1, .arrival = 0, .started = false};
	
	dyn_array_t* ready_queue = dyn_array_create(1, sizeof(ProcessControlBlock_t), NULL);
	
	dyn_array_push_back(ready_queue, &newPCB1);
	
	ScheduleResult_t result = {.average_waiting_time = 0, .average_turnaround_time = 0, .total_run_time = 0};
	
	EXPECT_EQ(true,first_come_first_serve(ready_queue, &result));
	
	EXPECT_EQ(result.total_run_time, 5UL);
	EXPECT_NEAR(result.average_waiting_time, 0.00, 0.01);
	EXPECT_NEAR(result.average_turnaround_time, 5.00, 0.01);
	
	dyn_array_destroy(ready_queue);
}

/*
*  Tests related to arrival time -- what FCFS is really looking at
**/

TEST (first_come_first_serve, SameArrivalTimes) 
{
	ProcessControlBlock_t newPCB1 = {.remaining_burst_time = 5, .priority = 1, .arrival = 0, .started = false};
	ProcessControlBlock_t newPCB2 = {.remaining_burst_time = 3, .priority = 1, .arrival = 0, .started = false};
	ProcessControlBlock_t newPCB3 = {.remaining_burst_time = 4, .priority = 1, .arrival = 0, .started = false};
	
	dyn_array_t* ready_queue = dyn_array_create(3, sizeof(ProcessControlBlock_t), NULL);
	
	dyn_array_push_back(ready_queue, &newPCB1);
	dyn_array_push_back(ready_queue, &newPCB2);
	dyn_array_push_back(ready_queue, &newPCB3);
	
	ScheduleResult_t result = {.average_waiting_time = 0, .average_turnaround_time = 0, .total_run_time = 0};
	
	EXPECT_EQ(true,first_come_first_serve(ready_queue, &result));
	
	EXPECT_EQ(result.total_run_time, 12UL);
	EXPECT_NEAR(result.average_waiting_time, 4.33, 0.01);
	EXPECT_NEAR(result.average_turnaround_time, 8.33, 0.01);
	
	dyn_array_destroy(ready_queue);
}

TEST (first_come_first_serve, DifferentArrivalTimes) 
{
	ProcessControlBlock_t newPCB1 = {.remaining_burst_time = 5, .priority = 1, .arrival = 2, .started = false};
	ProcessControlBlock_t newPCB2 = {.remaining_burst_time = 3, .priority = 1, .arrival = 0, .started = false};
	ProcessControlBlock_t newPCB3 = {.remaining_burst_time = 4, .priority = 1, .arrival = 4, .started = false};
	
	dyn_array_t* ready_queue = dyn_array_create(3, sizeof(ProcessControlBlock_t), NULL);
	
	dyn_array_push_back(ready_queue, &newPCB1);
	dyn_array_push_back(ready_queue, &newPCB2);
	dyn_array_push_back(ready_queue, &newPCB3);
	
	ScheduleResult_t result = {.average_waiting_time = 0, .average_turnaround_time = 0, .total_run_time = 0};
	
	EXPECT_EQ(true,first_come_first_serve(ready_queue, &result));
	
	EXPECT_EQ(result.total_run_time, 12UL);
	EXPECT_NEAR(result.average_waiting_time, 1.67, 0.01);
	EXPECT_NEAR(result.average_turnaround_time, 5.67, 0.01);
	
	dyn_array_destroy(ready_queue);
}

TEST (first_come_first_serve, ProcessArrivesRightAfterPreviousCompletes) 
{
	ProcessControlBlock_t newPCB1 = {.remaining_burst_time = 5, .priority = 1, .arrival = 0, .started = false};
	ProcessControlBlock_t newPCB2 = {.remaining_burst_time = 3, .priority = 1, .arrival = 5, .started = false};
	ProcessControlBlock_t newPCB3 = {.remaining_burst_time = 4, .priority = 1, .arrival = 8, .started = false};
	
	dyn_array_t* ready_queue = dyn_array_create(3, sizeof(ProcessControlBlock_t), NULL);
	
	dyn_array_push_back(ready_queue, &newPCB1);
	dyn_array_push_back(ready_queue, &newPCB2);
	dyn_array_push_back(ready_queue, &newPCB3);
	
	ScheduleResult_t result = {.average_waiting_time = 0, .average_turnaround_time = 0, .total_run_time = 0};
	
	EXPECT_EQ(true,first_come_first_serve(ready_queue, &result));
	
	EXPECT_EQ(result.total_run_time, 12UL);
	EXPECT_NEAR(result.average_waiting_time, 0.00, 0.01);
	EXPECT_NEAR(result.average_turnaround_time, 4.00, 0.01);
	
	dyn_array_destroy(ready_queue);
}

TEST (first_come_first_serve, GapsInArrivalTimes) 
{
	ProcessControlBlock_t newPCB1 = {.remaining_burst_time = 5, .priority = 1, .arrival = 0, .started = false};
	ProcessControlBlock_t newPCB2 = {.remaining_burst_time = 3, .priority = 1, .arrival = 8, .started = false};
	ProcessControlBlock_t newPCB3 = {.remaining_burst_time = 4, .priority = 1, .arrival = 20, .started = false};
	
	dyn_array_t* ready_queue = dyn_array_create(3, sizeof(ProcessControlBlock_t), NULL);
	
	dyn_array_push_back(ready_queue, &newPCB1);
	dyn_array_push_back(ready_queue, &newPCB2);
	dyn_array_push_back(ready_queue, &newPCB3);
	
	ScheduleResult_t result = {.average_waiting_time = 0, .average_turnaround_time = 0, .total_run_time = 0};
	
	EXPECT_EQ(true,first_come_first_serve(ready_queue, &result));
	
	EXPECT_EQ(result.total_run_time, 12UL);
	EXPECT_NEAR(result.average_waiting_time, 0.00, 0.01);
	EXPECT_NEAR(result.average_turnaround_time, 4.00, 0.01);
	
	dyn_array_destroy(ready_queue);
}

TEST (first_come_first_serve, NoProcessArrivesAtZero) 
{
	ProcessControlBlock_t newPCB1 = {.remaining_burst_time = 5, .priority = 1, .arrival = 2, .started = false};
	ProcessControlBlock_t newPCB2 = {.remaining_burst_time = 3, .priority = 1, .arrival = 4, .started = false};
	ProcessControlBlock_t newPCB3 = {.remaining_burst_time = 4, .priority = 1, .arrival = 6, .started = false};
	
	dyn_array_t* ready_queue = dyn_array_create(3, sizeof(ProcessControlBlock_t), NULL);
	
	dyn_array_push_back(ready_queue, &newPCB1);
	dyn_array_push_back(ready_queue, &newPCB2);
	dyn_array_push_back(ready_queue, &newPCB3);
	
	ScheduleResult_t result = {.average_waiting_time = 0, .average_turnaround_time = 0, .total_run_time = 0};
	
	EXPECT_EQ(true,first_come_first_serve(ready_queue, &result));
	
	EXPECT_EQ(result.total_run_time, 12UL);
	EXPECT_NEAR(result.average_waiting_time, 2.33, 0.01);
	EXPECT_NEAR(result.average_turnaround_time, 6.33, 0.01);
	
	dyn_array_destroy(ready_queue);
}

/*
*  Tests related to burst time, not examined by previous tests -- FCFS should ignore and still sort by arrival time
**/

TEST (first_come_first_serve, ProcessesWithZeroBurstTime)
{
	ProcessControlBlock_t newPCB1 = {.remaining_burst_time = 0, .priority = 1, .arrival = 0, .started = false};
	ProcessControlBlock_t newPCB2 = {.remaining_burst_time = 0, .priority = 1, .arrival = 1, .started = false};
	ProcessControlBlock_t newPCB3 = {.remaining_burst_time = 0, .priority = 1, .arrival = 2, .started = false};
	
	dyn_array_t* ready_queue = dyn_array_create(3, sizeof(ProcessControlBlock_t), NULL);
	
	dyn_array_push_back(ready_queue, &newPCB1);
	dyn_array_push_back(ready_queue, &newPCB2);
	dyn_array_push_back(ready_queue, &newPCB3);
	
	ScheduleResult_t result = {.average_waiting_time = 0, .average_turnaround_time = 0, .total_run_time = 0};
	
	EXPECT_EQ(true,first_come_first_serve(ready_queue, &result));
	
	EXPECT_EQ(result.total_run_time, 0UL);
	EXPECT_NEAR(result.average_waiting_time, 0.00, 0.01);
	EXPECT_NEAR(result.average_turnaround_time, 0.00, 0.01);
	
	dyn_array_destroy(ready_queue);
}

/*
*  Tests related to priority -- FCFS should ignore and still sort by arrival time
**/

TEST (first_come_first_serve, IgnoringDifferentPriorities) 
{
	ProcessControlBlock_t newPCB1 = {.remaining_burst_time = 5, .priority = 1, .arrival = 0, .started = false};
	ProcessControlBlock_t newPCB2 = {.remaining_burst_time = 3, .priority = 2, .arrival = 1, .started = false};
	ProcessControlBlock_t newPCB3 = {.remaining_burst_time = 4, .priority = 3, .arrival = 2, .started = false};
	
	dyn_array_t* ready_queue = dyn_array_create(3, sizeof(ProcessControlBlock_t), NULL);
	
	dyn_array_push_back(ready_queue, &newPCB1);
	dyn_array_push_back(ready_queue, &newPCB2);
	dyn_array_push_back(ready_queue, &newPCB3);
	
	ScheduleResult_t result = {.average_waiting_time = 0, .average_turnaround_time = 0, .total_run_time = 0};
	
	EXPECT_EQ(true,first_come_first_serve(ready_queue, &result));
	
	EXPECT_EQ(result.total_run_time, 12UL);
	EXPECT_NEAR(result.average_waiting_time, 3.33, 0.01);
	EXPECT_NEAR(result.average_turnaround_time, 7.33, 0.01);
	
	dyn_array_destroy(ready_queue);
}

/*
*  Shortest Job First UNIT TEST CASES
**/

/*
*  Tests related to invalid parameters
**/

// if ready_queue == NULL
TEST (shortest_job_first, ReadyQueueNULL) 
{
	ScheduleResult_t result = {.average_waiting_time = 0, .average_turnaround_time = 0, .total_run_time = 0};
	EXPECT_EQ(false,shortest_job_first(NULL, &result));

	EXPECT_EQ(result.total_run_time, 0UL);
	EXPECT_NEAR(result.average_waiting_time, 0.00, 0.01);
	EXPECT_NEAR(result.average_turnaround_time, 0.00, 0.01);
}


// if result == NULL
TEST (shortest_job_first, ResultNULL) 
{
	ProcessControlBlock_t newPCB1 = {.remaining_burst_time = 5, .priority = 1, .arrival = 0, .started = false};
	ProcessControlBlock_t newPCB2 = {.remaining_burst_time = 3, .priority = 1, .arrival = 0, .started = false};
	ProcessControlBlock_t newPCB3 = {.remaining_burst_time = 8, .priority = 1, .arrival = 0, .started = false};
	
	dyn_array_t* ready_queue = dyn_array_create(3, sizeof(ProcessControlBlock_t), NULL);
	
	dyn_array_push_back(ready_queue, &newPCB1);
	dyn_array_push_back(ready_queue, &newPCB2);
	dyn_array_push_back(ready_queue, &newPCB3);
	
	EXPECT_EQ(false,shortest_job_first(ready_queue, NULL));
	
	dyn_array_destroy(ready_queue);
}


/*
*  Test related to empty queue
**/

// if dyn_array_size(ready_queue) == 0
TEST (shortest_job_first, ZeroSizeArray) 
{
	dyn_array_t* ready_queue = dyn_array_create(3, sizeof(ProcessControlBlock_t), NULL);

	// Not adding any ProcessControlBlock_t to ready_queue, no processes to schedule

	ScheduleResult_t result = {.average_waiting_time = 0, .average_turnaround_time = 0, .total_run_time = 0};
	
	EXPECT_EQ(false,shortest_job_first(ready_queue, &result));

	EXPECT_EQ(result.total_run_time, 0UL);
	EXPECT_NEAR(result.average_waiting_time, 0.00, 0.01);
	EXPECT_NEAR(result.average_turnaround_time, 0.00, 0.01);
	
	dyn_array_destroy(ready_queue);
}

/*
*  Tests related to one process -- how does SJF handle just one process?
**/

TEST (shortest_job_first, OneProcess) // tests for just one process
{
	ProcessControlBlock_t newPCB1 = {.remaining_burst_time = 5, .priority = 1, .arrival = 0, .started = false};
	
	dyn_array_t* ready_queue = dyn_array_create(1, sizeof(ProcessControlBlock_t), NULL);
	
	dyn_array_push_back(ready_queue, &newPCB1);
	
	ScheduleResult_t result = {.average_waiting_time = 0, .average_turnaround_time = 0, .total_run_time = 0};
	
	EXPECT_EQ(true,shortest_job_first(ready_queue, &result));
	
	EXPECT_EQ(result.total_run_time, 5UL);
	EXPECT_NEAR(result.average_waiting_time, 0.00, 0.01);
	EXPECT_NEAR(result.average_turnaround_time, 5.00, 0.01);
	
	dyn_array_destroy(ready_queue);
}

/*
*  Tests related to burst time, not examined by previous tests
**/

TEST (shortest_job_first, ProcessesWithZeroBurstTime)
{
	ProcessControlBlock_t newPCB1 = {.remaining_burst_time = 0, .priority = 1, .arrival = 0, .started = false};
	ProcessControlBlock_t newPCB2 = {.remaining_burst_time = 0, .priority = 1, .arrival = 1, .started = false};
	ProcessControlBlock_t newPCB3 = {.remaining_burst_time = 0, .priority = 1, .arrival = 2, .started = false};
	
	dyn_array_t* ready_queue = dyn_array_create(3, sizeof(ProcessControlBlock_t), NULL);
	
	dyn_array_push_back(ready_queue, &newPCB1);
	dyn_array_push_back(ready_queue, &newPCB2);
	dyn_array_push_back(ready_queue, &newPCB3);
	
	ScheduleResult_t result = {.average_waiting_time = 0, .average_turnaround_time = 0, .total_run_time = 0};
	
	EXPECT_EQ(true,shortest_job_first(ready_queue, &result));
	
	EXPECT_EQ(result.total_run_time, 0UL);
	EXPECT_NEAR(result.average_waiting_time, 0.00, 0.01);
	EXPECT_NEAR(result.average_turnaround_time, 0.00, 0.01);
	
	dyn_array_destroy(ready_queue);
}

/*
*  Tests related to arrival time and priority in different combinations -- SJF should ignore and priority and still sort by remaining burst time
**/

TEST (shortest_job_first, DiffArrivalSamePriority) // should do schedule according to SJF
{
	ProcessControlBlock_t newPCB1 = {.remaining_burst_time = 3, .priority = 1, .arrival = 0, .started = false};
	ProcessControlBlock_t newPCB2 = {.remaining_burst_time = 8, .priority = 1, .arrival = 1, .started = false};
	ProcessControlBlock_t newPCB3 = {.remaining_burst_time = 5, .priority = 1, .arrival = 2, .started = false};
	
	dyn_array_t* ready_queue = dyn_array_create(3, sizeof(ProcessControlBlock_t), NULL);
	
	dyn_array_push_back(ready_queue, &newPCB1);
	dyn_array_push_back(ready_queue, &newPCB2);
	dyn_array_push_back(ready_queue, &newPCB3);
	
	ScheduleResult_t result = {.average_waiting_time = 0, .average_turnaround_time = 0, .total_run_time = 0};
	
	EXPECT_EQ(true,shortest_job_first(ready_queue, &result));
	
	EXPECT_EQ(result.total_run_time, 16UL);
	EXPECT_NEAR(result.average_waiting_time, 2.67, 0.01);
	EXPECT_NEAR(result.average_turnaround_time, 8.00, 0.01);
	
	dyn_array_destroy(ready_queue);
}

TEST (shortest_job_first, SameArrivalDiffPriority) // should do schedule according to SJF
{
	ProcessControlBlock_t newPCB1 = {.remaining_burst_time = 3, .priority = 1, .arrival = 0, .started = false};
	ProcessControlBlock_t newPCB2 = {.remaining_burst_time = 8, .priority = 2, .arrival = 0, .started = false};
	ProcessControlBlock_t newPCB3 = {.remaining_burst_time = 5, .priority = 3, .arrival = 0, .started = false};
	
	dyn_array_t* ready_queue = dyn_array_create(3, sizeof(ProcessControlBlock_t), NULL);
	
	dyn_array_push_back(ready_queue, &newPCB1);
	dyn_array_push_back(ready_queue, &newPCB2);
	dyn_array_push_back(ready_queue, &newPCB3);
	
	ScheduleResult_t result = {.average_waiting_time = 0, .average_turnaround_time = 0, .total_run_time = 0};
	
	EXPECT_EQ(true,shortest_job_first(ready_queue, &result));
	
	EXPECT_EQ(result.total_run_time, 16UL);
	EXPECT_NEAR(result.average_waiting_time, 3.67, 0.01);
	EXPECT_NEAR(result.average_turnaround_time, 9.00, 0.01);
	
	dyn_array_destroy(ready_queue);
}

TEST (shortest_job_first, SameArrivalSamePriority) // should do schedule according to SJF
{
	ProcessControlBlock_t newPCB1 = {.remaining_burst_time = 3, .priority = 1, .arrival = 0, .started = false};
	ProcessControlBlock_t newPCB2 = {.remaining_burst_time = 8, .priority = 1, .arrival = 0, .started = false};
	ProcessControlBlock_t newPCB3 = {.remaining_burst_time = 5, .priority = 1, .arrival = 0, .started = false};
	
	dyn_array_t* ready_queue = dyn_array_create(3, sizeof(ProcessControlBlock_t), NULL);
	
	dyn_array_push_back(ready_queue, &newPCB1);
	dyn_array_push_back(ready_queue, &newPCB2);
	dyn_array_push_back(ready_queue, &newPCB3);
	
	ScheduleResult_t result = {.average_waiting_time = 0, .average_turnaround_time = 0, .total_run_time = 0};
	
	EXPECT_EQ(true,shortest_job_first(ready_queue, &result));
	
	EXPECT_EQ(result.total_run_time, 16UL);
	EXPECT_NEAR(result.average_waiting_time, 3.67, 0.01);
	EXPECT_NEAR(result.average_turnaround_time, 9.00, 0.01);
	
	dyn_array_destroy(ready_queue);
}

TEST (shortest_job_first, DiffArrivalDiffPriority) // should do schedule according to SJF
{
	ProcessControlBlock_t newPCB1 = {.remaining_burst_time = 3, .priority = 1, .arrival = 0, .started = false};
	ProcessControlBlock_t newPCB2 = {.remaining_burst_time = 8, .priority = 2, .arrival = 1, .started = false};
	ProcessControlBlock_t newPCB3 = {.remaining_burst_time = 5, .priority = 3, .arrival = 2, .started = false};
	
	dyn_array_t* ready_queue = dyn_array_create(3, sizeof(ProcessControlBlock_t), NULL);
	
	dyn_array_push_back(ready_queue, &newPCB1);
	dyn_array_push_back(ready_queue, &newPCB2);
	dyn_array_push_back(ready_queue, &newPCB3);
	
	ScheduleResult_t result = {.average_waiting_time = 0, .average_turnaround_time = 0, .total_run_time = 0};
	
	EXPECT_EQ(true,shortest_job_first(ready_queue, &result));
	
	EXPECT_EQ(result.total_run_time, 16UL);
	EXPECT_NEAR(result.average_waiting_time, 2.67, 0.01);
	EXPECT_NEAR(result.average_turnaround_time, 8.00, 0.01);
	
	dyn_array_destroy(ready_queue);
}

TEST (shortest_job_first, GapsInArrivalTimes) 
{
	ProcessControlBlock_t newPCB1 = {.remaining_burst_time = 5, .priority = 1, .arrival = 0, .started = false};
	ProcessControlBlock_t newPCB2 = {.remaining_burst_time = 3, .priority = 1, .arrival = 8, .started = false};
	ProcessControlBlock_t newPCB3 = {.remaining_burst_time = 4, .priority = 1, .arrival = 20, .started = false};
	
	dyn_array_t* ready_queue = dyn_array_create(3, sizeof(ProcessControlBlock_t), NULL);
	
	dyn_array_push_back(ready_queue, &newPCB1);
	dyn_array_push_back(ready_queue, &newPCB2);
	dyn_array_push_back(ready_queue, &newPCB3);
	
	ScheduleResult_t result = {.average_waiting_time = 0, .average_turnaround_time = 0, .total_run_time = 0};
	
	EXPECT_EQ(true,shortest_job_first(ready_queue, &result));
	
	EXPECT_EQ(result.total_run_time, 12UL);
	EXPECT_NEAR(result.average_waiting_time, 0.00, 0.01);
	EXPECT_NEAR(result.average_turnaround_time, 4.00, 0.01);
	
	dyn_array_destroy(ready_queue);
}

TEST (shortest_job_first, NoProcessArrivesAtZero) 
{
	ProcessControlBlock_t newPCB1 = {.remaining_burst_time = 5, .priority = 1, .arrival = 2, .started = false};
	ProcessControlBlock_t newPCB2 = {.remaining_burst_time = 3, .priority = 1, .arrival = 4, .started = false};
	ProcessControlBlock_t newPCB3 = {.remaining_burst_time = 4, .priority = 1, .arrival = 6, .started = false};
	
	dyn_array_t* ready_queue = dyn_array_create(3, sizeof(ProcessControlBlock_t), NULL);
	
	dyn_array_push_back(ready_queue, &newPCB1);
	dyn_array_push_back(ready_queue, &newPCB2);
	dyn_array_push_back(ready_queue, &newPCB3);
	
	ScheduleResult_t result = {.average_waiting_time = 0, .average_turnaround_time = 0, .total_run_time = 0};
	
	EXPECT_EQ(true,shortest_job_first(ready_queue, &result));
	
	EXPECT_EQ(result.total_run_time, 12UL);
	EXPECT_NEAR(result.average_waiting_time, 2.33, 0.01);
	EXPECT_NEAR(result.average_turnaround_time, 6.33, 0.01);
	
	dyn_array_destroy(ready_queue);
}



/*
*  Tests related to burst time -- what SJF is really checking
**/

TEST (shortest_job_first, TiesInBurstTimeSameArrival)
{
	ProcessControlBlock_t newPCB1 = {.remaining_burst_time = 4, .priority = 1, .arrival = 0, .started = false};
	ProcessControlBlock_t newPCB2 = {.remaining_burst_time = 4, .priority = 1, .arrival = 0, .started = false};
	ProcessControlBlock_t newPCB3 = {.remaining_burst_time = 6, .priority = 1, .arrival = 0, .started = false};
	
	dyn_array_t* ready_queue = dyn_array_create(10, sizeof(ProcessControlBlock_t), NULL);
	
	dyn_array_push_back(ready_queue, &newPCB1);
	dyn_array_push_back(ready_queue, &newPCB2);
	dyn_array_push_back(ready_queue, &newPCB3);

	ScheduleResult_t result = {.average_waiting_time = 0, .average_turnaround_time = 0, .total_run_time = 0};
	
	EXPECT_EQ(true,shortest_job_first(ready_queue, &result));
	
	EXPECT_EQ(result.total_run_time, 14UL);
	EXPECT_NEAR(result.average_waiting_time, 4.00, 0.01);
	EXPECT_NEAR(result.average_turnaround_time, 8.67, 0.01);
	
	dyn_array_destroy(ready_queue);
}



// small amount of processes
TEST (shortest_job_first, SJFShort)
{
	ProcessControlBlock_t newPCB1 = {.remaining_burst_time = 4, .priority = 2, .arrival = 0, .started = false};
	ProcessControlBlock_t newPCB2 = {.remaining_burst_time = 6, .priority = 1, .arrival = 1, .started = false};
	ProcessControlBlock_t newPCB3 = {.remaining_burst_time = 2, .priority = 3, .arrival = 3, .started = false};
	
	dyn_array_t* ready_queue = dyn_array_create(10, sizeof(ProcessControlBlock_t), NULL);
	
	dyn_array_push_back(ready_queue, &newPCB1);
	dyn_array_push_back(ready_queue, &newPCB2);
	dyn_array_push_back(ready_queue, &newPCB3);

	ScheduleResult_t result = {.average_waiting_time = 0, .average_turnaround_time = 0, .total_run_time = 0};
	
	EXPECT_EQ(true,shortest_job_first(ready_queue, &result));
	
	EXPECT_EQ(result.total_run_time, 12UL);
	EXPECT_NEAR(result.average_waiting_time, 2.00, 0.01);
	EXPECT_NEAR(result.average_turnaround_time, 6.00, 0.01);
	
	dyn_array_destroy(ready_queue);
}


// tests with more than 3 processes
TEST (shortest_job_first, SJFLong)
{
	ProcessControlBlock_t newPCB1 = {.remaining_burst_time = 4, .priority = 2, .arrival = 0, .started = false};
	ProcessControlBlock_t newPCB2 = {.remaining_burst_time = 2, .priority = 1, .arrival = 1, .started = false};
	ProcessControlBlock_t newPCB3 = {.remaining_burst_time = 6, .priority = 3, .arrival = 3, .started = false};
	ProcessControlBlock_t newPCB4 = {.remaining_burst_time = 5, .priority = 5, .arrival = 5, .started = false};
	ProcessControlBlock_t newPCB5 = {.remaining_burst_time = 3, .priority = 4, .arrival = 8, .started = false};
	ProcessControlBlock_t newPCB6 = {.remaining_burst_time = 7, .priority = 7, .arrival = 10, .started = false};
	ProcessControlBlock_t newPCB7 = {.remaining_burst_time = 9, .priority = 8, .arrival = 2, .started = false};
	ProcessControlBlock_t newPCB8 = {.remaining_burst_time = 20, .priority = 10, .arrival = 4, .started = false};
	ProcessControlBlock_t newPCB9 = {.remaining_burst_time = 5, .priority = 3, .arrival = 6, .started = false};
	ProcessControlBlock_t newPCB10 = {.remaining_burst_time = 2, .priority = 1, .arrival = 20, .started = false};
	
	dyn_array_t* ready_queue = dyn_array_create(10, sizeof(ProcessControlBlock_t), NULL);
	
	dyn_array_push_back(ready_queue, &newPCB1);
	dyn_array_push_back(ready_queue, &newPCB2);
	dyn_array_push_back(ready_queue, &newPCB3);
	dyn_array_push_back(ready_queue, &newPCB4);
	dyn_array_push_back(ready_queue, &newPCB5);
	dyn_array_push_back(ready_queue, &newPCB6);
	dyn_array_push_back(ready_queue, &newPCB7);
	dyn_array_push_back(ready_queue, &newPCB8);
	dyn_array_push_back(ready_queue, &newPCB9);
	dyn_array_push_back(ready_queue, &newPCB10);

	ScheduleResult_t result = {.average_waiting_time = 0, .average_turnaround_time = 0, .total_run_time = 0};
	
	EXPECT_EQ(true,shortest_job_first(ready_queue, &result));
	
	EXPECT_EQ(result.total_run_time, 63UL);
	EXPECT_NEAR(result.average_waiting_time, 12.40, 0.01);
	EXPECT_NEAR(result.average_turnaround_time, 18.70, 0.01);
	
	dyn_array_destroy(ready_queue);
}


/*
* Round Robin UNIT TEST CASES
*/
//Null Params
TEST (round_robin, NULLParams)
{
    ScheduleResult_t result = {.average_waiting_time = 0, .average_turnaround_time = 0, .total_run_time = 0};
    EXPECT_EQ(false, round_robin(NULL, &result, QUANTUM)); //NULL Queue
    
    
    
    ProcessControlBlock_t newPCB1 = {.remaining_burst_time = 5, .priority = 0, .arrival = 0, .started = false};
    ProcessControlBlock_t newPCB2 = {.remaining_burst_time = 3, .priority = 0, .arrival = 0, .started = false};
    dyn_array_t* ready_queue = dyn_array_create(2, sizeof(ProcessControlBlock_t), NULL);
    dyn_array_push_back(ready_queue, &newPCB1);
    dyn_array_push_back(ready_queue, &newPCB2);
    
    EXPECT_EQ(false, round_robin(ready_queue, NULL, QUANTUM)); //NULL Result
    
    
    dyn_array_destroy(ready_queue);
}

//Bad Params
TEST (round_robin, BadParams)
{
    ScheduleResult_t result = {.average_waiting_time = 0, .average_turnaround_time = 0, .total_run_time = 0};
    
    
    
    
    dyn_array_t* ready_queue = dyn_array_create(2, sizeof(ProcessControlBlock_t), NULL); //Empty ready_queue
    
    EXPECT_EQ(false, round_robin(ready_queue, &result, 0)); //If Quantum is zero
    
    EXPECT_EQ(false, round_robin(ready_queue, &result, QUANTUM)); //If Queue is empty
    
    
    dyn_array_destroy(ready_queue);
}


/*
*  Tests related to one process -- how does priority handle just one process?
**/

TEST (round_robin, OneProcess) // tests for just one process
{
	ProcessControlBlock_t newPCB1 = {.remaining_burst_time = 5, .priority = 1, .arrival = 0, .started = false};
	
	dyn_array_t* ready_queue = dyn_array_create(1, sizeof(ProcessControlBlock_t), NULL);
	
	dyn_array_push_back(ready_queue, &newPCB1);
	
	ScheduleResult_t result = {.average_waiting_time = 0, .average_turnaround_time = 0, .total_run_time = 0};
	
	EXPECT_EQ(true, round_robin(ready_queue, &result, QUANTUM));
	
	EXPECT_EQ(result.total_run_time, 5UL);
	EXPECT_NEAR(result.average_waiting_time, 0.00, 0.01);
	EXPECT_NEAR(result.average_turnaround_time, 5.00, 0.01);
	
	dyn_array_destroy(ready_queue);
}

/*
*  Tests related to order of processes placed in the queue  
RR should sort by arrival first, then worry about the quantum
So processes arriving "in order" or "out of order" shouldn't make a difference
**/

TEST (round_robin, DiffArrivalInOrderSamePriority) // should do schedule according to RR
{
	ProcessControlBlock_t newPCB1 = {.remaining_burst_time = 5, .priority = 1, .arrival = 0, .started = false};
	ProcessControlBlock_t newPCB2 = {.remaining_burst_time = 8, .priority = 1, .arrival = 1, .started = false};
	ProcessControlBlock_t newPCB3 = {.remaining_burst_time = 3, .priority = 1, .arrival = 2, .started = false};
	
	dyn_array_t* ready_queue = dyn_array_create(3, sizeof(ProcessControlBlock_t), NULL);
	
	dyn_array_push_back(ready_queue, &newPCB1);
	dyn_array_push_back(ready_queue, &newPCB2);
	dyn_array_push_back(ready_queue, &newPCB3);
	
	ScheduleResult_t result = {.average_waiting_time = 0, .average_turnaround_time = 0, .total_run_time = 0};
	
	EXPECT_EQ(true, round_robin(ready_queue, &result, QUANTUM));
	
	EXPECT_EQ(result.total_run_time, 16UL);
	EXPECT_NEAR(result.average_waiting_time, 5.00, 0.01);
	EXPECT_NEAR(result.average_turnaround_time, 10.33, 0.01);
	
	dyn_array_destroy(ready_queue);
}

TEST (round_robin, DiffArrivalOutOfOrderSamePriority) // should do schedule according to RR
{
	ProcessControlBlock_t newPCB1 = {.remaining_burst_time = 5, .priority = 1, .arrival = 1, .started = false};
	ProcessControlBlock_t newPCB2 = {.remaining_burst_time = 8, .priority = 1, .arrival = 0, .started = false};
	ProcessControlBlock_t newPCB3 = {.remaining_burst_time = 3, .priority = 1, .arrival = 2, .started = false};
	
	dyn_array_t* ready_queue = dyn_array_create(3, sizeof(ProcessControlBlock_t), NULL);
	
	dyn_array_push_back(ready_queue, &newPCB1);
	dyn_array_push_back(ready_queue, &newPCB2);
	dyn_array_push_back(ready_queue, &newPCB3);
	
	ScheduleResult_t result = {.average_waiting_time = 0, .average_turnaround_time = 0, .total_run_time = 0};
	
	EXPECT_EQ(true, round_robin(ready_queue, &result, QUANTUM));
	
	EXPECT_EQ(result.total_run_time, 16UL);
	EXPECT_NEAR(result.average_waiting_time, 6.67, 0.01);
	EXPECT_NEAR(result.average_turnaround_time, 12.00, 0.01);
	
	dyn_array_destroy(ready_queue);
}

/*
*  Tests related to arrival time, idle time
**/

TEST (round_robin, GapsInArrivalTimes) 
{
	ProcessControlBlock_t newPCB1 = {.remaining_burst_time = 5, .priority = 1, .arrival = 0, .started = false};
	ProcessControlBlock_t newPCB2 = {.remaining_burst_time = 6, .priority = 1, .arrival = 8, .started = false};
	ProcessControlBlock_t newPCB3 = {.remaining_burst_time = 3, .priority = 1, .arrival = 20, .started = false};
	
	dyn_array_t* ready_queue = dyn_array_create(3, sizeof(ProcessControlBlock_t), NULL);
	
	dyn_array_push_back(ready_queue, &newPCB1);
	dyn_array_push_back(ready_queue, &newPCB2);
	dyn_array_push_back(ready_queue, &newPCB3);
	
	ScheduleResult_t result = {.average_waiting_time = 0, .average_turnaround_time = 0, .total_run_time = 0};
	
	EXPECT_EQ(true,round_robin(ready_queue, &result, QUANTUM));
	
	EXPECT_EQ(result.total_run_time, 14UL);
	EXPECT_NEAR(result.average_waiting_time, 0.00, 0.01);
	EXPECT_NEAR(result.average_turnaround_time, 4.67, 0.01);
	
	dyn_array_destroy(ready_queue);
}

TEST (round_robin, NoProcessArrivesAtZero) 
{
	ProcessControlBlock_t newPCB1 = {.remaining_burst_time = 5, .priority = 1, .arrival = 2, .started = false};
	ProcessControlBlock_t newPCB2 = {.remaining_burst_time = 6, .priority = 1, .arrival = 4, .started = false};
	ProcessControlBlock_t newPCB3 = {.remaining_burst_time = 3, .priority = 1, .arrival = 6, .started = false};
	
	dyn_array_t* ready_queue = dyn_array_create(3, sizeof(ProcessControlBlock_t), NULL);
	
	dyn_array_push_back(ready_queue, &newPCB1);
	dyn_array_push_back(ready_queue, &newPCB2);
	dyn_array_push_back(ready_queue, &newPCB3);
	
	ScheduleResult_t result = {.average_waiting_time = 0, .average_turnaround_time = 0, .total_run_time = 0};
	
	EXPECT_EQ(true,round_robin(ready_queue, &result, QUANTUM));
	
	EXPECT_EQ(result.total_run_time, 14UL);
	EXPECT_NEAR(result.average_waiting_time, 4.00, 0.01);
	EXPECT_NEAR(result.average_turnaround_time, 8.67, 0.01);
	
	dyn_array_destroy(ready_queue);
}

/*
*  Tests related to priority -- RR process should be unaffected by priority
**/

TEST (round_robin, SameArrivalDiffPriority) // should do schedule according to RR
{
	ProcessControlBlock_t newPCB1 = {.remaining_burst_time = 5, .priority = 1, .arrival = 0, .started = false};
	ProcessControlBlock_t newPCB2 = {.remaining_burst_time = 8, .priority = 2, .arrival = 0, .started = false};
	ProcessControlBlock_t newPCB3 = {.remaining_burst_time = 3, .priority = 3, .arrival = 0, .started = false};
	
	dyn_array_t* ready_queue = dyn_array_create(3, sizeof(ProcessControlBlock_t), NULL);
	
	dyn_array_push_back(ready_queue, &newPCB1);
	dyn_array_push_back(ready_queue, &newPCB2);
	dyn_array_push_back(ready_queue, &newPCB3);
	
	ScheduleResult_t result = {.average_waiting_time = 0, .average_turnaround_time = 0, .total_run_time = 0};
	
	EXPECT_EQ(true, round_robin(ready_queue, &result, QUANTUM));
	
	EXPECT_EQ(result.total_run_time, 16UL);
	EXPECT_NEAR(result.average_waiting_time, 6.00, 0.01);
	EXPECT_NEAR(result.average_turnaround_time, 11.33, 0.01);
	
	dyn_array_destroy(ready_queue);
}

TEST (round_robin, DiffArrivalDiffPriority) // should do schedule according to RR
{
	ProcessControlBlock_t newPCB1 = {.remaining_burst_time = 5, .priority = 1, .arrival = 0, .started = false};
	ProcessControlBlock_t newPCB2 = {.remaining_burst_time = 8, .priority = 2, .arrival = 1, .started = false};
	ProcessControlBlock_t newPCB3 = {.remaining_burst_time = 3, .priority = 3, .arrival = 2, .started = false};
	
	dyn_array_t* ready_queue = dyn_array_create(3, sizeof(ProcessControlBlock_t), NULL);
	
	dyn_array_push_back(ready_queue, &newPCB1);
	dyn_array_push_back(ready_queue, &newPCB2);
	dyn_array_push_back(ready_queue, &newPCB3);
	
	ScheduleResult_t result = {.average_waiting_time = 0, .average_turnaround_time = 0, .total_run_time = 0};
	
	EXPECT_EQ(true, round_robin(ready_queue, &result, QUANTUM));
	
	EXPECT_EQ(result.total_run_time, 16UL);
	EXPECT_NEAR(result.average_waiting_time, 5.00, 0.01);
	EXPECT_NEAR(result.average_turnaround_time, 10.33, 0.01);
	
	dyn_array_destroy(ready_queue);
}

/*
*  Tests related to small set of processeses
**/

TEST (round_robin, RRShort)
{
	ProcessControlBlock_t newPCB1 = {.remaining_burst_time = 4, .priority = 2, .arrival = 0, .started = false};
	ProcessControlBlock_t newPCB2 = {.remaining_burst_time = 6, .priority = 1, .arrival = 1, .started = false};
	ProcessControlBlock_t newPCB3 = {.remaining_burst_time = 2, .priority = 3, .arrival = 3, .started = false};
	
	dyn_array_t* ready_queue = dyn_array_create(10, sizeof(ProcessControlBlock_t), NULL);
	
	dyn_array_push_back(ready_queue, &newPCB1);
	dyn_array_push_back(ready_queue, &newPCB2);
	dyn_array_push_back(ready_queue, &newPCB3);

	ScheduleResult_t result = {.average_waiting_time = 0, .average_turnaround_time = 0, .total_run_time = 0};
	
	EXPECT_EQ(true, round_robin(ready_queue, &result, QUANTUM));
	
	EXPECT_EQ(result.total_run_time, 12UL);
	EXPECT_NEAR(result.average_waiting_time, 3.67, 0.01);
	EXPECT_NEAR(result.average_turnaround_time, 7.67, 0.01);
	
	dyn_array_destroy(ready_queue);
}


/*
*  Tests related to large sets of processes
**/

TEST (round_robin, RRLong) // tests with more than 3 processes
{
	ProcessControlBlock_t newPCB1 = {.remaining_burst_time = 4, .priority = 2, .arrival = 0, .started = false};
	ProcessControlBlock_t newPCB2 = {.remaining_burst_time = 2, .priority = 1, .arrival = 1, .started = false};
	ProcessControlBlock_t newPCB3 = {.remaining_burst_time = 6, .priority = 3, .arrival = 3, .started = false};
	ProcessControlBlock_t newPCB4 = {.remaining_burst_time = 5, .priority = 5, .arrival = 5, .started = false};
	ProcessControlBlock_t newPCB5 = {.remaining_burst_time = 3, .priority = 4, .arrival = 8, .started = false};
	ProcessControlBlock_t newPCB6 = {.remaining_burst_time = 7, .priority = 7, .arrival = 10, .started = false};
	ProcessControlBlock_t newPCB7 = {.remaining_burst_time = 9, .priority = 8, .arrival = 2, .started = false};
	ProcessControlBlock_t newPCB8 = {.remaining_burst_time = 20, .priority = 10, .arrival = 4, .started = false};
	ProcessControlBlock_t newPCB9 = {.remaining_burst_time = 5, .priority = 3, .arrival = 6, .started = false};
	ProcessControlBlock_t newPCB10 = {.remaining_burst_time = 2, .priority = 1, .arrival = 20, .started = false};
	
	dyn_array_t* ready_queue = dyn_array_create(10, sizeof(ProcessControlBlock_t), NULL);
	
	dyn_array_push_back(ready_queue, &newPCB1);
	dyn_array_push_back(ready_queue, &newPCB2);
	dyn_array_push_back(ready_queue, &newPCB3);
	dyn_array_push_back(ready_queue, &newPCB4);
	dyn_array_push_back(ready_queue, &newPCB5);
	dyn_array_push_back(ready_queue, &newPCB6);
	dyn_array_push_back(ready_queue, &newPCB7);
	dyn_array_push_back(ready_queue, &newPCB8);
	dyn_array_push_back(ready_queue, &newPCB9);
	dyn_array_push_back(ready_queue, &newPCB10);

	ScheduleResult_t result = {.average_waiting_time = 0, .average_turnaround_time = 0, .total_run_time = 0};
	
	EXPECT_EQ(true, round_robin(ready_queue, &result, QUANTUM));
	
	EXPECT_EQ(result.total_run_time, 63UL);
	EXPECT_NEAR(result.average_waiting_time, 22.80, 0.01);
	EXPECT_NEAR(result.average_turnaround_time, 29.10, 0.01);
	
	dyn_array_destroy(ready_queue);
}



/*
*  Priority UNIT TEST CASES
**/

/*
*  Tests related to invalid parameters
**/

// if ready_queue == NULL
TEST (priority, ReadyQueueNULL) 
{
	ScheduleResult_t result = {.average_waiting_time = 0, .average_turnaround_time = 0, .total_run_time = 0};
	EXPECT_EQ(false,priority(NULL, &result));

	EXPECT_EQ(result.total_run_time, 0UL);
	EXPECT_NEAR(result.average_waiting_time, 0.00, 0.01);
	EXPECT_NEAR(result.average_turnaround_time, 0.00, 0.01);
}


// if result == NULL
TEST (priority, ResultNULL) 
{
	ProcessControlBlock_t newPCB1 = {.remaining_burst_time = 5, .priority = 1, .arrival = 0, .started = false};
	ProcessControlBlock_t newPCB2 = {.remaining_burst_time = 3, .priority = 1, .arrival = 0, .started = false};
	ProcessControlBlock_t newPCB3 = {.remaining_burst_time = 8, .priority = 1, .arrival = 0, .started = false};
	
	dyn_array_t* ready_queue = dyn_array_create(3, sizeof(ProcessControlBlock_t), NULL);
	
	dyn_array_push_back(ready_queue, &newPCB1);
	dyn_array_push_back(ready_queue, &newPCB2);
	dyn_array_push_back(ready_queue, &newPCB3);
	
	EXPECT_EQ(false,priority(ready_queue, NULL));
	
	dyn_array_destroy(ready_queue);
}


/*
*  Test related to empty queue
**/

// if dyn_array_size(ready_queue) == 0
TEST (priority, ZeroSizeArray) 
{
	dyn_array_t* ready_queue = dyn_array_create(3, sizeof(ProcessControlBlock_t), NULL);

	// Not adding any ProcessControlBlock_t to ready_queue, no processes to schedule

	ScheduleResult_t result = {.average_waiting_time = 0, .average_turnaround_time = 0, .total_run_time = 0};
	
	EXPECT_EQ(false,priority(ready_queue, &result));

	EXPECT_EQ(result.total_run_time, 0UL);
	EXPECT_NEAR(result.average_waiting_time, 0.00, 0.01);
	EXPECT_NEAR(result.average_turnaround_time, 0.00, 0.01);
	
	dyn_array_destroy(ready_queue);
}

/*
*  Tests related to one process -- how does priority handle just one process?
**/

TEST (priority, OneProcess) // tests for just one process
{
	ProcessControlBlock_t newPCB1 = {.remaining_burst_time = 5, .priority = 1, .arrival = 0, .started = false};
	
	dyn_array_t* ready_queue = dyn_array_create(1, sizeof(ProcessControlBlock_t), NULL);
	
	dyn_array_push_back(ready_queue, &newPCB1);
	
	ScheduleResult_t result = {.average_waiting_time = 0, .average_turnaround_time = 0, .total_run_time = 0};
	
	EXPECT_EQ(true,priority(ready_queue, &result));
	
	EXPECT_EQ(result.total_run_time, 5UL);
	EXPECT_NEAR(result.average_waiting_time, 0.00, 0.01);
	EXPECT_NEAR(result.average_turnaround_time, 5.00, 0.01);
	
	dyn_array_destroy(ready_queue);
}

/*
*  Tests related to burst time, not examined by previous tests
**/

TEST (priority, ProcessesWithZeroBurstTime)
{
	ProcessControlBlock_t newPCB1 = {.remaining_burst_time = 0, .priority = 1, .arrival = 0, .started = false};
	ProcessControlBlock_t newPCB2 = {.remaining_burst_time = 0, .priority = 1, .arrival = 1, .started = false};
	ProcessControlBlock_t newPCB3 = {.remaining_burst_time = 0, .priority = 1, .arrival = 2, .started = false};
	
	dyn_array_t* ready_queue = dyn_array_create(3, sizeof(ProcessControlBlock_t), NULL);
	
	dyn_array_push_back(ready_queue, &newPCB1);
	dyn_array_push_back(ready_queue, &newPCB2);
	dyn_array_push_back(ready_queue, &newPCB3);
	
	ScheduleResult_t result = {.average_waiting_time = 0, .average_turnaround_time = 0, .total_run_time = 0};
	
	EXPECT_EQ(true,priority(ready_queue, &result));
	
	EXPECT_EQ(result.total_run_time, 0UL);
	EXPECT_NEAR(result.average_waiting_time, 0.00, 0.01);
	EXPECT_NEAR(result.average_turnaround_time, 0.00, 0.01);
	
	dyn_array_destroy(ready_queue);
}



/*
*  Tests related to arrival time
**/

TEST (priority, SamePrioritiesDiffArrival) // basically doing FCFS algorithm
{
	ProcessControlBlock_t newPCB1 = {.remaining_burst_time = 5, .priority = 1, .arrival = 0, .started = false};
	ProcessControlBlock_t newPCB2 = {.remaining_burst_time = 3, .priority = 1, .arrival = 1, .started = false};
	ProcessControlBlock_t newPCB3 = {.remaining_burst_time = 8, .priority = 1, .arrival = 2, .started = false};
	
	dyn_array_t* ready_queue = dyn_array_create(3, sizeof(ProcessControlBlock_t), NULL);
	
	dyn_array_push_back(ready_queue, &newPCB1);
	dyn_array_push_back(ready_queue, &newPCB2);
	dyn_array_push_back(ready_queue, &newPCB3);
	
	ScheduleResult_t result = {.average_waiting_time = 0, .average_turnaround_time = 0, .total_run_time = 0};
	
	EXPECT_EQ(true,priority(ready_queue, &result));
	
	EXPECT_EQ(result.total_run_time, 16UL);
	EXPECT_NEAR(result.average_waiting_time, 3.33, 0.01);
	EXPECT_NEAR(result.average_turnaround_time, 8.67, 0.01);
	
	dyn_array_destroy(ready_queue);
}

TEST (priority, SamePrioritiesSameArrival) // basically doing FCFS algorithm
{
	ProcessControlBlock_t newPCB1 = {.remaining_burst_time = 5, .priority = 1, .arrival = 1, .started = false};
	ProcessControlBlock_t newPCB2 = {.remaining_burst_time = 3, .priority = 1, .arrival = 1, .started = false};
	ProcessControlBlock_t newPCB3 = {.remaining_burst_time = 8, .priority = 1, .arrival = 1, .started = false};
	
	dyn_array_t* ready_queue = dyn_array_create(3, sizeof(ProcessControlBlock_t), NULL);
	
	dyn_array_push_back(ready_queue, &newPCB1);
	dyn_array_push_back(ready_queue, &newPCB2);
	dyn_array_push_back(ready_queue, &newPCB3);
	
	ScheduleResult_t result = {.average_waiting_time = 0, .average_turnaround_time = 0, .total_run_time = 0};
	
	EXPECT_EQ(true,priority(ready_queue, &result));
	
	EXPECT_EQ(result.total_run_time, 16UL);
	EXPECT_NEAR(result.average_waiting_time, 4.33, 0.01);
	EXPECT_NEAR(result.average_turnaround_time, 9.67, 0.01);
	
	dyn_array_destroy(ready_queue);
}

TEST (priority, LateArrivingHighPriority) // tests non-preemption
{
	ProcessControlBlock_t newPCB1 = {.remaining_burst_time = 4, .priority = 2, .arrival = 0, .started = false};
	ProcessControlBlock_t newPCB2 = {.remaining_burst_time = 2, .priority = 1, .arrival = 20, .started = false};
	ProcessControlBlock_t newPCB3 = {.remaining_burst_time = 6, .priority = 3, .arrival = 2, .started = false};
	
	dyn_array_t* ready_queue = dyn_array_create(3, sizeof(ProcessControlBlock_t), NULL);
	
	dyn_array_push_back(ready_queue, &newPCB1);
	dyn_array_push_back(ready_queue, &newPCB2);
	dyn_array_push_back(ready_queue, &newPCB3);
	
	ScheduleResult_t result = {.average_waiting_time = 0, .average_turnaround_time = 0, .total_run_time = 0};
	
	EXPECT_EQ(true,priority(ready_queue, &result));
	
	EXPECT_EQ(result.total_run_time, 12UL);
	EXPECT_NEAR(result.average_waiting_time, 0.67, 0.01);
	EXPECT_NEAR(result.average_turnaround_time, 4.67, 0.01);
	
	dyn_array_destroy(ready_queue);
}


TEST (priority, GapsInArrivalTimes) 
{
	ProcessControlBlock_t newPCB1 = {.remaining_burst_time = 5, .priority = 1, .arrival = 0, .started = false};
	ProcessControlBlock_t newPCB2 = {.remaining_burst_time = 3, .priority = 3, .arrival = 8, .started = false};
	ProcessControlBlock_t newPCB3 = {.remaining_burst_time = 4, .priority = 2, .arrival = 20, .started = false};
	
	dyn_array_t* ready_queue = dyn_array_create(3, sizeof(ProcessControlBlock_t), NULL);
	
	dyn_array_push_back(ready_queue, &newPCB1);
	dyn_array_push_back(ready_queue, &newPCB2);
	dyn_array_push_back(ready_queue, &newPCB3);
	
	ScheduleResult_t result = {.average_waiting_time = 0, .average_turnaround_time = 0, .total_run_time = 0};
	
	EXPECT_EQ(true,priority(ready_queue, &result));
	
	EXPECT_EQ(result.total_run_time, 12UL);
	EXPECT_NEAR(result.average_waiting_time, 0.00, 0.01);
	EXPECT_NEAR(result.average_turnaround_time, 4.00, 0.01);
	
	dyn_array_destroy(ready_queue);
}

TEST (priority, NoProcessArrivesAtZero) 
{
	ProcessControlBlock_t newPCB1 = {.remaining_burst_time = 5, .priority = 1, .arrival = 2, .started = false};
	ProcessControlBlock_t newPCB2 = {.remaining_burst_time = 3, .priority = 3, .arrival = 4, .started = false};
	ProcessControlBlock_t newPCB3 = {.remaining_burst_time = 4, .priority = 2, .arrival = 6, .started = false};
	
	dyn_array_t* ready_queue = dyn_array_create(3, sizeof(ProcessControlBlock_t), NULL);
	
	dyn_array_push_back(ready_queue, &newPCB1);
	dyn_array_push_back(ready_queue, &newPCB2);
	dyn_array_push_back(ready_queue, &newPCB3);
	
	ScheduleResult_t result = {.average_waiting_time = 0, .average_turnaround_time = 0, .total_run_time = 0};
	
	EXPECT_EQ(true,priority(ready_queue, &result));
	
	EXPECT_EQ(result.total_run_time, 12UL);
	EXPECT_NEAR(result.average_waiting_time, 2.67, 0.01);
	EXPECT_NEAR(result.average_turnaround_time, 6.67, 0.01);
	
	dyn_array_destroy(ready_queue);
}

/*
*  Tests related to priority -- what priority is really looking at
**/

TEST (priority, TiesInPrioritySameArrival) // should be actually doing the priority algorithm because each process is given a different priority
{
	ProcessControlBlock_t newPCB1 = {.remaining_burst_time = 4, .priority = 1, .arrival = 0, .started = false};
	ProcessControlBlock_t newPCB2 = {.remaining_burst_time = 2, .priority = 2, .arrival = 0, .started = false};
	ProcessControlBlock_t newPCB3 = {.remaining_burst_time = 6, .priority = 1, .arrival = 0, .started = false};
	
	dyn_array_t* ready_queue = dyn_array_create(3, sizeof(ProcessControlBlock_t), NULL);
	
	dyn_array_push_back(ready_queue, &newPCB1);
	dyn_array_push_back(ready_queue, &newPCB2);
	dyn_array_push_back(ready_queue, &newPCB3);
	
	ScheduleResult_t result = {.average_waiting_time = 0, .average_turnaround_time = 0, .total_run_time = 0};
	
	EXPECT_EQ(true,priority(ready_queue, &result));
	
	EXPECT_EQ(result.total_run_time, 12UL);
	EXPECT_NEAR(result.average_waiting_time, 4.67, 0.01);
	EXPECT_NEAR(result.average_turnaround_time, 8.67, 0.01);
	
	dyn_array_destroy(ready_queue);
}

TEST (priority, DifferentPrioritiesSameArrival) // should be actually doing the priority algorithm because each process is given a different priority
{
	ProcessControlBlock_t newPCB1 = {.remaining_burst_time = 4, .priority = 2, .arrival = 0, .started = false};
	ProcessControlBlock_t newPCB2 = {.remaining_burst_time = 2, .priority = 1, .arrival = 0, .started = false};
	ProcessControlBlock_t newPCB3 = {.remaining_burst_time = 6, .priority = 3, .arrival = 0, .started = false};
	
	dyn_array_t* ready_queue = dyn_array_create(3, sizeof(ProcessControlBlock_t), NULL);
	
	dyn_array_push_back(ready_queue, &newPCB1);
	dyn_array_push_back(ready_queue, &newPCB2);
	dyn_array_push_back(ready_queue, &newPCB3);
	
	ScheduleResult_t result = {.average_waiting_time = 0, .average_turnaround_time = 0, .total_run_time = 0};
	
	EXPECT_EQ(true,priority(ready_queue, &result));
	
	EXPECT_EQ(result.total_run_time, 12UL);
	EXPECT_NEAR(result.average_waiting_time, 2.67, 0.01);
	EXPECT_NEAR(result.average_turnaround_time, 6.67, 0.01);
	
	dyn_array_destroy(ready_queue);
}

TEST (priority, DifferentPrioritiesDiffArrivalShort) // should be actually doing the priority algorithm because each process is given a different priority
{
	ProcessControlBlock_t newPCB1 = {.remaining_burst_time = 4, .priority = 2, .arrival = 0, .started = false};
	ProcessControlBlock_t newPCB2 = {.remaining_burst_time = 2, .priority = 1, .arrival = 1, .started = false};
	ProcessControlBlock_t newPCB3 = {.remaining_burst_time = 6, .priority = 3, .arrival = 2, .started = false};
	
	dyn_array_t* ready_queue = dyn_array_create(3, sizeof(ProcessControlBlock_t), NULL);
	
	dyn_array_push_back(ready_queue, &newPCB1);
	dyn_array_push_back(ready_queue, &newPCB2);
	dyn_array_push_back(ready_queue, &newPCB3);
	
	ScheduleResult_t result = {.average_waiting_time = 0, .average_turnaround_time = 0, .total_run_time = 0};
	
	EXPECT_EQ(true,priority(ready_queue, &result));
	
	EXPECT_EQ(result.total_run_time, 12UL);
	EXPECT_NEAR(result.average_waiting_time, 2.33, 0.01);
	EXPECT_NEAR(result.average_turnaround_time, 6.33, 0.01);
	
	dyn_array_destroy(ready_queue);
}

TEST (priority, DifferentPrioritiesDiffArrivalLong) // tests with more than 3 processes
{
	ProcessControlBlock_t newPCB1 = {.remaining_burst_time = 4, .priority = 2, .arrival = 0, .started = false};
	ProcessControlBlock_t newPCB2 = {.remaining_burst_time = 2, .priority = 1, .arrival = 1, .started = false};
	ProcessControlBlock_t newPCB3 = {.remaining_burst_time = 6, .priority = 3, .arrival = 3, .started = false};
	ProcessControlBlock_t newPCB4 = {.remaining_burst_time = 5, .priority = 5, .arrival = 5, .started = false};
	ProcessControlBlock_t newPCB5 = {.remaining_burst_time = 3, .priority = 4, .arrival = 8, .started = false};
	ProcessControlBlock_t newPCB6 = {.remaining_burst_time = 7, .priority = 7, .arrival = 10, .started = false};
	ProcessControlBlock_t newPCB7 = {.remaining_burst_time = 9, .priority = 8, .arrival = 2, .started = false};
	ProcessControlBlock_t newPCB8 = {.remaining_burst_time = 20, .priority = 10, .arrival = 4, .started = false};
	ProcessControlBlock_t newPCB9 = {.remaining_burst_time = 5, .priority = 3, .arrival = 6, .started = false};
	ProcessControlBlock_t newPCB10 = {.remaining_burst_time = 2, .priority = 1, .arrival = 20, .started = false};
	
	dyn_array_t* ready_queue = dyn_array_create(10, sizeof(ProcessControlBlock_t), NULL);
	
	dyn_array_push_back(ready_queue, &newPCB1);
	dyn_array_push_back(ready_queue, &newPCB2);
	dyn_array_push_back(ready_queue, &newPCB3);
	dyn_array_push_back(ready_queue, &newPCB4);
	dyn_array_push_back(ready_queue, &newPCB5);
	dyn_array_push_back(ready_queue, &newPCB6);
	dyn_array_push_back(ready_queue, &newPCB7);
	dyn_array_push_back(ready_queue, &newPCB8);
	dyn_array_push_back(ready_queue, &newPCB9);
	dyn_array_push_back(ready_queue, &newPCB10);

	ScheduleResult_t result = {.average_waiting_time = 0, .average_turnaround_time = 0, .total_run_time = 0};
	
	EXPECT_EQ(true,priority(ready_queue, &result));
	
	EXPECT_EQ(result.total_run_time, 63UL);
	EXPECT_NEAR(result.average_waiting_time, 12.60, 0.01);
	EXPECT_NEAR(result.average_turnaround_time, 18.90, 0.01);
	
	dyn_array_destroy(ready_queue);
}

/*
*  Shortest remaining time UNIT TEST CASES
**/
// if ready_queue == NULL
TEST (shortest_remaining_time_first, ReadyQueueNULL) 
{
	ScheduleResult_t result = {.average_waiting_time = 0, .average_turnaround_time = 0, .total_run_time = 0};
	EXPECT_EQ(false,shortest_remaining_time_first(NULL, &result));
}


// if result == NULL
TEST (shortest_remaining_time_first, ResultNULL) 
{
	ProcessControlBlock_t newPCB1 = {.remaining_burst_time = 5, .priority = 1, .arrival = 0, .started = false};
	ProcessControlBlock_t newPCB2 = {.remaining_burst_time = 3, .priority = 1, .arrival = 0, .started = false};
	ProcessControlBlock_t newPCB3 = {.remaining_burst_time = 8, .priority = 1, .arrival = 0, .started = false};
	
	dyn_array_t* ready_queue = dyn_array_create(3, sizeof(ProcessControlBlock_t), NULL);
	
	dyn_array_push_back(ready_queue, &newPCB1);
	dyn_array_push_back(ready_queue, &newPCB2);
	dyn_array_push_back(ready_queue, &newPCB3);
	
	EXPECT_EQ(false,shortest_remaining_time_first(ready_queue, NULL));
	
	dyn_array_destroy(ready_queue);
}

// if dyn_array_size(ready_queue) == 0
TEST (shortest_remaining_time_first, ZeroSizeArray) 
{
	dyn_array_t* ready_queue = dyn_array_create(3, sizeof(ProcessControlBlock_t), NULL);

	// Not adding any ProcessControlBlock_t to ready_queue, no processes to schedule

	ScheduleResult_t result = {.average_waiting_time = 0, .average_turnaround_time = 0, .total_run_time = 0};
	
	EXPECT_EQ(false,shortest_remaining_time_first(ready_queue, &result));
	
	dyn_array_destroy(ready_queue);
}

TEST(shortest_remaining_time_first, ShortestArrivesFirst)
{
	ProcessControlBlock_t newPCB1 = {.remaining_burst_time = 3, .priority = 2, .arrival = 0, .started = false};
	ProcessControlBlock_t newPCB2 = {.remaining_burst_time = 4, .priority = 1, .arrival = 1, .started = false};
	ProcessControlBlock_t newPCB3 = {.remaining_burst_time = 6, .priority = 3, .arrival = 3, .started = false};
	
	dyn_array_t* ready_queue = dyn_array_create(3, sizeof(ProcessControlBlock_t), NULL);
	
	dyn_array_push_back(ready_queue, &newPCB1);
	dyn_array_push_back(ready_queue, &newPCB2);
	dyn_array_push_back(ready_queue, &newPCB3);
	
	ScheduleResult_t result = {.average_waiting_time = 0, .average_turnaround_time = 0, .total_run_time = 0};
	
	EXPECT_EQ(true, shortest_remaining_time_first(ready_queue, &result));
	
	EXPECT_EQ(result.total_run_time, 13UL);
	EXPECT_NEAR(result.average_waiting_time, 2.00, 0.01);
	EXPECT_NEAR(result.average_turnaround_time, 6.33, 0.01);
	
	dyn_array_destroy(ready_queue);
}

TEST(shortest_remaining_time_first, ShortestArrivesMiddle)
{
	ProcessControlBlock_t newPCB2 = {.remaining_burst_time = 4, .priority = 2, .arrival = 0, .started = false};
	ProcessControlBlock_t newPCB1 = {.remaining_burst_time = 3, .priority = 1, .arrival = 1, .started = false};
	ProcessControlBlock_t newPCB3 = {.remaining_burst_time = 6, .priority = 3, .arrival = 3, .started = false};
	
	dyn_array_t* ready_queue = dyn_array_create(3, sizeof(ProcessControlBlock_t), NULL);
	
	dyn_array_push_back(ready_queue, &newPCB1);
	dyn_array_push_back(ready_queue, &newPCB2);
	dyn_array_push_back(ready_queue, &newPCB3);
	
	ScheduleResult_t result = {.average_waiting_time = 0, .average_turnaround_time = 0, .total_run_time = 0};
	
	EXPECT_EQ(true, shortest_remaining_time_first(ready_queue, &result));
	
	EXPECT_EQ(result.total_run_time, 13UL);
	EXPECT_NEAR(result.average_waiting_time, 2.33, 0.01);
	EXPECT_NEAR(result.average_turnaround_time, 6.66, 0.01);
	
	dyn_array_destroy(ready_queue);
}

TEST(shortest_remaining_time_first, ShortestArrivesLast)
{
	ProcessControlBlock_t newPCB1 = {.remaining_burst_time = 6, .priority = 2, .arrival = 0, .started = false};
	ProcessControlBlock_t newPCB2 = {.remaining_burst_time = 4, .priority = 1, .arrival = 1, .started = false};
	ProcessControlBlock_t newPCB3 = {.remaining_burst_time = 3, .priority = 3, .arrival = 3, .started = false};
	
	dyn_array_t* ready_queue = dyn_array_create(3, sizeof(ProcessControlBlock_t), NULL);
	
	dyn_array_push_back(ready_queue, &newPCB1);
	dyn_array_push_back(ready_queue, &newPCB2);
	dyn_array_push_back(ready_queue, &newPCB3);
	
	ScheduleResult_t result = {.average_waiting_time = 0, .average_turnaround_time = 0, .total_run_time = 0};
	
	EXPECT_EQ(true, shortest_remaining_time_first(ready_queue, &result));
	
	EXPECT_EQ(result.total_run_time, 13UL);
	EXPECT_NEAR(result.average_waiting_time, 3.00, 0.01);
	EXPECT_NEAR(result.average_turnaround_time, 7.33, 0.01);
	
	dyn_array_destroy(ready_queue);
}

TEST(shortest_remaining_time_first, SameArrival)
{
	ProcessControlBlock_t newPCB2 = {.remaining_burst_time = 4, .priority = 1, .arrival = 0, .started = false};
	ProcessControlBlock_t newPCB1 = {.remaining_burst_time = 2, .priority = 3, .arrival = 0, .started = false};
	ProcessControlBlock_t newPCB3 = {.remaining_burst_time = 6, .priority = 2, .arrival = 0, .started = false};
	
	dyn_array_t* ready_queue = dyn_array_create(3, sizeof(ProcessControlBlock_t), NULL);
	
	dyn_array_push_back(ready_queue, &newPCB1);
	dyn_array_push_back(ready_queue, &newPCB2);
	dyn_array_push_back(ready_queue, &newPCB3);
	
	ScheduleResult_t result = {.average_waiting_time = 0, .average_turnaround_time = 0, .total_run_time = 0};
	
	EXPECT_EQ(true, shortest_remaining_time_first(ready_queue, &result));
	
	EXPECT_EQ(result.total_run_time, 12UL);
	EXPECT_NEAR(result.average_waiting_time, 2.66, 0.01);
	EXPECT_NEAR(result.average_turnaround_time, 6.66, 0.01);
	
	dyn_array_destroy(ready_queue);
}

TEST(shortest_remaining_time_first, SameArrivalSamePriority)
{
	ProcessControlBlock_t newPCB2 = {.remaining_burst_time = 4, .priority = 1, .arrival = 0, .started = false};
	ProcessControlBlock_t newPCB1 = {.remaining_burst_time = 2, .priority = 1, .arrival = 0, .started = false};
	ProcessControlBlock_t newPCB3 = {.remaining_burst_time = 6, .priority = 1, .arrival = 0, .started = false};
	
	dyn_array_t* ready_queue = dyn_array_create(3, sizeof(ProcessControlBlock_t), NULL);
	
	dyn_array_push_back(ready_queue, &newPCB1);
	dyn_array_push_back(ready_queue, &newPCB2);
	dyn_array_push_back(ready_queue, &newPCB3);
	
	ScheduleResult_t result = {.average_waiting_time = 0, .average_turnaround_time = 0, .total_run_time = 0};
	
	EXPECT_EQ(true, shortest_remaining_time_first(ready_queue, &result));
	
	EXPECT_EQ(result.total_run_time, 12UL);
	EXPECT_NEAR(result.average_waiting_time, 2.66, 0.01);
	EXPECT_NEAR(result.average_turnaround_time, 6.66, 0.01);
	
	dyn_array_destroy(ready_queue);
}

TEST(shortest_remaining_time_first, BigData)
{
	ProcessControlBlock_t newPCB1 = {.remaining_burst_time = 4, .priority = 2, .arrival = 0, .started = false};
	ProcessControlBlock_t newPCB2 = {.remaining_burst_time = 2, .priority = 1, .arrival = 1, .started = false};
	ProcessControlBlock_t newPCB3 = {.remaining_burst_time = 6, .priority = 3, .arrival = 3, .started = false};
	ProcessControlBlock_t newPCB4 = {.remaining_burst_time = 5, .priority = 5, .arrival = 5, .started = false};
	ProcessControlBlock_t newPCB5 = {.remaining_burst_time = 3, .priority = 4, .arrival = 8, .started = false};
	ProcessControlBlock_t newPCB6 = {.remaining_burst_time = 7, .priority = 7, .arrival = 10, .started = false};
	ProcessControlBlock_t newPCB7 = {.remaining_burst_time = 9, .priority = 8, .arrival = 2, .started = false};
	ProcessControlBlock_t newPCB8 = {.remaining_burst_time = 20, .priority = 10, .arrival = 4, .started = false};
	ProcessControlBlock_t newPCB9 = {.remaining_burst_time = 5, .priority = 3, .arrival = 6, .started = false};
	ProcessControlBlock_t newPCB10 = {.remaining_burst_time = 2, .priority = 1, .arrival = 20, .started = false};
	
	dyn_array_t* ready_queue = dyn_array_create(10, sizeof(ProcessControlBlock_t), NULL);
	
	dyn_array_push_back(ready_queue, &newPCB1);
	dyn_array_push_back(ready_queue, &newPCB2);
	dyn_array_push_back(ready_queue, &newPCB3);
	dyn_array_push_back(ready_queue, &newPCB4);
	dyn_array_push_back(ready_queue, &newPCB5);
	dyn_array_push_back(ready_queue, &newPCB6);
	dyn_array_push_back(ready_queue, &newPCB7);
	dyn_array_push_back(ready_queue, &newPCB8);
	dyn_array_push_back(ready_queue, &newPCB9);
	dyn_array_push_back(ready_queue, &newPCB10);
	
	ScheduleResult_t result = {.average_waiting_time = 0, .average_turnaround_time = 0, .total_run_time = 0};
	
	EXPECT_EQ(true, shortest_remaining_time_first(ready_queue, &result));
	
	EXPECT_EQ(result.total_run_time, 50UL);
	EXPECT_NEAR(result.average_waiting_time, 2.66, 0.01);
	EXPECT_NEAR(result.average_turnaround_time, 6.66, 0.01);
	
	dyn_array_destroy(ready_queue);
} 


/*
*  LOAD PROCESS CONTROL BLOCKS UNIT TEST CASES
**/

/*
*  Tests related to invalid parameters
**/

// if the filename is NULL
TEST (load_process_control_blocks, fileNULL) 
{
	EXPECT_EQ(NULL,load_process_control_blocks(NULL));
}

// if the file doesn't exist
TEST (load_process_control_blocks, fileDNE) 
{
	EXPECT_EQ(NULL,load_process_control_blocks("fileDNE.bin"));
}

/*
*  Tests related to corrupt or malformed file types
**/

TEST (load_process_control_blocks, emptyFile) 
{
	FILE *fptr = fopen("test.bin", "wb");
	fclose(fptr);
	EXPECT_EQ(NULL,load_process_control_blocks("test.bin"));
	remove("test.bin");
}

// test file too short, should at least have 4 bytes for N even if N=0
TEST (load_process_control_blocks, fileTooShort) 
{
	FILE *fptr = fopen("test.bin", "wb");
	uint16_t testNum = 24;
	fwrite(&testNum, sizeof(uint16_t), 1, fptr); // only write 2 bytes to the file
	fclose(fptr);
	EXPECT_EQ(NULL,load_process_control_blocks("test.bin"));
	remove("test.bin");
}

//The PCB file is composed of LESS than 1+(3*N) 32-bit integers
TEST (load_process_control_blocks, tooFewIntegers) 
{
	FILE *fptr = fopen("test.bin", "wb");
	
	uint32_t N = 2; // 2 pcbs
	fwrite(&N, sizeof(uint32_t), 1, fptr);

	uint32_t P1remaining_burst_time = 2;
	fwrite(&P1remaining_burst_time, sizeof(uint32_t), 1, fptr);
	uint32_t P1priority = 5;
	fwrite(&P1priority, sizeof(uint32_t), 1, fptr);
	uint32_t P1arrival = 3;
	fwrite(&P1arrival, sizeof(uint32_t), 1, fptr);

	uint32_t P2remaining_burst_time = 3;
	fwrite(&P2remaining_burst_time, sizeof(uint32_t), 1, fptr);
	uint32_t P2priority = 6;
	fwrite(&P2priority, sizeof(uint32_t), 1, fptr);
	// leaving out arrival of the second process so that the PCB file is NOT composed of 1+(3*N) 32-bit integers

	fclose(fptr);

	EXPECT_EQ(NULL, load_process_control_blocks("test.bin"));
	remove("test.bin");
}

//The PCB file is composed of MORE than 1+(3*N) 32-bit integers
TEST (load_process_control_blocks, tooManyIntegers) 
{
	FILE *fptr = fopen("test.bin", "wb");
	
	uint32_t N = 2; // 2 pcbs
	fwrite(&N, sizeof(uint32_t), 1, fptr);

	uint32_t P1remaining_burst_time = 2;
	fwrite(&P1remaining_burst_time, sizeof(uint32_t), 1, fptr);
	uint32_t P1priority = 5;
	fwrite(&P1priority, sizeof(uint32_t), 1, fptr);
	uint32_t P1arrival = 3;
	fwrite(&P1arrival, sizeof(uint32_t), 1, fptr);

	uint32_t P2remaining_burst_time = 3;
	fwrite(&P2remaining_burst_time, sizeof(uint32_t), 1, fptr);
	uint32_t P2priority = 6;
	fwrite(&P2priority, sizeof(uint32_t), 1, fptr);
	uint32_t P2arrival = 1;
	fwrite(&P2arrival, sizeof(uint32_t), 1, fptr);

	// Adding burst time of a third process, should not be here
	uint32_t P3remaining_burst_time = 4;
	fwrite(&P3remaining_burst_time, sizeof(uint32_t), 1, fptr);

	fclose(fptr);
	
	EXPECT_EQ(NULL, load_process_control_blocks("test.bin"));
	remove("test.bin");
}


/*
*  Tests related to zero processes in a valid binary file, N=0
**/

TEST (load_process_control_blocks, zeroProcesses) 
{
	FILE *fptr = fopen("test.bin", "wb");
	
	uint32_t N = 0;
	fwrite(&N, sizeof(uint32_t), 1, fptr);

	fclose(fptr);

	dyn_array_t * array = load_process_control_blocks("test.bin");

	ASSERT_NE(array, nullptr); // array is not null
	EXPECT_EQ(dyn_array_size(array), static_cast<size_t>(0)); // array is empty

	dyn_array_destroy(array);
	remove("test.bin");
}

/*
*  Tests related to one process in a valid binary file, N=1
**/

TEST (load_process_control_blocks, oneProcess) 
{
	FILE *fptr = fopen("test.bin", "wb");
	
	uint32_t N = 1;
	fwrite(&N, sizeof(uint32_t), 1, fptr);

	uint32_t P1remaining_burst_time = 2;
	fwrite(&P1remaining_burst_time, sizeof(uint32_t), 1, fptr);
	uint32_t P1priority = 5;
	fwrite(&P1priority, sizeof(uint32_t), 1, fptr);
	uint32_t P1arrival = 3;
	fwrite(&P1arrival, sizeof(uint32_t), 1, fptr);

	fclose(fptr);

	dyn_array_t * array = load_process_control_blocks("test.bin");

	ASSERT_NE(array, nullptr); // array is not null
	EXPECT_EQ(dyn_array_size(array), static_cast<size_t>(1)); // array has 1 process

	ProcessControlBlock_t* pcb = (ProcessControlBlock_t *)dyn_array_at(array,0);

	EXPECT_EQ(P1remaining_burst_time, pcb->remaining_burst_time);
	EXPECT_EQ(P1priority, pcb->priority);
	EXPECT_EQ(P1arrival, pcb->arrival);
	EXPECT_FALSE(pcb->started);

	dyn_array_destroy(array);
	remove("test.bin");
}

/*
*  Tests related to multiple processes in a valid binary file, N = 10
**/

TEST (load_process_control_blocks, multipleProcesses) 
{
	FILE *fptr = fopen("test.bin", "wb");
	
	uint32_t N = 10;
	fwrite(&N, sizeof(uint32_t), 1, fptr);

	uint32_t burst_times[10] = {2, 3, 7, 9, 0, 15, 8, 3, 6, 9};
	uint32_t priorities[10] = {5, 6, 4, 5, 10, 1, 9, 4, 7, 8};
	uint32_t arrivals[10] = {3, 8, 2, 1, 30, 4, 10, 5, 8, 9};

	size_t i;

	for (i = 0; i < 10; i++) {
		fwrite(&burst_times[i], sizeof(uint32_t), 1, fptr);
		fwrite(&priorities[i], sizeof(uint32_t), 1, fptr);
		fwrite(&arrivals[i], sizeof(uint32_t), 1, fptr);
	}

	fclose(fptr);

	dyn_array_t * array = load_process_control_blocks("test.bin");

	ASSERT_NE(array, nullptr); // array is not null
	EXPECT_EQ(dyn_array_size(array), static_cast<size_t>(10)); // array has 10 processes

	for (i = 0; i < 10; i++) {
		ProcessControlBlock_t* pcb = (ProcessControlBlock_t *)dyn_array_at(array,i);
		ASSERT_NE(pcb, nullptr); // pcb is not null

		EXPECT_EQ(burst_times[i], pcb->remaining_burst_time);
		EXPECT_EQ(priorities[i], pcb->priority);
		EXPECT_EQ(arrivals[i], pcb->arrival);
		EXPECT_FALSE(pcb->started);
	}

	dyn_array_destroy(array);
	remove("test.bin");
}

// large amount of pcbs test?



unsigned int score;
unsigned int total;

class GradeEnvironment : public testing::Environment
{
	public:
		virtual void SetUp()
		{
			score = 0;
			total = 210;
		}

		virtual void TearDown()
		{
			::testing::Test::RecordProperty("points_given", score);
			::testing::Test::RecordProperty("points_total", total);
			std::cout << "SCORE: " << score << '/' << total << std::endl;
		}
};



int main(int argc, char **argv)
{
	::testing::InitGoogleTest(&argc, argv);
	::testing::AddGlobalTestEnvironment(new GradeEnvironment);
	return RUN_ALL_TESTS();
}
