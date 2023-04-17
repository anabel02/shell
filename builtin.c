//
// Created by anabel on 4/1/2023.
//

#include "builtin.h"


char *builtin_str[] = {
        "cd",
        "help",
        "exit"
};


int (*builtin_func[]) (char **) = {
        &lsh_cd,
        &lsh_help,
        &lsh_exit
};


int lsh_num_builtins() {
    return sizeof(builtin_str) / sizeof(char *);
}


int lsh_cd(char **args) {
    if (args[1] == NULL) {
        fprintf(stderr, "lsh: expected argument to \"cd\"\n");
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