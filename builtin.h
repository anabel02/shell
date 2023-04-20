//
// Created by anabel on 4/1/2023.
//

#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "list.h"

List* bg_pid_list;

int lsh_cd(char **args);
int lsh_help(char **args);
int lsh_exit(char **args);
int lsh_true(char **args);
int lsh_false(char **args);
int lsh_jobs(char **args);
int lsh_foreground(char **args);


int lsh_num_builtins();
void lsh_update_background();

extern char *builtin_str[];
extern int (*builtin_func[]) (char **);