//
// Created by anabel on 4/1/2023.
//

#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>

#include "list.h"

#define HISTORY_MAX_SIZE 10

char* history[HISTORY_MAX_SIZE];
int history_length;

List* bg_pid_list;

int lsh_cd(char **args);
int lsh_help(char **args);
int lsh_exit(char **args);
int lsh_true(char **args);
int lsh_false(char **args);
int lsh_jobs(char **args);
int lsh_foreground(char **args);
int lsh_history(char **args);

void lsh_load_history();
void lsh_save_history(char *line);
int lsh_num_builtins();
void lsh_update_background();

extern char *builtin_str[];
extern int (*builtin_func[]) (char **);