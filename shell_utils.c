//
// Created by ubuntie on 31/05/18.
//
#include <shell.h>
#include "job_control.h"
#include "shell_utils.h"

int edit_job(job *list, job *item, enum job_state state) {
    job *aux = list;

    while (aux->next != NULL && aux->pgid != item->pgid) {
        aux = aux->next;
    }

    if (aux->pgid == item->pgid) {
        aux->state = state;
    }
}

void execute_on_foreground(int child_pid, int parent_pid, job *processes) {
    int status;
    enum status process_status;

    job * new_job = get_item_bypid(processes, child_pid);

    printf("Foreground pid: %d, command: %s, (Status: %s) \n", child_pid, new_job->command,
           status_strings[process_status]);

    wait_child(child_pid, parent_pid, &status);

    printf("\n"); // To avoid collapsing lines

    process_status = getStatus(status);

    update_processes(processes, &process_status, new_job);

    fflush(stdout);
}

void update_processes(job *processes, const enum status *process_status, job *new_job) {
    block_SIGCHLD();
    edit_job(processes, new_job, STOPPED);
    printf(ANSI_COLOR_BLUE "Proccess %s stopped (Status: %s)\n" ANSI_COLOR_RESET, new_job->command, state_strings[(*process_status)]);

    if ((*process_status) == EXITED || (*process_status) == SIGNALED)
        delete_job(processes, new_job);

    unblock_SIGCHLD();
}

void wait_child(int child_pid, int parent_pid, int *status) {
    set_terminal(child_pid);
    waitpid(child_pid, status, WUNTRACED);
    set_terminal(parent_pid);
}


enum status getStatus(int status) {
    enum status proccess_status;
    int info;

    proccess_status = analyze_status(status, &info);

    return proccess_status;
}
