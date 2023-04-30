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
            printf("lsh: syntax error near <\n");
            return 1;
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

    if (strcmp(args[0], "set") == 0) {
        return lsh_execute_set(args);
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
                printf("lsh: syntax error near >\n");
                return 1;
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
                printf("lsh: syntax error near >>\n");
                return 1;
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


/** Si el comando está separado por caracteres de cadena && ; ||
 * ejecuta su respectiva funcionalidad, si no lo está revisa
 * si debe ejecutarse en el background o no y lo ejecuta en dependencia.
  **/
int lsh_execute_chain(char **args) {
    if (args[0] == NULL) {
        return 0;
    }

    int open_conditionals = 0;

    for (int i = 0; args[i] != NULL; ++i) {
        if (strcmp(args[i], "set") == 0) {
            if (args[i + 1] == NULL) break;
            int post_set = set_command_value(args + 2);
            i = post_set != -1 ? post_set + i : i;
            continue;
        }

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
        } else if (strcmp(args[j], "end") == 0) {
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

    if (then_pos == -1 || end_pos == -1 || then_pos > end_pos) {
        printf("lsh: syntax error in if statement");
        return 1;
    }

    if(args[end_pos + 1] != NULL) {
        printf("lsh: syntax error near end\n");
    }

    if (lsh_execute(args + if_pos + 1) == 0) {
        return lsh_execute(args + then_pos + 1);
    } else if (else_pos != -1 && else_pos > then_pos && else_pos < end_pos) {
        return lsh_execute(args + else_pos + 1);
    }

    return 1;
}


int set_command_value(char **args) {
    if (strcmp(args[0], "\'") != 0) {
        return -1;
    }
    int open_sets = 1;
    int simple_quotes = 1;
    for (int i = 1; args[i] != NULL; ++i) {
        if (strcmp(args[i], "set") == 0) {
            open_sets++;
        }
        if (strcmp(args[i], "\'") != 0) continue;
        simple_quotes++;
        if (simple_quotes != open_sets * 2) continue;
        return i;
    }
    return -1;
}


/** Ejecuta un comando con estructura \n
 * set var /<value/>
 * \param args comando que inicia con el keyword set.
  **/
int lsh_execute_set(char **args) {
    if (args[1] == NULL) {
        int len = dict_keys->len;
        for (int i = 0; i < len; ++i) {
            printf("%s = %s\n", (char *)dict_keys->array[i], (char *)dict_values->array[i]);
        }
        return 0;
    }
    if (args[2] == NULL) {
        fprintf(stderr, "lsh: set: syntax error in set statement\n");
        return 1;
    }
    if (strcmp(args[2], "\'") == 0) {
        int post_set = set_command_value(args + 2) + 2;
        if (post_set == -1) {
            fprintf(stderr, "lsh: set: syntax error in set statement, unclosed \'\n");
            return 1;
        }
        args[post_set] = NULL;
        int fd_out = dup(STDOUT_FILENO);
        int fd[2];
        pipe(fd);
        dup2(fd[1], STDOUT_FILENO);
        int status = lsh_execute(args + 3);
        write(fd[1], "\0", 1);
        close(fd[1]);

        fflush(stdout);
        dup2(fd_out, STDOUT_FILENO);
        close(fd_out);
        char *buffer = malloc(1024);
        char c = 1;
        int i = 0;
        while (1) {
            read(fd[0], &c, 1);
            buffer[i] = c;
            if (c == '\0') break;
            if (c == '\n') buffer[i] = ' ';
            i++;
            if (i > 1024) {
                buffer = realloc(buffer, i + 1024);
            }
        }
        close(fd[0]);
        buffer[i] = '\0';
        char *key = malloc(strlen(args[1]));
        char *value = malloc(i);
        strcpy(key, args[1]);
        strcpy(value, buffer);
        append_g(dict_keys, key);
        append_g(dict_values, value);
        free(buffer);
        return status;
    }

    char *key = malloc(strlen(args[1]));
    char *value = malloc(strlen(args[2]));
    strcpy(key, args[1]);
    strcpy(value, args[2]);
    append_g(dict_keys, key);
    append_g(dict_values, value);
    if (args[3] != NULL) {
        fprintf(stderr, "lsh: set: syntax error in set statement\n");
        return 1;
    }
    return 0;
}


int lsh_execute(char** args) {
    return lsh_execute_chain(args);
}
