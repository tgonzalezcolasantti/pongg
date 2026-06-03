#ifndef __LIST_H
#define __LIST_H

typedef struct list_t* list_adt;

list_adt create_list();

list_adt append(list_adt list, void* elem);

list_adt remove(list_adt list, void* value);

int length(list_adt list);

void init_iterator(list_adt list);

bool has_next(list_adt list);

void* next(list_adt list);

#endif