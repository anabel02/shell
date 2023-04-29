#include <stdlib.h>

#include "builtin.h"
#include "utils.h"
#include "execute.h"

/* malloc comando mas largo por cantidad de again
 * liberar
 * again 9 ; again 8 ; again 6*/

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


/** Reemplaza las ocurrencias de again \<command\> por el comando correspondiente en history cuando es un comando válido.
**/
char *replace_again(char *line) {
    int again_pos = kmp_matcher(line, "again");
    if (again_pos == -1) {
        return line;
    }

    size_t max_command_length = 0;
    for (int i = 0; i < history_length; ++i) {
        max_command_length = max_command_length > strlen(history[i]) ? max_command_length : strlen(history[i]);
    }

    char *no_again_line = malloc(strlen(line) + max_command_length * 10);
    int pos = 0;
    no_again_line[pos] = 0;

    //Si el patrón again está contenido en otra palabra, se ignora
    if ((again_pos > 0 && !is_special_char_or_blank(line[again_pos - 1]))
        || !is_special_char_or_blank(line[again_pos + 5])) {
        int i;
        for (i = 0; i < again_pos + 5 || !is_special_char_or_blank(line[i]); ++i) {
            no_again_line[pos++] = line[i];
        }
        no_again_line[pos] = 0;
        return strcat(no_again_line, replace_again(line + i));
    }

    char* arg = malloc(1024);
    int arg_pos = 0;
    int j;
    int blanks = 0;
    for (j = again_pos + 5; line[j] != 0; ++j) {
        if ((line[j] == ' ' && arg_pos > 0) || is_special_char(line[j])) {
            break;
        } else if (!is_special_char_or_blank(line[j])) {
            arg[arg_pos++] = line[j];
        } else {
            blanks++;
        }
    }
    arg[arg_pos] = 0;
    char *ptr;
    long history_command = strtol(arg, &ptr, 10);

    if (ptr[0] == 0 && history_command > 0 && history_command <= history_length) {
        blanks = 0;
        strcpy(arg, history[history_command - 1]);
        arg[strlen(history[history_command - 1]) - 1] = 0;
    } else if (history_length == 0) {
        printf("%s\n", "lsh: history is empty");
        again_pos += 5;
    } else {
        printf("%s %d\n", "lsh: history index must be an integer between 1 and", history_length);
        again_pos += 5;
    }

    for (int i = 0; i < again_pos; ++i) {
        no_again_line[pos++] = line[i];
    }
    for (int i = 0; i < blanks; ++i) {
        no_again_line[pos++] = ' ';
    }
    no_again_line[pos] = 0;
    strcat(no_again_line, arg);
    free(arg);
    return strcat(no_again_line, replace_again(line + j));
}