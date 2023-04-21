//
// Created by anabel on 4/16/2023.
//

#include "execute.h"


void print_args(char** args) {
    for (int i = 0; args[i] != NULL; ++i) {
        printf("%s ", args[i]);
    }
    printf("\n");
}


int len(char **array) {
    int i;
    for (i = 0; array[i] != NULL; ++i) {}
    return i;
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

        for (int i = 0; i < lsh_num_builtins_out(); i++) {
            if (strcmp(args[0], builtin_str_out[i]) == 0) {
                exit((*builtin_func_out[i])(args));
            }
        }

        if (execvp(args[0], args) == -1) {
            perror("lsh");
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
    if (args[0] == NULL) {
        return 0;
    }

    for (int i = 0; i < lsh_num_builtins(); i++) {
        if (strcmp(args[0], builtin_str[i]) == 0) {
            return (*builtin_func[i])(args);
        }
    }

    return lsh_launch(args, fd_in, fd_out);
}


int lsh_execute_redirections_in(char **args, int fd_in, int fd_out) {
    if (args[0] == NULL) {
        return 0;
    }

    for (int i = 0; args[i] != NULL; ++i) {
        if (strcmp(args[i], "<") != 0) continue;
        args[i] = NULL;
        int fd = open(args[i + 1], O_RDONLY);
        if (fd == -1) {
            perror("lsh");
            return 1;
        }
        int exit_status = lsh_execute_simple(args, fd, fd_out);
        if(args[i + 2] != NULL) {
            exit_status += lsh_execute_simple(args + i + 2, fd_in, fd_out);
        }
        close(fd);
        return exit_status;
    }
    return lsh_execute_simple(args, fd_in, fd_out);
}


int lsh_execute_redirections_out(char **args, int fd_in, int fd_out) {
    if (args[0] == NULL) {
        return 0;
    }

    if (strcmp(args[0], "if") == 0) {
        return lsh_execute_conditional(args);
    }

    for (int i = 0; args[i] != NULL; ++i) {
        if (strcmp(args[i], "|") == 0) {
            args[i] = NULL;
            int fd[2];
            if (pipe(fd) == -1) {
                perror("lsh");
                return 1;
            }
            int exit_status = lsh_execute_redirections_in(args, fd_in, fd[1]);
            if (args[i + 1] != NULL) {
                exit_status += lsh_execute_redirections_out(args + i + 1, fd[0], fd_out);
            }
            close(fd[0]);
            close(fd[1]);
            return exit_status;
        } else if (strcmp(args[i], ">") == 0) {
            args[i] = NULL;
            int fd = open(args[i + 1], O_WRONLY | O_CREAT | O_TRUNC, 0644);
            if (fd == -1) {
                perror("lsh");
                return 1;
            }
            int exit_status = lsh_execute_redirections_in(args, fd_in, fd);
            close(fd);
            if(args[i + 2] != NULL) {
                exit_status += lsh_execute_redirections_out(args + i + 2, fd_in, fd_out);
            }
            return exit_status;
        } else if (strcmp(args[i], ">>") == 0) {
            args[i] = NULL;
            int fd = open(args[i + 1], O_WRONLY | O_CREAT | O_APPEND, 0644);
            if (fd == -1) {
                perror("lsh");
                return 1;
            }

            int exit_status = lsh_execute_redirections_in(args, fd_in, fd);
            close(fd);
            if(args[i + 2] != NULL) {
                exit_status += lsh_execute_redirections_out(args + i + 2, fd_in, fd_out);
            }
            return exit_status;
        }
    }
    return lsh_execute_redirections_in(args, fd_in, fd_out);
}


/** Ejecuta un proceso en el background \n
 * args: comando que termina con el operador &
  **/
int lsh_background(char **args) {
    args[len(args) - 1] = NULL;
    pid_t pid = fork();
    if (pid < 0) {
        perror("lsh");
    } else if (pid == 0) {
        setpgid(0, 0);
        lsh_execute_redirections_out(args, -1, -1);
        exit(EXIT_FAILURE);
    } else {
        setpgid(pid, pid);
        append(bg_pid_list, pid);
        printf("[%d]\t%d\n", bg_pid_list->len, pid);
        return 0;
    }
    return 1;
}

/** Si el comando está separado por caracteres de cadena && ;
 * ejecuta su respectiva funcionalidad, si no lo está revisa
 * si debe ejecutarse en el background o no y lo ejecuta en dependencia.
  **/
int lsh_execute_chain(char **args) {
    if (args[0] == NULL) {
        return 0;
    }

    int open_conditionals = 0;

    for (int i = 0; args[i] != NULL; ++i) {
        if (strcmp(args[i], "if") == 0) {
            open_conditionals++;
        }
        if (strcmp(args[i], "end") == 0) {
            open_conditionals--;
        }
        if (open_conditionals > 0) continue;
        if (strcmp(args[i], ";") == 0) {
            args[i] = NULL;
            return lsh_execute_redirections_out(args, -1, -1) + lsh_execute_chain(args + i + 1);
        } else if (strcmp(args[i], "&&") == 0) {
            args[i] = NULL;
            if (lsh_execute_redirections_out(args, -1, -1) == 0) {
                return lsh_execute_chain(args + i + 1);
            }
            return 1;
        } else if (strcmp(args[i], "||") == 0) {
            args[i] = NULL;
            if (lsh_execute_redirections_out(args, -1, -1) != 0) {
                return lsh_execute_chain(args + i + 1);
            }
            return 0;
        }
    }

    return strcmp(args[len(args) - 1], "&") == 0 ?
        lsh_background(args) :
        lsh_execute_redirections_out(args, -1, -1);
}

/** Ejecuta una condicional con estructura \n
 * if \<command1\> then \<command2\> else \<command3\> end.
    \param
    args: comando que inicia con el keyword if.
  **/
int lsh_execute_conditional(char **args) {
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

    if(args[end_pos + 1] != NULL) {
        printf("lsh: después del end debe aparecer algun caracter de cadena\n");
    }

    if (then_pos != -1 && end_pos != -1) {
        if (lsh_execute_chain(args + if_pos + 1) == 0) {
            return lsh_execute_chain(args + then_pos + 1);
        } else if (else_pos != -1) {
            return lsh_execute_chain(args + else_pos + 1);
        }
    }

    return 1;
}


int lsh_execute(char** args) {
    return lsh_execute_chain(args);
}
