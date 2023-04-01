#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <pwd.h>

#include "builtin.h"

char *lsh_read_line(void) {
    size_t size = 64;
    char *line = malloc(size);
    char pos = 0;
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

    line[pos] = 0;
    return line;
}

char **lsh_split_line(char *line)
{
    char token_delim[] = " \t\r\n\a";
    size_t buf_size = 64;
    int pos = 0;
    char **tokens = malloc(buf_size * sizeof(char*));
    char *token;

    if(!token) {
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


void print_prompt()
{
    struct passwd* pwd = getpwuid(getuid());
    char *username = pwd->pw_name;

    int capacity = 64;
    //hacer espacio para {capacity} caracteres
    char *cwd = malloc(capacity * sizeof(char));
    char *to_free = cwd;
    //si no cabe en esa capacidad la amplío
    while (getcwd(cwd, capacity) == NULL) {
        capacity *= 2;
        cwd = realloc(cwd, capacity);
    }
    printf("%s@%s:%s$ ", "my_shell", username, cwd);
    free(to_free);
}

void lsh_loop(void)
{
    char *line;
    char **args;
    int status;

    do {
        print_prompt();
        line = lsh_read_line();
        args = lsh_split_line(line);
        status = lsh_execute(args);

        free(line);
        free(args);
    } while (status >= 0);
}

int main(int argc, char **argv)
{
    lsh_loop();

    return EXIT_SUCCESS;
}