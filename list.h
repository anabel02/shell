//
// Created by anabel on 4/19/2023.
//

#include <stdlib.h>
#include <stdio.h>
#include <values.h>

#ifndef SHELL_LIST_H
#define SHELL_LIST_H

typedef struct List {
    int len;
    unsigned capacity;
    int* array;
} List;

struct List* new_list();

struct List* clone_list(struct List* original);

void append(struct List* list, int value);

int get(struct List* list,int index);

int set(struct List* list,int index, int value);

int remove_at(struct List* list,int index);

int add_at(struct List* list, int index, int value);

#endif //SHELL_LIST_H


