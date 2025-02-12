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
