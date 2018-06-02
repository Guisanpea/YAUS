//
// Created by ubuntie on 2/06/18.
//

#ifndef SHELL_SHELL_UTILS_H
#define SHELL_SHELL_UTILS_H

#include "job_control.h"

enum status getStatus(int status);

void wait_child(int child_pid, int parent_pid, int *status);

void update_processes(job *processes, const enum status *process_status, job *new_job);

int edit_job(job *list, job *item, enum job_state state);

void execute_on_foreground(int child_pid, int parent_pid, job * processes);

#endif //SHELL_SHELL_UTILS_H
