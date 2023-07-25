# Operating System Programs in C

This repository contains a collection of operating system programs implemented in C. Each project showcases different aspects of OS concepts, algorithms, and solutions.

## Projects

**Disk Scheduling Algorithms**
   - Description: This project simulates various OS disk scheduling algorithms with a given input of disk requests. It allows you to explore different disk scheduling strategies and compare their performance.
   - Disk Scheduling Algorithms Implemented: FCFS, SSTF, SCAN, C-SCAN, LOOK, C-LOOK
   - How to Run: 
        - Navigate to program directory
        - Execute `gcc scheduling-algorithms.c -o scheduling-algorithms`
        - Execute `./scheduling-algorithms ARG1 ARG2`
        - `ARG1` is an integer between 0 and 299 representing the initial head position
        - `ARG2` is a string either `LEFT` or `RIGHT` representing the direction of the head

**Memory Management Simulator**
   - Description: This project simulates the memory management of an OS, translating logical addresses to physical addresses. It includes address translation, handling of page faults, and a TLB (Translation Lookaside Buffer).
   - How to Run:
    - Navigate to program directory
        - Execute `gcc memory-management.c -o memory-management`
        - Execute `./memory-management`

**OS Shell**
   - Description: This project simulates an operating system shell, providing a shell interface where users can execute UNIX commands. It also includes a history feature that displays the history of past commands.
   - How to Run:
        - Navigate to program directory
        - Execute `gcc shell.c -o shell`
        - Execute `./shell`

**Sleeping TA Solution**
   - Description: This project implements a multithreaded solution to the Sleeping TA problem using POSIX threads, mutex locks, and semaphores. It showcases synchronization mechanisms to manage students seeking help from a TA.
   - How to Run:
        - Navigate to program directory
        - Execute `gcc sleeping-ta.c -o sleeping-ta`
        - Execute `./sleeping-ta`

**Time Kernel Module**
   - Description: This project implements a proc kernel module that reports the number of seconds elapsed since the module was loaded. It allows you to explore kernel module development and interaction with the kernel space.
   - How to Run:
        - Navigate to program directory
        - Execute `gcc seconds-kernel.c -o seconds-kernel`
        - Execute `./seconds-kernel`

## Prerequisites

Before running any of the projects, make sure you have the necessary tools and libraries installed. Ensure you have a C compiler (e.g., GCC) and any specific dependencies required for each project.

## Usage

To run any of the projects, navigate to the respective project folder and follow the instructions provided in the project description above to run it.