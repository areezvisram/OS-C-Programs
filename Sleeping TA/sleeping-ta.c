#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <semaphore.h>

#define true 1

// Determines number of students
#define NUMStudents 7
// Determines how many programming cycles the students take
#define cycleLimit 2
// Determines the amount of chairs available
#define NUMChairs 3
// Constants to determine sleep times in seconds
#define helpTime 4
#define MAXProgramTime 15
#define MINProgramTime 1

/* Code to declare mutex and Semaphores*/
pthread_mutex_t mutex;
sem_t TA_help_sem;
sem_t TA_alert_sem;
sem_t student_wait_sem;

/*Declare the thread functions*/
void *student(void *param);
void *TA(void *param);

// Declare global values
int open_chairs = NUMChairs;

int main(int argc, char *argv[])
{
	printf("\n****************TA Simulation****************\n");
	/*Declare threads*/

	pthread_t student_threads[NUMStudents];
	pthread_t TA_thread;

	/*Declare and initialize thread attribute*/
	pthread_attr_t attr;
	pthread_attr_init(&attr);

	/*Initialize mutex & semaphores*/
	if (pthread_mutex_init(&mutex, NULL) != 0)
	{
		printf("Error in initializing mutex!\n");
	}

	if (sem_init(&TA_help_sem, 0, 0) != 0)
	{
		printf("Error in initializing TA help Semaphore!\n");
	}

	if (sem_init(&TA_alert_sem, 0, 0) != 0)
	{
		printf("Error in initializing TA alert Semaphore!\n");
	}

	if (sem_init(&student_wait_sem, 0, 0) != 0)
	{
		printf("Error in initializing student wait Semaphore!\n");
	}

	/*Create TA Thread*/
	pthread_create(&TA_thread, &attr, TA, NULL);

	for (int i = 0; i < NUMStudents; i++)
	{
		int *studentID = malloc(sizeof(*studentID));

		*studentID = i + 1;
		pthread_create(&student_threads[i], &attr, student, *studentID);
	}
	/*Join threads*/
	pthread_join(TA_thread, NULL);
	for (int i = 0; i < NUMStudents; i++)
	{
		pthread_join(student_threads[i], NULL);
	}

	printf("\nTA has Finished teaching!\n");

	return 0;
}

void *student(void *param)
{

	// Variable to store cycles
	int helpCount = 0;
	// Storing ID
	int studentID = param;

	while (true)
	{

		// Terminates when cycle limit reached
		while (helpCount < cycleLimit)
		{

			// Generate Time Number
			int program_time = (rand() % MAXProgramTime) + MINProgramTime;

			// Student Programming
			sleep(program_time); // In seconds

			// Student Checks For Chair
			pthread_mutex_lock(&mutex);
			printf("Student %d is checking the waiting room\n", studentID);
			if (open_chairs > 0)
			{

				// Add to waiting room
				open_chairs--;
				printf("Student %d has entered Waiting Room\n", studentID);
				pthread_mutex_unlock(&mutex);

				// Wait for TA Spot To be open
				sem_wait(&student_wait_sem);

				// When open leave waiting room and open new slot
				pthread_mutex_lock(&mutex);
				printf("Student %d has left Waiting Room\n", studentID);
				open_chairs++;

				// Wake up TA
				sem_post(&TA_alert_sem);
				printf("Student %d has entered TA's office\n", studentID);
				pthread_mutex_unlock(&mutex);

				// Get Help from TA
				sem_wait(&TA_help_sem);

				// Leave room and continue programming
				helpCount++;
			}
			else
			{
				// When room is full
				pthread_mutex_unlock(&mutex);
				printf("No Space! Student %d will come back later\n", studentID);
			}
		}
		pthread_exit(0);
	}
}

void *TA(void *param)
{

	// Store cycles
	int helpCount = 0;
	// Store sleeping toggle
	int asleep = 0;

	while (true)
	{

		// When all cycles have been passed terminate TA
		while (helpCount < cycleLimit * NUMStudents)
		{

			// Check if Teacher can sleep
			pthread_mutex_lock(&mutex);
			if (open_chairs == NUMChairs)
			{
				asleep = 1;
				printf("TA has fallen asleep!\n");
			}
			// Allow a student through
			sem_post(&student_wait_sem);

			pthread_mutex_unlock(&mutex);

			// Wait for TA to be alerted
			sem_wait(&TA_alert_sem);

			// Teacher Being Awoken
			pthread_mutex_lock(&mutex);
			if (asleep)
			{
				asleep = 0;
				printf("TA has been awoken!\n");
			}

			// Help Student
			printf("TA is helping the student!--------------\n");
			pthread_mutex_unlock(&mutex);
			sleep(helpTime);
			printf("TA has Finished Helping the student!------------\n");
			pthread_mutex_lock(&mutex);
			sem_post(&TA_help_sem);
			pthread_mutex_unlock(&mutex);
			helpCount++;
		}
		pthread_exit(0);
	}
}