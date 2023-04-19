#include <stdlib.h>

#include "builtin.h"
#include "utils.h"
#include "execute.h"

void lsh_loop() {
    char *line;
    char *cline;
    char **args;
    int status;

    do {
        print_prompt();
        line = lsh_read_line();
        cline = clean_line(line);
        args = lsh_split_line(cline);
        status = lsh_execute(args);

        free(line);
        free(cline);
        free(args);
    } while (status >= 0);
}

int main() {
    lsh_loop();

    exit(EXIT_SUCCESS);
}
