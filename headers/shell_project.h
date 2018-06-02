//
// Created by ubuntie on 2/06/18.
//

#ifndef SHELL_SHELL_PROJECT_H
#define SHELL_SHELL_PROJECT_H

#include "job_control.h"

// -----------------------------------------------------------------------
//                        GLOBAL VARIABLES
// -----------------------------------------------------------------------

int shell_terminal;
struct termios conf;
job *processes;

#endif //SHELL_SHELL_PROJECT_H
