//
// Created by anabel on 4/1/2023.
//

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pwd.h>
#include <bits/local_lim.h>

#include "list.h"

#define BOLD_CYAN "\033[1;36m"

#define TRUE 1
#define FALSE 0

char *lsh_read_line(void);
char *lsh_clean_line(char *line);
char **lsh_split_line(char *line);
void lsh_print_prompt();
int *compute_prefix_function(char *pattern);
List* kmp_matcher(char *text, char *pattern);
int is_special_char(char c);
int is_special_char_or_blank(char c);
int string_to_unsigned_int(char *str);