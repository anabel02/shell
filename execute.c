//
// Created by anabel on 4/16/2023.
//

#include <fcntl.h>
#include "execute.h"

/* if ls ; if ls ; ls then ls end ls then ls else cd .. end */


void lsh_print_args(char** args) {
    for (int i = 0; args[i] != NULL; ++i) {
        printf("%s ", args[i]);
    }
    printf("\n");
}


int lsh_launch(char **args, int fd_in, int fd_out) {
    pid_t pid;
    int status = 0;

    pid = fork();

    if (pid < 0) {
        perror("lsh");
    } else if (pid == 0) {
        if (fd_in > 0) {
            dup2(fd_in, STDIN_FILENO);
            close(fd_in);
        }

        if (fd_out > 0) {
            dup2(fd_out, STDOUT_FILENO);
            close(fd_out);
        }

        if (execvp(args[0], args) == -1) {
            perror("lsh execute");
        }
        exit(EXIT_FAILURE);
    } else {
        do {
            if (fd_in > 0) {
                close(fd_in);
            }

            if (fd_out > 0) {
                close(fd_out);
            }
            waitpid(pid, &status, WUNTRACED);
        } while (!WIFEXITED(status) && !WIFSIGNALED(status));

    }

    return status;
}


int lsh_execute_simple(char **args, int fd_in, int fd_out)
{
    for (int i = 0; i < lsh_num_builtins(); i++) {
        if (strcmp(args[0], builtin_str[i]) == 0) {
            return (*builtin_func[i])(args);
        }
    }
    return lsh_launch(args, fd_in, fd_out);
}

int execute_redirections(char **args, int fd_in, int fd_out) {
    if (args[0] == NULL) {
        return 0;
    }

    int changes = 0;
    for (int i = 0; args[i] != NULL; ++i) {
        if (strcmp(args[i], "|") == 0) {
            args[i] = NULL;
            changes ++;
            int fd[2];
            pipe(fd);
            lsh_execute_simple(args, fd_in, fd[1]);
            if (args[i + 1] != NULL) {
                execute_redirections(args + i + 1, fd[0], fd_out);
            }
            close(fd[0]);
            close(fd[1]);
            return 0;
        } else if (strcmp(args[i], ">") == 0) {
            args[i] = NULL;
            changes++;
            int fd = open(args[i + 1], O_WRONLY | O_CREAT | O_TRUNC, 0644);
            if (fd == -1) {
                perror("lsh");
                return 1;
            }
            lsh_execute_simple(args, fd_in, fd);
            if(args[i + 2] != NULL) {
                execute_redirections(args + i + 2, fd_in, fd_out);
            }
            close(fd);
            return 0;
        } else if (strcmp(args[i], ">>") == 0) {
            args[i] = NULL;
            changes++;
            int fd = open(args[i + 1], O_WRONLY | O_CREAT | O_APPEND, 0644);
            if (fd == -1) {
                perror("lsh");
                return 1;
            }
            lsh_execute_simple(args, fd_in, fd);
            if(args[i + 2] != NULL) {
                execute_redirections(args + i + 2, fd_in, fd_out);
            }
            close(fd);
            return 0;
        } else if (strcmp(args[i], "<") == 0) {
            args[i] = NULL;
            changes++;
            int fd = open(args[i + 1], O_RDONLY);
            if (fd == -1) {
                perror("lsh");
                return 1;
            }
            lsh_execute_simple(args, fd, fd_out);
            if(args[i + 2] != NULL) {
                execute_redirections(args + i + 2, fd_in, fd_out);
            }
            close(fd);
            return 0;
        }
    }
    return changes == 0 ? lsh_execute_simple(args, fd_in, fd_out) : 0;
}

int conditional(char **args, int fd_in, int fd_out);

int execute_chain(char **args, int fd_in, int fd_out) {
    if (args[0] == NULL) {
        return 0;
    }

    int changes = 0;
    int open_conditionals = 0;
    int cond = 0;

    for (int i = 0; args[i] != NULL; ++i) {
        if (strcmp(args[i], "if") == 0) {
            changes++;
            if (i != 0) {
                printf("lsh: error parsing, must be a chain character before if block\n");
                return 1;
            }
            int end_pos = conditional(args + i, fd_in, fd_out);
            if (end_pos == -1) {
                perror("lsh : if without end");
                return 1;
            } else {
                i = end_pos;
                if (args[i+1] == NULL || strcmp(args[i + 1], ";") == 0 || strcmp(args[i + 1], "&&") == 0 || strcmp(args[i + 1], "||") == 0 ) {
                    continue;
                }
                printf("lsh: error parsing, must be a chain character after if block\n");
                return 1;
            }
        }

        if (strcmp(args[i], ";") == 0) {
            args[i] = NULL;
            changes++;
            execute_redirections(args, fd_in, fd_out);
            execute_chain(args + i + 1, fd_in, fd_out);
            return 0;
        } else if (strcmp(args[i], "&&") == 0) {
            args[i] = NULL;
            changes++;
            if(execute_redirections(args, fd_in, fd_out) == 0) {
                execute_chain(args + i + 1, fd_in, fd_out);
            }
            return 0;
        } else if (strcmp(args[i], "||") == 0) {
            args[i] = NULL;
            changes++;
            if(execute_redirections(args, fd_in, fd_out) != 0) {
                execute_chain(args + i + 1, fd_in, fd_out);
            }
            return 0;
        }
    }
    return changes == 0 ? execute_redirections(args, fd_in, fd_out) : 0;
}

int conditional(char **args, int fd_in, int fd_out) {

    if (strcmp(args[0], "if") != 0) {
        return -1;
    }

    int if_pos = 0;
    args[0] = NULL;
    int then_pos = -1;
    int else_pos = -1;
    int end_pos = -1;

    int conditionals = 1;
    for (int j = 1; args[j] != NULL; ++j) {
        if (strcmp(args[j], "if") == 0) {
            conditionals++;
        }
        if (strcmp(args[j], "end") == 0) {
            conditionals--;
            if (conditionals != 0) continue;
            end_pos = j;
            args[j] = NULL;
            break;
        }
        if (conditionals != 1) continue;
        if (strcmp(args[j], "then") == 0) {
            then_pos = j;
            args[j] = NULL;
        } else if (strcmp(args[j], "else") == 0) {
            else_pos = j;
            args[j] = NULL;
        }
    }

    if (then_pos != -1 && end_pos != -1) {
        if (execute_chain(args + if_pos + 1, fd_in, fd_out) == 0) {
            execute_chain(args + then_pos + 1, fd_in, fd_out);
        } else if (else_pos != -1) {
            execute_chain(args + else_pos + 1, fd_in, fd_out);
        }
    }

    return end_pos;
}

int lsh_execute(char** args) {
    return execute_chain(args, -1, -1);
}
