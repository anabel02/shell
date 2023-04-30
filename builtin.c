//
// Created by anabel on 4/1/2023.
//

#include "builtin.h"


char *builtin_str[] = {
        "cd",
        "exit",
        "true",
        "false",
        "fg",
        "again",
        "unset"
};


int (*builtin_func[]) (char **) = {
        &lsh_cd,
        &lsh_exit,
        &lsh_true,
        &lsh_false,
        &lsh_foreground,
        &lsh_again,
        &lsh_unset
};


int lsh_num_builtins() {
    return sizeof(builtin_str) / sizeof(char *);
}


char *builtin_str_out[] = {
        "help",
        "jobs",
        "history",
        "get"
};


int (*builtin_func_out[]) (char **) = {
        &lsh_help,
        &lsh_jobs,
        &lsh_history,
        &lsh_get
};


int lsh_num_builtins_out() {
    return sizeof(builtin_str_out) / sizeof(char *);
}


int lsh_cd(char **args) {
    if (args[1] == NULL) {
        if (chdir(getenv("HOME")) == 0) return 0;
        perror("lsh: cd: ");

    } else if (args[2] != NULL){
        fprintf(stderr, "%s%s", BOLD_RED, "lsh: cd: too many arguments\n");
    } else {
        if (chdir(args[1]) == 0) return 0;
        perror("lsh : cd");
    }
    return 1;
}


int lsh_help(char **args) {
    if (args[1] == NULL) {
        printf("%s", commands_help[0]);
    } else if (args[2] != NULL){
        fprintf(stderr, "%s%s", BOLD_RED, "lsh: help: too many arguments\n");
        return 1;
    } else {
        for (int i = 0; i < lsh_num_commands_help(); i++) {
            if (strcmp(args[1], commands[i]) != 0) continue;
            printf("%s: %s", commands[i], commands_help[i]);
            return 0;
        }
        fprintf(stderr, "%s%s", BOLD_RED, "lsh: help: command not found.  Try `help help' or 'help'.\n");
        return 1;
    }
    return 0;
}


int lsh_exit(char **args) {
    exit(EXIT_SUCCESS);
}


int lsh_true(char **args) {
    return 0;
}


int lsh_false(char **args) {
    return 1;
}


int lsh_jobs(char **args) {
    if (args[1] != NULL) {
        fprintf(stderr, "%s%s", BOLD_RED, "lsh: jobs: too many arguments\n");
        return 1;
    } else {
        for (int i = 0; i < bg_pid_list->len; ++i) {
            fprintf(stdout,"[%d] %d\n", i + 1, get(bg_pid_list, i));
        }
    }
    return 0;
}


int lsh_foreground(char **args) {
    if (args[1] == NULL) {
        int pid = get(bg_pid_list, bg_pid_list->len - 1);
        int status;

        do {
            waitpid(pid, &status, WUNTRACED);
        } while (!WIFEXITED(status) && !WIFSIGNALED(status));

        remove_at(bg_pid_list, bg_pid_list->len - 1);
    } else if (args[2] != NULL) {
        fprintf(stderr, "%s%s", BOLD_RED, "lsh: fg: too many arguments\n");
        return 1;
    } else {
        int pid = atoi(args[1]);
        int pid_list_pos = -1;
        for (int i = 0; i < bg_pid_list->len; ++i) {
            if (pid != bg_pid_list->array[i]) continue;
            pid_list_pos = i;
            break;
        }
        if (pid_list_pos == -1) {
            fprintf(stderr, "%s%s", BOLD_RED, "lsh: fg: no such job\n");
            return 1;
        }
        int status;

        do {
            waitpid(pid, &status, WUNTRACED);
        } while (!WIFEXITED(status) && !WIFSIGNALED(status));

        remove_at(bg_pid_list, pid_list_pos);
    }
    return 0;
}


int lsh_history(char **args) {
    if (args[1] != NULL){
        fprintf(stderr, "%s%s", BOLD_RED, "lsh: history: too many arguments\n");
        return 1;
    }
    for (int i = 0; i < history_length; ++i) {
        fprintf(stdout, "%d: %s", i + 1, history[i]);
    }
    fprintf(stdout, "\n");
    return 0;
}


int lsh_again(char **args) {
    if (args[1] == NULL) {
        fprintf(stderr, "%s%s", BOLD_RED, "lsh: again: not enough arguments\n");
    } else if (args[2] != NULL) {
        fprintf(stderr, "%s%s", BOLD_RED, "lsh: again: too many arguments\n");
    } else if (history_length == 0) {
        fprintf(stderr, "%s%s", BOLD_RED, "lsh: again: no commands in history\n");
    } else {
        fprintf(stderr, "%s%s %d\n", BOLD_RED, "lsh: again: index must be an integer between 1 and", history_length);
    }
    return 1;
}


int lsh_unset(char **args) {
    if (args[1] == NULL) {
        fprintf(stderr, "%s%s", BOLD_RED, "lsh: unset: not enough arguments\n");
        return 1;
    } else if (args[2] != NULL) {
        fprintf(stderr, "%s%s", BOLD_RED, "lsh: unset: too many arguments\n");
        return 1;
    }
    int pos = contains(dict_keys, args[1]);
    if (pos == -1) {
        fprintf(stderr, "%slsh: unset: %s isn't a variable\n", BOLD_RED, args[1]);
        return 1;
    }
    remove_at_g(dict_keys, pos);
    remove_at_g(dict_values, pos);
    return 0;
}


int lsh_get(char **args) {
    if (args[1] == NULL) {
        fprintf(stderr, "%s%s", BOLD_RED, "lsh: get: not enough arguments\n");
        return 1;
    } else if (args[2] != NULL) {
        fprintf(stderr, "%s%s", BOLD_RED, "lsh: get: too many arguments\n");
        return 1;
    }
    int pos = contains(dict_keys, args[1]);
    if (pos == -1) {
        fprintf(stderr, "%slsh: get: %s isn't a variable\n", BOLD_RED, args[1]);
        return 1;
    }
    fprintf(stdout, "%s\n", (char *)dict_values->array[pos]);
    return 0;
}

/** \return Path del archivo history.
 **/
char *lsh_path_history() {
    char *path = (char *) malloc((strlen(getenv("HOME")) + strlen("/.shell_history")));
    strcpy(path, getenv("HOME"));
    strcat(path, "/.shell_history");
    return path;
}


/** Carga history desde su archivo.
 **/
void lsh_load_history() {
    for(int i = 0; i < HISTORY_MAX_SIZE; i++){
        history[i] = malloc(1024 * sizeof(char));
    }
    char *path = lsh_path_history();
    history_length = 0;
    int status = 0;
    FILE *file;
    file = fopen(path, "r");
    int i = 0;

    if(file == NULL){
        free(path);
        return;
    }

    while (status != -1) {
        char *line = NULL;
        size_t buf_size = 0;
        status = (int) getline(&line, &buf_size, file);
        if (status == -1) {
            i--;
            free(line);
            continue;
        }
        if (i == HISTORY_MAX_SIZE) break;
        strcpy(history[i], line);
        history_length++;
        free(line);
        i++;
    }

    fclose(file);
    free(path);
}


/** Guarda line como última posición de history.
 **/
void lsh_save_history(char *line) {
    if (history_length == HISTORY_MAX_SIZE) {
        for (int i = 0; i < HISTORY_MAX_SIZE - 1; ++i) {
            strcpy(history[i], history[i + 1]);
        }
        strcpy(history[HISTORY_MAX_SIZE - 1], line);
    } else {
        history[history_length] = malloc(1024 * sizeof(char));
        strcpy(history[history_length], line);
        history_length++;
    }

    char *path = lsh_path_history();
    int fd = open(path, O_WRONLY | O_CREAT | O_TRUNC, S_IRWXU);
    for (int i = 0; i < history_length; i++) {
        write(fd, history[i], strlen(history[i]));
    }
    write(fd, "\n", 1);
    close(fd);
}


/** Remueve del background los procesos que no entén vivos**/
void lsh_update_background() {
    int status;
    for (int i = 0; i < bg_pid_list->len; ++i) {
        int pid_t = waitpid(bg_pid_list->array[i], &status, WNOHANG);
        if (pid_t <= 0) continue;
        remove_at(bg_pid_list, i);
        i = -1;
    }
}
