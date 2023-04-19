//
// Created by anabel on 4/1/2023.
//

#include "builtin.h"


char *builtin_str[] = {
        "cd",
        "help",
        "exit",
        "true",
        "false"
};


int (*builtin_func[]) (char **) = {
        &lsh_cd,
        &lsh_help,
        &lsh_exit,
        &lsh_true,
        &lsh_false
};


int lsh_num_builtins() {
    return sizeof(builtin_str) / sizeof(char *);
}


int lsh_cd(char **args) {
    if (args[1] == NULL) {
        char *env = getenv("HOME");
        if (chdir(env) != 0) {
            perror("lsh : cd fhynj");
        }
    } else if (args[2] != NULL){
        fprintf(stderr, "lsh: cd: too many arguments\n");
    } else {
            if (chdir(args[1]) != 0) {
                perror("lsh : cd");
            }
    }
    return 0;
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