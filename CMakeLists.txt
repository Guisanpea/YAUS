cmake_minimum_required(VERSION 3.10)
project(Shell C)

set(CMAKE_C_STANDARD 99)

include_directories(headers)

add_executable(Shell job_control.c shell_project.c yaus.c headers/yaus.h headers/job_control.h headers/shell_project.h)
