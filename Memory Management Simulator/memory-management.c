#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/mman.h>

// Define the needed macros
#define BUFFER_SIZE 10
#define OFFSET_MASK 255
#define PAGES 256
#define OFFSET_BITS 8
#define PAGE_SIZE 256
#define LOGICAL_MEM_SIZE PAGE_SIZE *PAGES
#define DATA_COUNT 128
#define TLB_SIZE 16

// Global variables
int page_table[PAGES];
signed int data_table[DATA_COUNT][PAGE_SIZE];
int pass_complete = 0;
signed int current_value;
typedef struct
{
    int valid, page_number, frame_number;
} TLBEntry;
int TLB_pointer;
char *mmapfptr;
int frame_counter = 0;
int page_fault_count = 0;
int address_count = 0;
int TLB_hit_count = 0;
TLBEntry TLB[TLB_SIZE];

// Update the TLB at a given page and update the frame number
void update_TLB(int page_number, int frame_number)
{
    int victim = TLB_pointer % TLB_SIZE;
    TLB_pointer = (TLB_pointer + 1) % TLB_SIZE;
    TLB[victim].valid = 1;
    TLB[victim].page_number = page_number;
    TLB[victim].frame_number = frame_number;
}

// Look for a TLB entry corresponding to a page number (checks for a TLB hit)
int search_TLB(int page_number, int *frame_number)
{
    for (int i = 0; i < TLB_SIZE; i++)
    {
        if (TLB[i].valid && TLB[i].page_number == page_number)
        {
            *frame_number = TLB[i].frame_number;
            TLB_hit_count++;
            return 1;
        }
    }
    return 0;
}

void handle_page_fault(int page_number, int logical_address)
{
    // Copy into memory and value for print statement
    memcpy(data_table + frame_counter, mmapfptr + page_number * PAGE_SIZE, PAGE_SIZE);

    // After first pass all memory needs to be replaced and removed from page table
    // This finds the corresponding page table entry and sets it to display empty
    if (pass_complete)
    {
        for (int i = 0; i < PAGES; i++)
        {
            if (page_table[i] == frame_counter)
            {
                page_table[i] = -1;
            }
        }
    }
    // Frame_counter is current frame
    page_table[page_number] = frame_counter;

    // Circular array
    frame_counter++;
    if (frame_counter > DATA_COUNT - 1)
    {
        frame_counter = 0;
        pass_complete = 1;
    }
}

// Given a logical address compute the physical address
int get_physical_address(int logical_address)
{
    int page_number = logical_address >> OFFSET_BITS;
    // printf("page number %d \n", page_number);
    int page_offset = logical_address & OFFSET_MASK;
    int frame_number;
    // TLB miss
    if (!search_TLB(page_number, &frame_number))
    {
        // Not in page table, handle page fault
        if (page_table[page_number] == -1)
        {
            page_fault_count++;
            handle_page_fault(page_number, logical_address);
        }
        frame_number = page_table[page_number];

        // Update TLB with new entry
        update_TLB(page_number, frame_number);
    }
    // Retrieve current page frame data and extract the offset byte
    int physical_address = (frame_number << OFFSET_BITS) | page_offset;
    current_value = *data_table[frame_number] >> (8 * page_offset) & 0xff;
    return physical_address;
}

int main(int argc, char *argv[])
{
    // Initially Page Table is empty
    for (int i = 0; i < PAGES; i++)
        page_table[i] = -1;

    TLB_pointer = 0;
    int logical_address;

    // Open and map BACKING_STORE
    int mmapfile_fd = open("./BACKING_STORE.bin", O_RDONLY);
    mmapfptr = mmap(0, LOGICAL_MEM_SIZE, PROT_READ, MAP_PRIVATE, mmapfile_fd, 0);

    // Open logical addresses
    FILE *fptr = fopen("addresses.txt", "r");
    char buff[BUFFER_SIZE];

    // Read logical addresses, get physical address, value and print
    while (fgets(buff, BUFFER_SIZE, fptr) != NULL)
    {
        buff[strcspn(buff, "\n")] = 0;
        logical_address = atoi(buff);
        int physical_address = get_physical_address(logical_address);
        printf("Virtual address: %d Physical address: %d Value: %d \n", logical_address, physical_address, current_value);
        address_count++;
    }
    fclose(fptr);
    munmap(mmapfptr, LOGICAL_MEM_SIZE);

    // Output statistics
    printf("Total addresses: %d \nTotal Page Faults: %d \nTotal TLB hits: %d \n", address_count, page_fault_count, TLB_hit_count);

    return 0;
}