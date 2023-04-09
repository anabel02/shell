//
// Created by anabel on 4/1/2023.
//

#include "utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pwd.h>
#include <bits/local_lim.h>

#include "builtin.h"

#define BOLD_CYAN "\033[1;36m"

char *lsh_read_line(void) {
    size_t size = 64;
    char *line = malloc(size);
    int pos = 0;
    char cur_char;

    do {
        if (scanf("%c", &cur_char) == EOF) {
            return line;
        }
        if (pos == size - 1) {
            size *= 2;
            line = realloc(line, size);
        }
        line[pos++] = cur_char;
    } while(cur_char != '\n');

    line[pos] = '\0';
    return line;
}

char **lsh_split_line(char *line) {
    char token_delim[] = " \t\r\n\a";
    size_t buf_size = 64;
    int pos = 0;
    char **tokens = malloc(buf_size * sizeof(char*));
    char *token;

    if(!tokens) {
        fprintf(stderr, "lsh: allocation error\n");
        exit(EXIT_FAILURE);
    }

    token = strtok(line, token_delim);

    while (token != NULL) {
        tokens[pos++] = token;
        if (pos >= buf_size) {
            buf_size *= 2;
            tokens = realloc(tokens, buf_size * sizeof(char*));
            if (!tokens) {
                fprintf(stderr, "lsh: allocation error\n");
                exit(EXIT_FAILURE);
            }
        }
        token = strtok(NULL, token_delim);
    }
    tokens[pos] = NULL;
    return tokens;
}

void print_prompt() {
    struct passwd* pwd = getpwuid(getuid());

    char hostname[HOST_NAME_MAX];
    gethostname(hostname, HOST_NAME_MAX);

    int capacity = 64;

    char *cwd = malloc(capacity * sizeof(char));

    while (getcwd(cwd, capacity) == NULL) {
        capacity *= 2;
        cwd = realloc(cwd, capacity);
   }
    printf("%s%s@%s%s:%s%s$ ",BOLD_CYAN, pwd->pw_name, BOLD_CYAN, hostname, BOLD_CYAN, cwd);
    free(cwd);
}