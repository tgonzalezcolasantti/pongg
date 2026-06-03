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
    list_entry_t* iterator_current;
    int length;
} list_t;

list_t* create_list(){
    return (list_t*)SDL_calloc(1, sizeof(list_t));
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

void init_iterator(list_t* list){
    if (list){
        list->iterator_current = list->first;
    }
} 

bool has_next(list_t* list){
    if (list)
        return list->iterator_current != NULL;
    return false;
}

void* next(list_t* list){
    if (list){
        if (!list->iterator_current) return NULL;
        void* value = list->iterator_current->value;
        list->iterator_current = list->iterator_current->next;
        return value;
    }
    return NULL;
}