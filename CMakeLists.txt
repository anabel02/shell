cmake_minimum_required(VERSION 3.16)
project(shell C)

set(CMAKE_C_STANDARD 11)

add_executable(shell src/main.c src/builtin.h src/builtin.c src/utils.c src/utils.h src/execute.h src/execute.c src/list.c src/list.h src/help.c src/help.h src/glist.h src/glist.c)
