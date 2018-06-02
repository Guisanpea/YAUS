/**
UNIX Shell Project

Sistemas Operativos
Grados I. Informatica, Computadores & Software
Dept. Arquitectura de Computadores - UMA

Some code adapted from "Fundamentos de Sistemas Operativos", Silberschatz et al.

To compile and run the program:
   $ gcc Shell_project.c job_control.c -o Shell
   $ ./Shell          
	(then type ^D to exit program)

**/

#include <stdbool.h>
#include <string.h>
#include "job_control.h"   // remember to compile with module job_control.c
#include "yaus.h"
#include "shell_project.h"

#define MAX_LINE 256 /* 256 chars per line, per command, should be enough. */


// -----------------------------------------------------------------------
//                        GLOBAL VARIABLES
// -----------------------------------------------------------------------

// -----------------------------------------------------------------------
//                       FUNCTION DEFINITIONS
// -----------------------------------------------------------------------

void process_command(int background, char *args[128]);

void execute_as_child(char *args[]);

void act_as_parent(char **args, int background, int child_pid);

bool isChild(int pid_fork);

void print_prompt();

int treat_child(int i, enum status status_res, job *job);

void initialization();

void process_internal(char **args, bool *isInternal);

void print_welcome();

job * get_processes();

// -----------------------------------------------------------------------
//                       SIGCHLD HANDLER
// -----------------------------------------------------------------------



void sigchld_handler(int signum) {
    int i, _status, info, pid_wait, list_length;
    enum status status_res;
    job *job;

    printf(ANSI_COLOR_RESET "SIGCHLD received\n");

    for (i = 1; i <= list_size(processes); i++) {
        job = get_item_bypos(processes, i);
        pid_wait = waitpid(job->pgid, &_status, WUNTRACED || WNOHANG);

        if (pid_wait == job->pgid) {
            status_res = analyze_status(_status, &info);
            printf("%s#%i received signal (Status: %s)\n", job->command, job->pgid, status_strings[status_res]);

            i = treat_child(i, status_res, job);
        }
    }

    fflush(stdout);
}

int treat_child(int i, enum status status_res, job *job) {
    block_SIGCHLD();
    if ((status_res) == SUSPENDED) edit_job(processes, job, STOPPED);
    else if ((status_res) == EXITED || (status_res) == SIGNALED) {
        delete_job(processes, job);
        i--;
    }
    unblock_SIGCHLD();

    tcsetattr(shell_terminal, TCSANOW, &conf);

    return i;
}

// -----------------------------------------------------------------------
//                            MAIN
// -----------------------------------------------------------------------

int main(void) {
    char inputBuffer[MAX_LINE]; /* buffer to hold the command entered */
    int background;             /* equals 1 if a command is followed by '&' */
    char *args[MAX_LINE / 2];     /* command line (of 256) has max of 128 arguments */

    initialization();
    print_welcome();

    while (true) {
        ignore_terminal_signals();

        print_prompt();
        fflush(stdout);
        get_command(inputBuffer, MAX_LINE, args, &background);  /* get next command */

        if (!args[0] == NULL) {   // if empty command
            process_command(background, args);
        }
    } // end while
}

// -----------------------------------------------------------------------
//                       FUNCTION DEFINITIONS
// -----------------------------------------------------------------------

void initialization() {
    processes = new_list("jobs");

    shell_terminal = STDIN_FILENO;
    tcgetattr(shell_terminal, &conf);
    signal(SIGCHLD, sigchld_handler);
}

void print_prompt() {
    char current_dir[1024];
    getcwd(current_dir, 1024);
    printf(ANSI_COLOR_GREEN "%s"  ANSI_COLOR_CYAN " $ " ANSI_COLOR_RESET, current_dir);
}

void process_command(int background, char *args[128]) {
    bool isInternal = false;
    process_internal(args, &isInternal);

    if (!isInternal) {
        int pid_fork = fork();

        if (pid_fork == -1) {
            printf(ANSI_COLOR_RED "Error creating a child!\n");
            exit(EXIT_FAILURE);
        }

        if (isChild(pid_fork)) {
            execute_as_child(args);
        } else {
            act_as_parent(args, background, pid_fork);
        }
    }
}

void process_internal(char **args, bool *isInternal) {
    for (int i = 0; i < number_of_internals(); ++i) {
        char *s2 = internals_strings[i];
        if (strcmp(args[0], s2) == 0) {
            *isInternal = true;
            (*internal_func[i])(args);
        }
    }
}

bool isChild(int pid_fork) { return pid_fork == 0; }

void execute_as_child(char *args[]) {
    new_process_group(getpid());
    restore_terminal_signals();

    printf(ANSI_COLOR_RESET); //Force defult color
    execvp(args[0], args);

    printf(ANSI_COLOR_RED "Error, command not found: %s \n", args[0]);
    exit(EXIT_FAILURE);
}

void act_as_parent(char **args, int background, int child_pid) {

    block_SIGCHLD();
    add_job(processes, new_job(child_pid, args[0], background ? BACKGROUND : FOREGROUND));
    unblock_SIGCHLD();

    if (!background) {
        execute_on_foreground(child_pid, getpid(), processes);
        tcsetattr(shell_terminal, TCSANOW, &conf);
    } else {
        printf("Background job running... pid: %d, command: %s \n", child_pid, args[0]);
    }
}