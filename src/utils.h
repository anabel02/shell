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
#define BOLD_RED "\033[1;31m"
#define WHITE "\033[0m"


char *lsh_read_line(void);
char *lsh_clean_line(char *line);
char **lsh_split_line(char *line);
void lsh_print_prompt();
List* kmp_matcher(char *text, char *pattern);
int is_special_char_or_blank(char c);
int string_to_unsigned_int(char *str);