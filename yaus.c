//
// Created by ubuntie on 31/05/18.
//
#include "shell_project.h"
#include "job_control.h"
#include "yaus.h"

char *internals_strings[] = {
        "cd",
        "jobs",
        "fg",
        "bg"
};

void (*internal_func[])(char **) = {
        &cd,
        &jobs,
        &fg,
        &bg
};

void process_job(job *background_job);

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

    job *new_job = get_item_bypid(processes, child_pid);

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
    printf(ANSI_COLOR_BLUE "Proccess %s stopped (Status: %s)\n" ANSI_COLOR_RESET, new_job->command,
           state_strings[(*process_status)]);

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

void execute_background(job *auxJob) {
    if (auxJob->state == STOPPED) killpg(auxJob->pgid, SIGCONT);
}

void cd(char **args) {
    chdir(args[1]);
}

int number_of_internals() {
    return NELEMS(internals_strings);
}

void jobs(char **args) {
    job *process_list = processes;
    printf(ANSI_COLOR_MAGENTA);
    print_job_list(process_list);
    printf(ANSI_COLOR_RESET);

}

void fg(char **args) {
    job *background_job;
    int job_to_foreground = args[1] != NULL ? atoi(args[1]) : 1;

    if (!empty_list(processes)) {
        block_SIGCHLD();
        background_job = get_item_bypos(processes, job_to_foreground);
        unblock_SIGCHLD();

        process_job(background_job);
    }
}

void process_job(job *background_job) {
    if (background_job != NULL) {
        execute_background(background_job);

        block_SIGCHLD();
        edit_job(processes, background_job, FOREGROUND);
        unblock_SIGCHLD();

        execute_on_foreground(background_job->pgid, getpid(), processes);
        tcsetattr(shell_terminal, TCSANOW, &conf);
    }

}

void bg(char ** args){
    job * background_job;
    if (!empty_list(processes)) {
        block_SIGCHLD();
        background_job = get_item_bypos(processes, (args[1] != NULL) ? atoi(args[1]) : 1);
        unblock_SIGCHLD();
        if (background_job != NULL) {
            execute_background(background_job);
        } else {
            printf(ANSI_COLOR_RED"\n Sorry but entry number %d hasn't been found"ANSI_COLOR_RESET, atoi(args[1]));
        }
    } else {
        printf(ANSI_COLOR_RED"\nThere is not any job available\n"ANSI_COLOR_RESET);
    }
}

void print_welcome() {
    printf(ANSI_COLOR_YELLOW);
    printf("                        ,.    Welcome\n");
    printf("                       /-|       to\n");
    printf("                      (--;     "ANSI_COLOR_BLUE"Y et\n"ANSI_COLOR_YELLOW);
    printf("                     (,-';     "ANSI_COLOR_BLUE"A nother\n"ANSI_COLOR_YELLOW);
    printf("                   _/_.-';     "ANSI_COLOR_BLUE"U nix\n"ANSI_COLOR_YELLOW);
    printf("                 _/-.__._<     "ANSI_COLOR_BLUE"S hell\n"ANSI_COLOR_YELLOW);
    printf("              .-'`-.__   '\\\n");
    printf("           .'`---=___`===':\n");
    printf("          /_..---' ___.--.'\n");
    printf("         |` ___.--' __ .i|\n");
    printf("         |-' ___.--'_.8:E|\n");
    printf("         \\,-'  __.-/88::E!\n");
    printf("          `-.,' _.'|88::E|\n");
    printf("             `;'../88:: E;\n");
    printf("              |.'!88::E\\\"/\n");
    printf("             /.''!\"iiE /\n");
    printf("             `--'`._.-'\n");
    printf(ANSI_COLOR_RESET"Check my source code on " ANSI_COLOR_BLUE "https://github.com/Guisanpea/YAUS\n" ANSI_COLOR_RESET);
}