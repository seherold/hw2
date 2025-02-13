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
/*TEST (first_come_first_serve, BasicPCBs) {
	
	ProcessControlBlock_t newPCB1 = {.remaining_burst_time = 5, .priority = 0, .arrival = 0, .started = false};
	ProcessControlBlock_t newPCB2 = {.remaining_burst_time = 3, .priority = 0, .arrival = 1, .started = false};
	ProcessControlBlock_t newPCB3 = {.remaining_burst_time = 8, .priority = 0, .arrival = 2, .started = false};
	

	

	ScheduleResult_t result;

	EXPECT_EQ(true,first_come_first_serve(ready_queue,result));
}*/

TEST (first_come_first_serve, SameArrivalTime) 
{
	ProcessControlBlock_t newPCB1 = {.remaining_burst_time = 5, .priority = 0, .arrival = 0, .started = false};
	ProcessControlBlock_t newPCB2 = {.remaining_burst_time = 3, .priority = 0, .arrival = 0, .started = false};
	ProcessControlBlock_t newPCB3 = {.remaining_burst_time = 8, .priority = 0, .arrival = 0, .started = false};
	
	dyn_array_t* ready_queue = dyn_array_create(3, sizeof(ProcessControlBlock_t), NULL);
	
	dyn_array_push_back(ready_queue, &newPCB1);
	dyn_array_push_back(ready_queue, &newPCB2);
	dyn_array_push_back(ready_queue, &newPCB3);
	
	ScheduleResult_t result = {.average_waiting_time = 0, .average_turnaround_time = 0, .total_run_time = 0};
	
	EXPECT_EQ(true,first_come_first_serve(ready_queue, &result));
	
	EXPECT_EQ(result.total_run_time, 16UL);
	EXPECT_NEAR(result.average_waiting_time, 4.33, 0.01);
	EXPECT_NEAR(result.average_turnaround_time, 9.67, 0.01);
	
	dyn_array_destroy(ready_queue);
}

// if ready_queue == NULL
TEST (first_come_first_serve, ReadyQueueNULL) 
{
	ScheduleResult_t result = {.average_waiting_time = 0, .average_turnaround_time = 0, .total_run_time = 0};
	EXPECT_EQ(false,first_come_first_serve(NULL, &result));
}


// if result == NULL
TEST (first_come_first_serve, ResultNULL) 
{
	ProcessControlBlock_t newPCB1 = {.remaining_burst_time = 5, .priority = 0, .arrival = 0, .started = false};
	ProcessControlBlock_t newPCB2 = {.remaining_burst_time = 3, .priority = 0, .arrival = 0, .started = false};
	ProcessControlBlock_t newPCB3 = {.remaining_burst_time = 8, .priority = 0, .arrival = 0, .started = false};
	
	dyn_array_t* ready_queue = dyn_array_create(3, sizeof(ProcessControlBlock_t), NULL);
	
	dyn_array_push_back(ready_queue, &newPCB1);
	dyn_array_push_back(ready_queue, &newPCB2);
	dyn_array_push_back(ready_queue, &newPCB3);
	
	EXPECT_EQ(false,first_come_first_serve(ready_queue, NULL));
	
	dyn_array_destroy(ready_queue);
}

// if dyn_array_size(ready_queue) == 0
TEST (first_come_first_serve, ZeroSizeArray) 
{
	dyn_array_t* ready_queue = dyn_array_create(3, sizeof(ProcessControlBlock_t), NULL);

	// Not adding any ProcessControlBlock_t to ready_queue, no processes to schedule

	ScheduleResult_t result = {.average_waiting_time = 0, .average_turnaround_time = 0, .total_run_time = 0};
	
	EXPECT_EQ(false,first_come_first_serve(ready_queue, &result));
	
	dyn_array_destroy(ready_queue);
}

/*
*  Shortest Job First UNIT TEST CASES
**/
TEST (shortest_job_first, ReadyQueueNULL) 
{
	ScheduleResult_t result = {.average_waiting_time = 0, .average_turnaround_time = 0, .total_run_time = 0};
	EXPECT_EQ(false,shortest_job_first(NULL, &result));
}


// if result == NULL
TEST (shortest_job_first, ResultNULL) 
{
	ProcessControlBlock_t newPCB1 = {.remaining_burst_time = 5, .priority = 0, .arrival = 0, .started = false};
	ProcessControlBlock_t newPCB2 = {.remaining_burst_time = 3, .priority = 0, .arrival = 0, .started = false};
	ProcessControlBlock_t newPCB3 = {.remaining_burst_time = 8, .priority = 0, .arrival = 0, .started = false};
	
	dyn_array_t* ready_queue = dyn_array_create(3, sizeof(ProcessControlBlock_t), NULL);
	
	dyn_array_push_back(ready_queue, &newPCB1);
	dyn_array_push_back(ready_queue, &newPCB2);
	dyn_array_push_back(ready_queue, &newPCB3);
	
	EXPECT_EQ(false,shortest_job_first(ready_queue, NULL));
	
	dyn_array_destroy(ready_queue);
}

// if dyn_array_size(ready_queue) == 0
TEST (shortest_job_first, ZeroSizeArray) 
{
	dyn_array_t* ready_queue = dyn_array_create(3, sizeof(ProcessControlBlock_t), NULL);

	// Not adding any ProcessControlBlock_t to ready_queue, no processes to schedule

	ScheduleResult_t result = {.average_waiting_time = 0, .average_turnaround_time = 0, .total_run_time = 0};
	
	EXPECT_EQ(false,shortest_job_first(ready_queue, &result));
	
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
    
    EXPECT_EQ(false, round_robin(ready_queue, &result, 0)); //If Quantym is zero
    
    EXPECT_EQ(false, round_robin(ready_queue, &result, QUANTUM)); //If Queue is empty
    
    
    dyn_array_destroy(ready_queue);
}



/*
*  Priority UNIT TEST CASES
**/
// if ready_queue == NULL
TEST (priority, ReadyQueueNULL) 
{
	ScheduleResult_t result = {.average_waiting_time = 0, .average_turnaround_time = 0, .total_run_time = 0};
	EXPECT_EQ(false,priority(NULL, &result));
}


// if result == NULL
TEST (priority, ResultNULL) 
{
	ProcessControlBlock_t newPCB1 = {.remaining_burst_time = 5, .priority = 0, .arrival = 0, .started = false};
	ProcessControlBlock_t newPCB2 = {.remaining_burst_time = 3, .priority = 0, .arrival = 0, .started = false};
	ProcessControlBlock_t newPCB3 = {.remaining_burst_time = 8, .priority = 0, .arrival = 0, .started = false};
	
	dyn_array_t* ready_queue = dyn_array_create(3, sizeof(ProcessControlBlock_t), NULL);
	
	dyn_array_push_back(ready_queue, &newPCB1);
	dyn_array_push_back(ready_queue, &newPCB2);
	dyn_array_push_back(ready_queue, &newPCB3);
	
	EXPECT_EQ(false,priority(ready_queue, NULL));
	
	dyn_array_destroy(ready_queue);
}

// if dyn_array_size(ready_queue) == 0
TEST (priority, ZeroSizeArray) 
{
	dyn_array_t* ready_queue = dyn_array_create(3, sizeof(ProcessControlBlock_t), NULL);

	// Not adding any ProcessControlBlock_t to ready_queue, no processes to schedule

	ScheduleResult_t result = {.average_waiting_time = 0, .average_turnaround_time = 0, .total_run_time = 0};
	
	EXPECT_EQ(false,priority(ready_queue, &result));
	
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
	ProcessControlBlock_t newPCB1 = {.remaining_burst_time = 5, .priority = 0, .arrival = 0, .started = false};
	ProcessControlBlock_t newPCB2 = {.remaining_burst_time = 3, .priority = 0, .arrival = 0, .started = false};
	ProcessControlBlock_t newPCB3 = {.remaining_burst_time = 8, .priority = 0, .arrival = 0, .started = false};
	
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
