//
// Created by anabel on 4/1/2023.
//

#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int lsh_cd(char **args);
int lsh_help(char **args);
int lsh_exit(char **args);
int lsh_num_builtins();

extern char *builtin_str[];
extern int (*builtin_func[]) (char **);