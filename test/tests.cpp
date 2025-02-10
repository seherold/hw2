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
*  DYN ARRAY CREATE UNIT TEST CASES
**/

/*
*  DYN ARRAY IMPORT UNIT TEST CASES
**/

/*
*  DYN ARRAY EXPORT UNIT TEST CASES
**/

/*
*  DYN ARRAY DESTROY UNIT TEST CASES
**/

/*
*  DYN ARRAY FRONT UNIT TEST CASES
**/


/*
*  DYN ARRAY PUSH FRONT UNIT TEST CASES
**/
TEST (dyn_array_push_front, NullArray) {
	int fakeObject = 0;
	EXPECT_EQ(false,dyn_array_push_front(NULL, (const void*)&fakeObject));
}

// Other test case ideas
// if size == 0
// if size > capacity
// if fakeArray.array == NULL
// if no elements have been initialized in fakeArray

TEST (dyn_array_push_front, NullObject) {
	dyn_array_t fakeArray = {
        .capacity = 10,  
        .size = 10,  
        .data_size = sizeof(int),  
        .array = malloc(10 * sizeof(int)),  
        .destructor = NULL
	};
	memset(fakeArray.array, 0, 10 * sizeof(int));
	EXPECT_EQ(false,dyn_array_push_front(&fakeArray, NULL));
	free(fakeArray.array);
}


/*
*  DYN ARRAY POP FRONT UNIT TEST CASES
**/
TEST (dyn_array_pop_front, NullArray) {
	int fakeObject = 0;
	EXPECT_EQ(false,dyn_array_pop_front(NULL, (const void*)&fakeObject));
}

/*
*  DYN ARRAY EXTRACT FRONT UNIT TEST CASES
**/
TEST (dyn_array_extract_front, NullArray) {
	int fakeObject = 0;
	EXPECT_EQ(false,dyn_array_extract_front(NULL, (const void*)&fakeObject));
}

TEST (dyn_array_extract_front, NullObject) {
	dyn_array_t fakeArray = {
        .capacity = 10,  
        .size = 10,  
        .data_size = sizeof(int),  
        .array = malloc(10 * sizeof(int)),  
        .destructor = NULL
	};
	memset(fakeArray.array, 0, 10 * sizeof(int));
	EXPECT_EQ(false,dyn_array_extract_front(&fakeArray, NULL));
	free(fakeArray.array);
}

/*
*  DYN ARRAY BACK UNIT TEST CASES
**/

/*
*  DYN ARRAY PUSH BACK UNIT TEST CASES
**/
TEST (dyn_array_push_back, NullArray) {
	int fakeObject = 0;
	EXPECT_EQ(false,dyn_array_push_back(NULL, (const void*)&fakeObject));
}

TEST (dyn_array_push_back, NullObject) {
	dyn_array_t fakeArray = {
        .capacity = 10,  
        .size = 10,  
        .data_size = sizeof(int),  
        .array = malloc(10 * sizeof(int)),  
        .destructor = NULL
	};
	memset(fakeArray.array, 0, 10 * sizeof(int));
	EXPECT_EQ(false,dyn_array_push_back(&fakeArray, NULL));
	free(fakeArray.array);
}

/*
*  DYN ARRAY POP BACK UNIT TEST CASES
**/
TEST (dyn_array_pop_back, NullArray) {
	int fakeObject = 0;
	EXPECT_EQ(false,dyn_array_pop_back(NULL, (const void*)&fakeObject));
}

/*
*  DYN ARRAY EXTRACT BACK UNIT TEST CASES
**/
TEST (dyn_array_extract_back, NullArray) {
	int fakeObject = 0;
	EXPECT_EQ(false,dyn_array_extract_back(NULL, (const void*)&fakeObject));
}

TEST (dyn_array_extract_back, NullObject) {
	dyn_array_t fakeArray = {
        .capacity = 10,  
        .size = 10,  
        .data_size = sizeof(int),  
        .array = malloc(10 * sizeof(int)),  
        .destructor = NULL
	};
	memset(fakeArray.array, 0, 10 * sizeof(int));
	EXPECT_EQ(false,dyn_array_extract_back(&fakeArray, NULL));
	free(fakeArray.array);
}


/*
*  DYN ARRAY AT UNIT TEST CASES
**/
TEST (dyn_array_at, NullArray) {
	size_t fakeIndex = 2;
	EXPECT_EQ(NULL,dyn_array_at(NULL, fakeIndex));
}

TEST (dyn_array_at, NegIndex) {
	dyn_array_t fakeArray = {
        .capacity = 10,  
        .size = 10,  
        .data_size = sizeof(int),  
        .array = malloc(10 * sizeof(int)),  
        .destructor = NULL
	};
	memset(fakeArray.array, 0, 10 * sizeof(int));
	EXPECT_EQ(NULL,dyn_array_at(&fakeArray, -1));
	free(fakeArray.array);
}


/*
*  DYN ARRAY INSERT UNIT TEST CASES
**/
TEST (dyn_array_insert, NullArray) {
	int fakeObject = 0;
	size_t fakeIndex = 2;
	EXPECT_EQ(false,dyn_array_insert(NULL, fakeIndex, (const void*)&fakeObject));
}

TEST (dyn_array_insert, NegIndex) {
	dyn_array_t fakeArray = {
        .capacity = 10,  
        .size = 10,  
        .data_size = sizeof(int),  
        .array = malloc(10 * sizeof(int)),  
        .destructor = NULL
	};
	memset(fakeArray.array, 0, 10 * sizeof(int));
	int fakeObject = 0;
	EXPECT_EQ(false,dyn_array_insert(&fakeArray, -1, (const void*)&fakeObject));
	free(fakeArray.array);
}

TEST (dyn_array_insert, NullObject) {
	dyn_array_t fakeArray = {
        .capacity = 10,  
        .size = 10,  
        .data_size = sizeof(int),  
        .array = malloc(10 * sizeof(int)),  
        .destructor = NULL
	};
	memset(fakeArray.array, 0, 10 * sizeof(int));
	size_t fakeIndex = 2;
	EXPECT_EQ(false,dyn_array_insert(&fakeArray, fakeIndex, NULL));
	free(fakeArray.array);
}

/*
*  DYN ARRAY ERASE UNIT TEST CASES
**/
TEST (dyn_array_erase, NullArray) {
	size_t fakeIndex = 2;
	EXPECT_EQ(NULL,dyn_array_erase(NULL, fakeIndex));
}

TEST (dyn_array_erase, NegIndex) {
	dyn_array_t fakeArray = {
        .capacity = 10,  
        .size = 10,  
        .data_size = sizeof(int),  
        .array = malloc(10 * sizeof(int)),  
        .destructor = NULL
	};
	memset(fakeArray.array, 0, 10 * sizeof(int));
	EXPECT_EQ(NULL,dyn_array_erase(&fakeArray, -1));
	free(fakeArray.array);
}


/*
*  DYN ARRAY EXTRACT UNIT TEST CASES
**/
TEST (dyn_array_extract, NullArray) {
	int fakeObject = 0;
	size_t fakeIndex = 2;
	EXPECT_EQ(false,dyn_array_extract(NULL, fakeIndex, (const void*)&fakeObject));
}

TEST (dyn_array_extract, NegIndex) {
	dyn_array_t fakeArray = {
        .capacity = 10,  
        .size = 10,  
        .data_size = sizeof(int),  
        .array = malloc(10 * sizeof(int)),  
        .destructor = NULL
	};
	memset(fakeArray.array, 0, 10 * sizeof(int));
	int fakeObject = 0;
	EXPECT_EQ(false,dyn_array_extract(&fakeArray, -1, (const void*)&fakeObject));
	free(fakeArray.array);
}

TEST (dyn_array_extract, NullObject) {
	dyn_array_t fakeArray = {
        .capacity = 10,  
        .size = 10,  
        .data_size = sizeof(int),  
        .array = malloc(10 * sizeof(int)),  
        .destructor = NULL
	};
	memset(fakeArray.array, 0, 10 * sizeof(int));
	size_t fakeIndex = 2;
	EXPECT_EQ(false,dyn_array_extract(&fakeArray, fakeIndex, NULL));
	free(fakeArray.array);
}

/*
*  DYN ARRAY CLEAR UNIT TEST CASES
**/


/*
*  DYN ARRAY EMPTY UNIT TEST CASES
**/
TEST (dyn_array_empty, NullArray) {
	EXPECT_EQ(true,dyn_array_empty(NULL));
}

/*
*  DYN ARRAY SIZE UNIT TEST CASES
**/
TEST (dyn_array_size, NullArray) {
	EXPECT_EQ(0,dyn_array_size(NULL));
}


/*
*  DYN ARRAY CAPACITY UNIT TEST CASES
**/
TEST (dyn_array_capacity, NullArray) {
	EXPECT_EQ(0,dyn_array_capacity(NULL));
}


/*
*  DYN ARRAY CAPACITY UNIT TEST CASES
**/
TEST (dyn_array_data_size, NullArray) {
	EXPECT_EQ(0,dyn_array_data_size(NULL));
}

/*
*  DYN ARRAY SORT UNIT TEST CASES
**/











/*
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
*/


int main(int argc, char **argv)
{
	// ::testing::InitGoogleTest(&argc, argv);
	// ::testing::AddGlobalTestEnvironment(new GradeEnvironment);
	return RUN_ALL_TESTS();
}
