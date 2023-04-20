#include <stdlib.h>

#include "builtin.h"
#include "utils.h"
#include "execute.h"

char *again(char *line) {
    int again_pos = kmp_matcher(line, "again");
    if (again_pos == -1) {
        return line;
    }
    char *no_again_line = malloc(strlen(line) * 5);
    int pos = 0;
    no_again_line[pos] = 0;
    int j;
    char* arg = malloc(1024);
    int arg_pos = 0;
    arg[0] = 0;
    for (j = again_pos + 5; line[j] != 0; ++j) {
        if(line[j] != ' ' && !is_special_char(line[j])) {
            arg[arg_pos++] = line[j];
        } else if (arg_pos > 0) {
            break;
        }
    }

    long history_command = strtol(arg, NULL, 10);

    if (history_command > 0 && history_command <= history_length) {
        strcpy(arg, history[history_command - 1]);
        arg[strlen(history[history_command - 1]) - 1] = 0;
    } else {
        printf("lsh: error near again\n");
        return NULL;
    }

    for (int i = 0; i < again_pos; ++i) {
        no_again_line[pos++] = line[i];
    }

    no_again_line[pos] = 0;
    strcat(no_again_line, arg);
    if (line[j] != 0) {
        strcat(no_again_line, again(line + j));
    }
    return no_again_line;
}


void lsh_loop() {
    char *line;
    char *cline;
    char *again_line;
    char **args;
    int status;

    do {
        lsh_load_history();

        lsh_print_prompt();
        line = lsh_read_line();
        again_line = again(line);
        if (again_line == NULL) {
            status = 1;
            continue;
        }
        if (line[0] != ' ') {
            lsh_save_history(again_line);
        }
        cline = lsh_clean_line(again_line);
        args = lsh_split_line(cline);

        lsh_update_background();

        status = lsh_execute(args);

        free(line);
        free(cline);
        free(args);
    } while (status >= 0);
}


int main() {
    bg_pid_list = new_list();

    lsh_loop();

    exit(EXIT_SUCCESS);
}
