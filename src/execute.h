//
// Created by anabel on 4/16/2023.
//

#ifndef SHELL_EXECUTE_H
#define SHELL_EXECUTE_H

#endif //SHELL_EXECUTE_H

#define BOLD_RED "\033[1;31m"

#include <fcntl.h>
#include "builtin.h"
#include "utils.h"

int lsh_execute(char **args);
int lsh_execute_conditional(char **args);
int find_end(char **args);
int parse_set(char **args);
int lsh_stdout_to_buffer(char **args, char *buffer);
int lsh_execute_set(char **args);
int lsh_execute_chain(char **args);
int lsh_background(char **args);
int lsh_execute_redirections_out(char **args, int fd_in, int fd_out);
int lsh_execute_redirections_in(char **args, int fd_in, int fd_out);
int lsh_execute_simple(char **args, int fd_in, int fd_out);
int lsh_launch(char **args, int fd_in, int fd_out);