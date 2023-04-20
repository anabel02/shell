#include <stdlib.h>

#include "builtin.h"
#include "utils.h"
#include "execute.h"

/* history | grep his    arreglar redirecciones con builtin
 * malloc*/

char *replace_again(char *line);
void lsh_loop();


int main() {
    bg_pid_list = new_list();

    lsh_loop();

    exit(EXIT_SUCCESS);
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
        again_line = replace_again(line);
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


/** Reemplaza las ocurrencias de again \<command\> por el comando correspondiente en history\n
 * Devuelve NULL si existe un error de sintaxis
**/
char *replace_again(char *line) {
    int again_pos = kmp_matcher(line, "again");
    if (again_pos == -1) {
        return line;
    }

    char *no_again_line = malloc(strlen(line) * 5);
    int pos = 0;
    no_again_line[pos] = 0;

    if ((again_pos > 0 && !special_char_or_blank(line[again_pos - 1]))
        || !special_char_or_blank(line[again_pos + 5])) {
        int i;
        for (i = 0; i < again_pos + 5 || !special_char_or_blank(line[i]); ++i) {
            no_again_line[pos++] = line[i];
        }
        no_again_line[pos] = 0;
        char *post_again = replace_again(line + i);
        if (post_again == NULL) {
            return NULL;
        }
        return strcat(no_again_line, post_again);
    }

    char* arg = malloc(1024);
    int arg_pos = 0;
    int j;
    for (j = again_pos + 5; line[j] != 0; ++j) {
        if (!special_char_or_blank(line[j])) {
            arg[arg_pos++] = line[j];
        } else if (arg_pos > 0) {
            break;
        }
    }
    arg[arg_pos] = 0;
    char *ptr;
    long history_command = strtol(arg, &ptr, 10);

    if (ptr[0] == 0 && history_command > 0 && history_command <= history_length) {
        strcpy(arg, history[history_command - 1]);
    } else {
        free(arg);
        printf("%s\n", "lsh: error near again, again is a reserved keyword");
        return NULL;
    }

    for (int i = 0; i < again_pos; ++i) {
        no_again_line[pos++] = line[i];
    }

    no_again_line[pos] = 0;
    strcat(no_again_line, arg);
    char *post_again = replace_again(line + j);
    if(post_again == NULL) {
        return NULL;
    }
    strcat(no_again_line, post_again);
    return no_again_line;
}