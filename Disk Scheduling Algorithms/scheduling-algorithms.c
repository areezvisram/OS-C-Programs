/* Sorting algorithm used in code:
Q-sort used from https://www.tutorialspoint.com/c_standard_library/c_function_qsort.htm
*/

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/mman.h>

// Define the needed macros
#define NUM_REQUEST 20
#define REQUEST_SIZE 4
#define MEMORY_SIZE NUM_REQUEST *REQUEST_SIZE
#define DISK_SIZE 300

// Global variables
char *mmapfptr;
int requestArray[NUM_REQUEST];
int sortedRequests[NUM_REQUEST];

// Helper function to find index of value in array of given size
int find_index(const int a[], int size, int value)
{
    int index = 0;

    while (index < size && a[index] != value)
        ++index;

    return (index == size ? -1 : index);
}

// Helper function to find the value in array closest to a given value
// Used in SSTF
int closest_value(int *arr, int n, int target)
{
    int i, closest, distance;
    closest = arr[0];
    distance = abs(target - closest);
    for (i = 1; i < n; i++)
    {
        int new_distance = abs(target - arr[i]);
        if (new_distance < distance)
        {
            closest = arr[i];
            distance = new_distance;
        }
    }
    return closest;
}

// FCFS Algorithm
void FCFS(int initial, int *requests, int arraySize)
{
    printf("FCFS DISK SCHEDULING ALGORITHM\n");
    // Initial head movements is distance from head to first request
    int total_head_movements = abs(initial - requests[0]);

    // First request
    printf("%d, ", requests[0]);

    // Serve the rest of the requests in the order they appear (first come first serve)
    for (int i = 1; i < arraySize; i++)
    {
        printf("%d, ", requests[i]);
        int difference = abs(requests[i] - requests[i - 1]);
        total_head_movements = total_head_movements + difference;
    }

    printf("\n");
    printf("FCFS - Total Head Movements %d \n", total_head_movements);
    printf("\n");
}

// SSTF Algorithm
void SSTF(int initial, int *requests, int arraySize)
{
    // Initialize some values
    int head_movement = 0;
    int completed_requests = 0;
    int position = initial;
    int local_request[NUM_REQUEST];
    for (int i = 0; i < arraySize; i++)
    {
        local_request[i] = requests[i];
    }

    printf("SSTF DISK SCHEDULING ALGORITHM\n");

    // Loop until all requests met
    while (completed_requests < arraySize)
    {
        // Check for match
        for (int i = 0; i < arraySize; i++)
        {
            if (position == local_request[i])
            {
                printf("%d, ", position);
                local_request[i] = 1000;
                completed_requests++;
            }
        }

        // Find the request position that is closest to the current head position,
        // this is the next request served
        if (completed_requests < arraySize)
        {
            int prev_position = position;
            position = closest_value(local_request, arraySize, position);
            head_movement += abs(position - prev_position);
        }
    }

    printf("\n");
    printf("SSTF - Total Head Movements = %d\n", head_movement);
    printf("\n");
}

// SCAN Algorithm
void SCAN(int initial, int direction, int *sorted_requests, int arraySize)
{
    // Initialize some values
    int head_movement = 0;
    int completed_requests = 0;
    int position = initial;

    // Find the index of the starting position in the requests
    int starting_index = find_index(sorted_requests, arraySize, initial);
    int local_request[NUM_REQUEST];

    for (int i = 0; i < arraySize; i++)
    {
        local_request[i] = sorted_requests[i];
    }

    printf("SCAN DISK SCHEDULING ALGORITHM\n");

    // Loop until all requests met
    while (completed_requests < arraySize)
    {
        // Check for match
        for (int i = 0; i < arraySize; i++)
        {
            if (position == local_request[i])
            {
                printf("%d, ", position);
                local_request[i] = -1;
                completed_requests++;
            }
        }

        // Check if reached end of scan, if not increment by direction
        // If going right and reach the end, start going left serving requests along the way
        if (direction == 1 && position == DISK_SIZE - 1)
        {
            position = sorted_requests[starting_index - 1];
            direction = -1;
            head_movement += (DISK_SIZE - 1) - sorted_requests[starting_index - 1];
        }
        // If going left and reach the start, start going right serving requests along the way
        else if (direction == -1 && position == 0)
        {
            position = sorted_requests[starting_index + 1];
            direction = 1;
            head_movement += sorted_requests[starting_index + 1];
        }
        // Increment position if we are not at either end
        else
        {
            if (completed_requests < arraySize)
            {
                position += direction;
                head_movement++;
            }
        }
    }
    printf("\n");
    printf("SCAN - Total Head Movements = %d\n", head_movement);
    printf("\n");
}

// C-SCAN Algorithm
void C_SCAN(int inital, int direction, int *sorted_requests, int arraySize)
{

    // Initialize some values
    int head_movement = 0;
    int completed_requests = 0;
    int position = inital;
    int local_request[NUM_REQUEST];

    for (int i = 0; i < arraySize; i++)
    {
        local_request[i] = sorted_requests[i];
    }

    printf("C-SCAN DISK SCHEDULING ALGORITHM\n");
    // Loop until all requests met
    while (completed_requests < arraySize)
    {
        // Check for match
        for (int i = 0; i < arraySize; i++)
        {
            if (position == local_request[i])
            {
                printf("%d, ", position);
                local_request[i] = -1;
                completed_requests++;
            }
        }

        // Check if reached end of scan, if not increment by direction
        if (direction == 1 && position == DISK_SIZE - 1)
        {
            position = 0;
            head_movement += DISK_SIZE - 1;
        }
        else if (direction == -1 && position == 0)
        {
            position = DISK_SIZE - 1;
            head_movement += DISK_SIZE - 1;
        }
        else
        {
            if (completed_requests < arraySize)
            {
                position += direction;
                head_movement++;
            }
        }
    }
    printf("\n");
    printf("C-SCAN - Total Head Movements = %d\n", head_movement);
    printf("\n");
}

// LOOK Algorithm
void LOOK(int inital, int init_direction, int *sorted_requests, int arraySize)
{

    // Initialize some values
    int head_movement = 0;
    int completed_requests = 0;
    int position = inital;
    int direction = init_direction;
    int local_request[NUM_REQUEST];

    for (int i = 0; i < arraySize; i++)
    {
        local_request[i] = sorted_requests[i];
    }

    int upper = local_request[19];
    int lower = local_request[0];

    printf("LOOK DISK SCHEDULING ALGORITHM \n");

    // Loop until all requests met
    while (completed_requests < arraySize)
    {
        // Check for match
        for (int i = 0; i < arraySize; i++)
        {
            if (position == local_request[i])
            {
                printf("%d, ", local_request[i]);
                local_request[i] = -1;
                completed_requests++;
            }
        }

        // Check if reached end of look, change direction accordingly
        if (direction == 1 && position == upper)
        {
            direction = -1;
        }
        else if (direction == -1 && position == lower)
        {
            direction = 1;
        }

        if (completed_requests < arraySize)
        {
            position += direction;
            head_movement++;
        }
    }
    printf("\n");
    printf("LOOK - Total Head Movements = %d\n", head_movement);
    printf("\n");
}

// C-Look
void C_LOOK(int inital, int direction, int *sorted_requests, int arraySize)
{

    // Initialize some values
    int head_movement = 0;
    int completed_requests = 0;
    int position = inital;
    int local_request[NUM_REQUEST];

    for (int i = 0; i < arraySize; i++)
    {
        local_request[i] = sorted_requests[i];
    }

    int upper = local_request[19];
    int lower = local_request[0];
    printf("C-LOOK DISK SCHEDULING ALGORITHM \n");

    // Loop until all requests met
    while (completed_requests < arraySize)
    {
        // Check for match
        for (int i = 0; i < arraySize; i++)
        {
            if (position == local_request[i])
            {
                printf("%d, ", position);
                local_request[i] = -1;
                completed_requests++;
            }
        }

        // Check if reached end of look, change position to other end of array
        if (direction == 1 && position == upper)
        {
            position = lower;
            head_movement += upper - lower;
        }
        else if (direction == -1 && position == lower)
        {
            position = upper;
            head_movement += upper - lower;
        }
        else
        {
            if (completed_requests < arraySize)
            {
                position += direction;
                head_movement++;
            }
        }
    }
    printf("\n");
    printf("C-LOOK - Total Head Movements = %d\n", head_movement);
    printf("\n");
}

// Q-sort used from https://www.tutorialspoint.com/c_standard_library/c_function_qsort.htm
int cmpfunc(const void *a, const void *b)
{
    return (*(int *)a - *(int *)b);
}

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        printf("INVALID ARGUMENTS, Expecting 2 arguments\n");
    }
    else
    {

        // Retrieving command line arguments
        int initial_position = atoi(argv[1]);
        int direction;

        if (strcmp(argv[2], "LEFT") == 0)
        {
            direction = -1;
        }
        else if (strcmp(argv[2], "RIGHT") == 0)
        {
            direction = 1;
        }
        else
        {
            printf("INVALID DIRECTION, Expecting 'LEFT' or 'RIGHT'\nDEFUALTING TO RIGHT\n");
            direction = 1;
        }

        // Open and map requests
        int mmapfile_fd = open("./request.bin", O_RDONLY);
        mmapfptr = mmap(0, MEMORY_SIZE, PROT_READ, MAP_PRIVATE, mmapfile_fd, 0);

        // Copy into request array
        for (int i = 0; i < NUM_REQUEST; i++)
        {
            memcpy(requestArray + i, mmapfptr + (4 * i), REQUEST_SIZE);
            sortedRequests[i] = requestArray[i];
        }

        munmap(mmapfptr, MEMORY_SIZE);

        // Print initial information
        printf("Total Requests: %d \n", NUM_REQUEST);
        printf("Initial Head Position: %d \n", initial_position);
        if (direction == 1)
        {
            printf("Direction of Head: RIGHT \n");
        }
        else if (direction == -1)
        {
            printf("Direction of Head: LEFT \n");
        }

        printf("\n");

        // Q-sort used from https://www.tutorialspoint.com/c_standard_library/c_function_qsort.htm
        qsort(sortedRequests, NUM_REQUEST, REQUEST_SIZE, cmpfunc);

        FCFS(initial_position, requestArray, NUM_REQUEST);
        SSTF(initial_position, requestArray, NUM_REQUEST);
        SCAN(initial_position, direction, sortedRequests, NUM_REQUEST);
        C_SCAN(initial_position, direction, sortedRequests, NUM_REQUEST);
        LOOK(initial_position, direction, sortedRequests, NUM_REQUEST);
        C_LOOK(initial_position, direction, sortedRequests, NUM_REQUEST);
    }

    return 0;
}