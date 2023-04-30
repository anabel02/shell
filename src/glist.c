//
// Created by anabel on 4/19/2023.
//

#include "glist.h"

struct GList *new_g_list() {
	struct GList *list = (struct GList *) malloc(sizeof(struct GList));
	list->capacity = 32;
	list->len = 0;
	list->array = (void **) malloc(list->capacity * sizeof(void *));
	return list;
}

void append_g(struct GList *list, void *value) {
	if (list->len == list->capacity) {
		list->capacity *= 2;
		list->array = realloc(list->array, list->capacity * sizeof(void *));
	}
	list->array[list->len++] = value;
}

void *get_g(struct GList *list, int index) {
	if (index >= list->len || index < 0) {
		return NULL;
	}
	return list->array[index];
}

int set_g(struct GList *list, int index, void *value) {
	if (index >= list->len || index < 0) {
		return -1;
	}
	list->array[index] = value;
	return 0;
}

int remove_at_g(struct GList *list, int index) {
	if (index >= list->len || index < 0) {
		return -1;
	}
	for (int i = index; i < list->len - 1; ++i) {
		list->array[i] = list->array[i + 1];
	}
	list->len--;
	return 0;
}

int add_at_g(struct GList *list, int index, void *value) {
	if (index >= list->len || index < 0) {
		return -1;
	}
	if (list->len == list->capacity) {
		list->capacity *= 2;
		list->array = realloc(list->array, list->capacity * sizeof(int));
	}
	for (int i = list->len + 1; i > index; --i) {
		list->array[i] = list->array[i - 1];
	}
	list->array[index] = value;
	list->len++;
	return 0;
}

int contains(struct GList *list, void *value) {
    for (int i = 0; i < list->len; ++i) {
        if (strcmp(list->array[i], value) != 0) continue;
        return i;
    }
    return -1;
}