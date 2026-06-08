#ifndef __LIST_H
#define __LIST_H

typedef struct list_t* list;
typedef struct list_iterator_t* list_iterator;

list create_list();
void free_list(list_t* list);

list append(list list, void* elem);
list remove(list list, void* value);
int length(list list);

bool contains(list list, void* item);
list_iterator init_iterator(list list, unsigned int start_index);
bool has_next(list_iterator iter);
void* next(list_iterator iter);
void iterator_replace(list_iterator iterator, void* value);
void free_iterator(list_iterator iter);

#endif