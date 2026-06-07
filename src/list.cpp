// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
using namespace std;

#include <iostream>
#include <stdbool.h>
#include <list.h>
#include <SDL.h>

typedef struct list_entry_t{
    void* value;
    list_entry_t* next;
} list_entry_t;

typedef struct list_t{
    list_entry_t* first;
    int length;
} list_t;

typedef struct list_iterator_t{
    list_entry_t* current;
    list_entry_t* next;
} list_iterator_t;

list_t* create_list(){
    return (list_t*)SDL_calloc(1, sizeof(list_t));
}

void free_list(list_t* list){
    if (list){
        while(list->length){
            remove(list, list->first->value);
        }
        SDL_free(list);
    }
}

list_entry_t* create_entry(){
    return (list_entry_t*)SDL_calloc(1, sizeof(list_entry_t));
}

list_entry_t* get_last_entry(list_t* list){
    if (list){
        if (list->first == NULL) return NULL;
        list_entry_t* entry = list->first;
        while(entry->next != NULL){
            entry = entry->next;
        }
        return entry;
    }
    return NULL;
}

list_t* append(list_t* list, void* elem){
    if (list){
        list_entry_t* lastentry = get_last_entry(list);
        if (lastentry){
            lastentry->next = create_entry();
            lastentry->next->value = elem;
            list->length++;
        } else {
            list->first = create_entry();
            list->first->value = elem;
            list->length++;
        }
    }
    return list;
}

list_t* remove(list_t* list, void* value){
    if (list){
        if (list->first){
            list_entry_t* entry = list->first;
            if (entry->value == value){
                list->first = entry->next;
                SDL_free(entry);
                list->length--;
                return list;
            }
            while(entry->next){
                if (entry->next->value == value){
                    list_entry_t* temp = entry->next;
                    entry->next = temp->next;
                    SDL_free(temp);
                    list->length--;
                }
            }
        }
    }
    return list;
}

int length(list_t* list){
    return list->length;
}

list_iterator_t* init_iterator(list_t* list, unsigned int start_index){
    if (list){
        list_iterator_t* iterator = (list_iterator_t*)SDL_malloc(sizeof(list_iterator_t));
        iterator->current = list->first;
        while(start_index && iterator->current){
            start_index--;
            iterator->current = iterator->current->next;
        }
        iterator->next=iterator->current;
        if (start_index){
            SDL_free(iterator);
            return NULL;
        }
        return iterator;
    }
    return NULL;
} 

bool has_next(list_iterator_t* iterator){
    if (iterator)
        return iterator->next != NULL;
    return false;
}

void* next(list_iterator_t* iterator){
    if (iterator && has_next(iterator)){
        void* value = iterator->next->value;
        iterator->current = iterator->next;
        iterator->next = iterator->next->next;
        return value;
    }
    return NULL;
}

void iterator_replace(list_iterator_t* iterator, void* value){
    if (iterator){
        iterator->current->value = value;
    }
}

void free_iterator(list_iterator_t* iterator){
    SDL_free(iterator);
}