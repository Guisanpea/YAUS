cmake_minimum_required(VERSION 3.10)
project(Shell C)

set(CMAKE_C_STANDARD 99)

include_directories(headers)

add_executable(Shell job_control.c Shell_project.c shell_utils.c headers/job_control.h headers/shell.h headers/shell_utils.h)
