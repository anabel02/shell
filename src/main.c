#include <stdlib.h>

#include "builtin.h"
#include "utils.h"
#include "execute.h"
#include "glist.h"

char *replace_again(char *line);
void lsh_loop();


int main() {
    bg_pid_list = new_list();
    dict_keys = new_g_list();
    dict_values = new_g_list();

    lsh_loop();

    exit(EXIT_SUCCESS);
}


void lsh_loop() {
    char *line;
    char *clean_line;
    char *replaced_again_line;
    char **args;
    int status;

    do {
        lsh_load_history();
        lsh_update_background();

        lsh_print_prompt();
        line = lsh_read_line();
        replaced_again_line = replace_again(line);
        if (line[0] != ' ') {
            lsh_save_history(replaced_again_line);
        }
        clean_line = lsh_clean_line(replaced_again_line);
        args = lsh_split_line(clean_line);

        status = lsh_execute(args);

        free(line);
        free(replaced_again_line);
        free(clean_line);
        free(args);
    } while (status >= 0);
}


/**
 * Reemplaza las ocurrencias de \n
 * again \<valid_command\> \n
 * por el comando correspondiente en history.
 */
char *replace_again(char *line) {
    int line_pos = 0;

    List* again_pos_list = kmp_matcher(line, "again");

    size_t max_command_length = 0;
    for (int i = 0; i < history_length; ++i) {
        max_command_length = max_command_length > strlen(history[i]) ? max_command_length : strlen(history[i]);
    }

    char *no_again_line = malloc(strlen(line) + max_command_length * again_pos_list->len);
    if (!no_again_line) {
        print_error("lsh: allocation error\n");
        exit(EXIT_FAILURE);
    }
    int pos = 0;

    for (int i = 0; i < again_pos_list->len; ++i) {
        int again_pos = again_pos_list->array[i];

        //Si el patrón again está contenido en otra palabra, se ignora
        if ((again_pos > 0 && !is_special_char_or_blank(line[again_pos - 1]))
            || !is_special_char_or_blank(line[again_pos + 5])) {
            continue;
        }

        int arg_pos;
        for (arg_pos = again_pos + 6; line[arg_pos] == ' '; ++arg_pos);

        int history_command = string_to_unsigned_int(line + arg_pos);

        if (history_command <= 0 || history_command > history_length) {
            continue;
        }

        for (int j = line_pos; j < again_pos; ++j) {
            no_again_line[pos++] = line[j];
        }
        for (int j = 0; j < strlen(history[history_command - 1]) - 1; ++j) {
            no_again_line[pos++] = history[history_command - 1][j];
        }
        line_pos = arg_pos + history_command / 10 + 1;
    }

    free(again_pos_list);

    no_again_line[pos] = 0;
    strcat(no_again_line, line + line_pos);
    return no_again_line;
}