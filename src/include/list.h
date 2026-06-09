#ifndef __LIST_H
#define __LIST_H

typedef struct list_t* list;
typedef struct list_iterator_t* list_iterator;

list create_list();
void free_list(list_t* list);

list list_append(list list, void* elem);
list list_remove(list list, void* value);
int list_length(list list);

bool contains(list list, void* item);
list_iterator init_iterator(list list, unsigned int start_index);
bool iterator_has_next(list_iterator iter);
void* iterator_next(list_iterator iter);
void iterator_replace(list_iterator iterator, void* value);
void iterator_remove(list_iterator interator);
void free_iterator(list_iterator iter);

#endif