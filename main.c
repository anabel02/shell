#include <stdlib.h>

#include "builtin.h"
#include "utils.h"

#define BOLD_CYAN "\033[1;36m"

void lsh_loop()
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

    exit(EXIT_SUCCESS);
}