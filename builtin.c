//
// Created by anabel on 4/1/2023.
//

#include "builtin.h"


char *builtin_str[] = {
        "cd",
        "help",
        "exit",
        "true",
        "false",
        "jobs"
};


int (*builtin_func[]) (char **) = {
        &lsh_cd,
        &lsh_help,
        &lsh_exit,
        &lsh_true,
        &lsh_false,
        &jobs
};


int lsh_num_builtins() {
    return sizeof(builtin_str) / sizeof(char *);
}


int lsh_cd(char **args) {
    if (args[1] == NULL) {
        if (chdir(getenv("HOME")) == 0) return 0;
        perror("lsh: cd: ");

    } else if (args[2] != NULL){
        fprintf(stderr, "lsh: cd: too many arguments\n");
    } else {
        if (chdir(args[1]) == 0) return 0;
        perror("lsh : cd");
    }
    return 1;
}


int lsh_help(char **args) {
    int i;

    for (i = 0; i < lsh_num_builtins(); i++) {
        printf("  %s\n", builtin_str[i]);
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


int jobs(char **args) {
    if (args[1] != NULL) {
        fprintf(stderr, "lsh: jobs: too many arguments\n");
        return 1;
    } else {
        for (int i = 0; i < bg_pid_list->len; ++i) {
            printf("[%d] %d\n", i + 1, get(bg_pid_list, i));
        }
    }
    return 0;
}

void update_background() {
    int status;
    for (int i = 0; i < bg_pid_list->len; ++i) {
        int pid_t = waitpid(bg_pid_list->array[i], &status, WNOHANG);
        if (pid_t <= 0) continue;
        printf("[%d]\tDone\t%d\n", i + 1, bg_pid_list->array[i]);
        remove_at(bg_pid_list, i);
        i = -1;
    }
}
