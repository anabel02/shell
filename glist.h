//
// Created by anabel on 4/19/2023.
//

#ifndef NSH_GLIST_H
#define NSH_GLIST_H

#include <stdlib.h>

typedef struct GList{
	int len;
	unsigned capacity;
	void** array;
}GList;

struct GList *new_g_list();

void append_g(struct GList *list, void *value);

void *get_g(struct GList *list, int index);

int set_g(struct GList *list, int index, void *value);

int remove_at_g(struct GList *list, int index);

int add_at_g(struct GList *list, int index, void *value);

int contains(struct GList *list, void *value);

#endif //NSH_GLIST_H
