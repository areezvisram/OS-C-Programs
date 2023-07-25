/**
 * Note: For history, the table is printed, and then history is added to the table
 * For reading input and freeing memory, the following open source code was referenced:
 * https://github.com/forestLoop/Learning-EI338/blob/master/Project-2-1/simple_shell.c
 */

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <stdlib.h>

#define MAX_LINE 80
#define HISTORY_SIZE 5
char *history[HISTORY_SIZE];
int current_position = -1;
int commands_count = 0;

// prints history of last 5 commands
int print_history(char **args)
{
    // If no commands in history
    if (current_position == -1 || history[current_position] == NULL)
    {
        printf("No commands in history\n");
        return 0;
    }

    int last_position = 0, cur_position = current_position, count = 0;
    if (cur_position != HISTORY_SIZE && history[cur_position + 1] != NULL)
    {
        last_position = cur_position + 1;
    }

    while (count < 5)
    {
        char *command = history[cur_position];
        printf("%d %s\n", commands_count - count, command);
        cur_position = cur_position - 1;
        cur_position = (cur_position + HISTORY_SIZE) % HISTORY_SIZE;
        count++;
    }
}

// add a command to history
void add_to_history(char **args)
{
    current_position = (current_position + 1) % HISTORY_SIZE;
    history[current_position] = malloc(64 * sizeof(char *));
    char **temp_args = args;

    while (*temp_args != NULL)
    {
        strcat(history[current_position], *temp_args);
        strcat(history[current_position], " ");
        temp_args++;
    }
}

// clear the command arguments whenever a new argument is entered, frees memory
void refresh_args(char *args[])
{
    while (*args)
    {
        free(*args);
        *args++ = NULL;
    }
}

// initialize the arguments to NULL to start, assume maximum number of arguments is MAX_LINE / 2 + 1
void initialize_arguments(char *args[])
{
    for (int i = 0; i < MAX_LINE / 2 + 1; ++i)
    {
        args[i] = NULL;
    }
}

// initialize the command to an empty string to start
void initialize_command(char *command)
{
    strcpy(command, "");
}

// read input from the command line
int read_input(char *command)
{
    char input[MAX_LINE + 1];
    fgets(input, MAX_LINE + 1, stdin);
    commands_count++;
    strtok(input, "\n"); // fgets adds a \n, remove it before setting as the command
    strcpy(command, input);
    return 1;
}

// parses the arguments to the command and save in args array, returns the number of total arguments
int parse_args(char *args[], char *original_command)
{
    char command[MAX_LINE];
    strcpy(command, original_command);
    char *parsed = strtok(command, " ");
    int i = 0;
    while (parsed != NULL)
    {
        args[i] = malloc(strlen(parsed) + 1);
        strcpy(args[i], parsed);
        i++;
        parsed = strtok(NULL, " ");
    }
    return i;
}

// check if the last argument passed is an ampersand
int check_ampersand(char **args, int *num_args)
{
    int length = strlen(args[*num_args - 1]);
    if (args[*num_args - 1][length - 1] != '&')
    {
        return 0; // not an ampersand
    }
    if (length == 1) // if only an ampersand, remove from arguments
    {
        free(args[*num_args - 1]);
        args[*num_args - 1] = NULL;
        --(*num_args);
    }
    else
    {
        args[*num_args - 1][length - 1] = '\0'; // remove the ampersand from arguments so it doesn't affect execution of command
    }
    return 1;
}

int main(void)
{
    // variable initialization
    pid_t pid;
    char *args[MAX_LINE / 2 + 1];
    char command[MAX_LINE];
    initialize_arguments(args);
    initialize_command(command);
    int should_run = 1;

    while (should_run)
    {

        // setup
        printf("osh>");
        fflush(stdout);
        fflush(stdin);
        refresh_args(args);

        // ensure input is read
        if (!read_input(command))
        {
            continue;
        }

        // parse arguments from input command
        int num_args = parse_args(args, command);

        // if command is exit, exit the shell
        if (strcmp(args[0], "exit") == 0)
        {
            should_run = 0;
        }

        if (strcmp(args[0], "history") == 0)
        {
            print_history(args);
        }

        if (strcmp(args[0], "!!") == 0)
        {
            // If no commands in history
            if (current_position == -1 || history[current_position] == NULL)
            {
                printf("No commands in history\n");
            }
            else
            {
                refresh_args(args);
                strcpy(command, history[current_position]);
                if (strcmp(command, "history ") == 0)
                {
                    print_history(args);
                }
                int num_args = parse_args(args, command);
                add_to_history(args);
            }
        }
        add_to_history(args);

        // check if parent process should wait for child by detecting presence of ampersand
        int should_wait = check_ampersand(args, &num_args);

        // fork the process and execute the passed command with its args
        pid = fork();

        if (pid == 0)
        {
            execvp(args[0], args);
        }
        else
        {
            // if no ampersand, wait for child process
            if (!should_wait)
            {
                wait(NULL);
            }
        }

        fflush(stdout);
    }

    refresh_args(args);
    return 0;
}