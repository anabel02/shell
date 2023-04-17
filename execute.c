//
// Created by anabel on 4/16/2023.
//

#include <fcntl.h>
#include "execute.h"

void lsh_print_args(char** args) {
    for (int i = 0; args[i] != NULL; ++i) {
        printf("%s ", args[i]);
    }
    printf("\n");
}


int length(char **args) {
    int i;
    for (i = 0; args[i]!= NULL; ++i) {}
    return i + 1;
}


int lsh_launch(char **args, int fd_in, int fd_out) {
    for (int i = 0; i < lsh_num_builtins(); i++) {
        if (strcmp(args[0], builtin_str[i]) == 0) {
            (*builtin_func[i])(args);
        }
    }

    pid_t pid;
    int status = 0;

    pid = fork();
    if (pid == 0) {
        if (fd_in > 0) {
            dup2(fd_in, STDIN_FILENO);
            close(fd_in);
        }

        if (fd_out > 0) {
            dup2(fd_out, STDOUT_FILENO);
            close(fd_out);
        }

        if (execvp(args[0], args) == -1) {
            perror("lsh");
        }
        exit(EXIT_FAILURE);
    } else if (pid < 0) {
        perror("lsh");
    } else {

        if (fd_in > 0) {
            close(fd_in);
        }
        if (fd_out > 0) {
            close(fd_out);
        }
        do {
            waitpid(pid, &status, WUNTRACED);
        } while (!WIFEXITED(status) && !WIFSIGNALED(status));

    }

    return status;
}


int lsh_execute_simple(char **args, int fd_in, int fd_out, int start, int end)
{
    char *new_args[end - start + 1];
    for (int i = start; i < end; ++i) {
        new_args[i - start] = args[i];
    }
    new_args[end] = NULL;

    if (new_args[0] == NULL) {
        // An empty command was entered.
        return 0;
    }

    for (int i = 0; i < lsh_num_builtins(); i++) {
        if (strcmp(args[0], builtin_str[i]) == 0) {
            return (*builtin_func[i])(new_args);
        }
    }

    return lsh_launch(new_args, fd_in, fd_out);
}


int execute_redirections(char **args, int fd_in, int fd_out, int start, int end) {
    if (args[0] == NULL) {
        return 0;
    }

    int changes = 0;

    for (int i = 0; args[i] != NULL; ++i) {
        if (strcmp(args[i], "|") == 0) {
            changes ++;
            int fd[2];
            pipe(fd);
            int pid = fork();
            if (pid < 0) {
                perror("lsh");
            } else if (pid == 0) {
                close(fd[0]);
                lsh_execute_simple(args, fd_in, fd[1], start, i);
                close(fd[1]);
            } else {
                close(fd[1]);
                wait(NULL);
                if(i + 1 < length(args) - 1) {
                    execute_redirections(args, fd[0], fd_out, i + 1, end);
                }
                close(fd[0]);
            }
            return 0;
        } else if (strcmp(args[i], ">") == 0) {
            changes++;
            int fd = open(args[i + 1], O_WRONLY | O_CREAT | O_TRUNC, 0644);
            if (fd == -1) {
                perror("lsh");
                return 1;
            }
            lsh_execute_simple(args, fd_in, fd, start, i);
            if(i + 2 < length(args) - 1) {
                execute_redirections(args, fd_in, fd_out, i + 2, end);
            }
            close(fd);
            return 0;
        } else if (strcmp(args[i], ">>") == 0) {
            changes++;
            int fd = open(args[i + 1], O_WRONLY | O_CREAT | O_APPEND, 0644);
            if (fd == -1) {
                perror("lsh");
                return 1;
            }
            lsh_execute_simple(args, fd_in, fd, start, i);
            if(i + 2 < length(args) - 1) {
                execute_redirections(args, fd_in, fd_out, i + 2, end);
            }
            close(fd);
            return 0;
        } else if (strcmp(args[i], "<") == 0) {
            changes++;
            int fd = open(args[i + 1], O_RDONLY);
            if (fd == -1) {
                perror("lsh");
                return 1;
            }
            lsh_execute_simple(args, fd, fd_out, start, i);
            if(i + 2 < length(args) - 1) {
                execute_redirections(args, fd_in, fd_out, i + 2, end);
            }
            return 0;
        }
    }
    if (changes == 0) {
        return lsh_execute_simple(args, fd_in, fd_out, start, end);
    }
    return 0;
}


int lsh_execute(char** args) {
    return execute_redirections(args, -1, -1, 0, length(args));
}
