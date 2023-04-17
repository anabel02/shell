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


#define BOLD_CYAN "\033[1;36m"
typedef int bool_t;
#define TRUE 1
#define FALSE 0

/* REVISAR MENSAJES DE ERROR*/

char specialChars[] = {'|', '<', '>', ';', '&', '\"'};

bool_t is_special_char(char c) {
    for (int i = 0; i < strlen(specialChars); ++i) {
        if(specialChars[i] == c) {
            return TRUE;
        }
    }
    return FALSE;
}

char *lsh_read_line(void) {
    size_t size = 64;
    char *line = malloc(size);
    int pos = 0;
    char c;

    do {
        if (scanf("%c", &c) == EOF) {
            return line;
        }
        if (pos == size - 1) {
            size *= 2;
            line = realloc(line, size);
        }
        line[pos++] = c;
    } while(c != '\n');

    line[pos] = '\0';
    return line;
}

char *clean_line(char *line) {
    size_t size = strlen(line);
    char *clean_line = malloc(size * 2);
    int pos = 0;
    int quotes = 0;

    if (!clean_line) {
        fprintf(stderr, "lsh: allocation error\n");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < strlen(line); ++i) {
        if(line[i] == '\"') {
            quotes++;
            if (quotes % 2 == 1 && i > 0 && line[i-1] != ' ') {
                clean_line[pos++] = ' ';
            }
            clean_line[pos++] = line[i];
            if (quotes % 2 == 0) {
                clean_line[pos++] = ' ';
            }
            continue;
        }
        if (quotes % 2 == 1) {
            clean_line[pos++] = line[i];
            continue;
        }
        if (line[i] == '#') {
            break;
        } else if (line[i] == ' ') {
            if (pos == 0 || clean_line[pos - 1] == ' ') {
                continue;
            }
            clean_line[pos++] = ' ';
        } else if (is_special_char(line[i]) == FALSE) {
            clean_line[pos++] = line[i];
            continue;
        } else if (line[i] == '<') {
            if (i > 0 && line[i-1] == '<') {
                continue;
            }
            if (line[i-1] == ' ') {
                clean_line[pos++] = line[i];
            } else {
                clean_line[pos++] = ' ';
                clean_line[pos++] = line[i];
            }
            if (line[i+1] == '<') {
                clean_line[pos++] = line[i+1];
            }
            clean_line[pos++] = ' ';
        } else if (line[i] == '>') {
            if (i > 0 && line[i-1] == '>') {
                continue;
            }
            if (line[i-1] == ' ') {
                clean_line[pos++] = line[i];
            } else {
                clean_line[pos++] = ' ';
                clean_line[pos++] = line[i];
            }
            if (line[i+1] == '>') {
                clean_line[pos++] = line[i+1];
            }
            clean_line[pos++] = ' ';
            continue;
        }  else if (line[i] == '|') {
            if (i > 0 && line[i - 1] == '|') {
                continue;
            }
            if (line[i - 1] == ' ') {
                clean_line[pos++] = line[i];
            } else {
                clean_line[pos++] = ' ';
                clean_line[pos++] = line[i];
            }
            if (line[i + 1] == '|') {
                clean_line[pos++] = line[i + 1];
            }
            clean_line[pos++] = ' ';
            continue;
        } else if (line[i] == '&') {
            if (i > 0 && line[i - 1] == '&') {
                continue;
            }
            if (line[i - 1] == ' ') {
                clean_line[pos++] = line[i];
            } else {
                clean_line[pos++] = ' ';
                clean_line[pos++] = line[i];
            }
            if (line[i + 1] == '&') {
                clean_line[pos++] = line[i + 1];
            }
            clean_line[pos++] = ' ';
            continue;
        } else if (line[i] == ';') {
            if (line[i - 1] == ' ') {
                clean_line[pos++] = line[i];
            } else {
                clean_line[pos++] = ' ';
                clean_line[pos++] = line[i];
            }
            clean_line[pos++] = ' ';
            continue;
        }
    }
    clean_line[pos] = '\0';
    return clean_line;
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