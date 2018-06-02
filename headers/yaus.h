#ifndef SHELL_SHELL_UTILS_H
#define SHELL_SHELL_UTILS_H

#include "job_control.h"

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"

#define NELEMS(x)  (sizeof(x) / sizeof((x)[0]))

int number_of_internals();

void execute_background(job* auxJob);

void cd(char ** args);

char * internals_strings[];

void (* internal_func[]) (char **);

enum status getStatus(int status);

void wait_child(int child_pid, int parent_pid, int *status);

void update_processes(job *processes, const enum status *process_status, job *new_job);

int edit_job(job *list, job *item, enum job_state state);

void execute_on_foreground(int child_pid, int parent_pid, job * processes);

void jobs(char ** args);

void fg(char **args);

void bg(char ** args);

void print_welcome();
#endif //SHELL_SHELL_UTILS_H
