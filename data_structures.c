#include "data_structures.h"

void free_linked_list(struct node** head) {
    struct node* current = *head;
    struct node* temp = NULL;
    while (current != NULL) {
        temp = current;
        current = current->next;
        free(temp);
    }
}

size_t hash(char* buffer) {
    size_t hash = 0;
    for (size_t i = 0; i < strlen(buffer); i++) {
        hash += buffer[i];
    }

    return hash % MAP_CAPACITY;
}

void init_map(struct map* map) {
    for (size_t i = 0; i < MAP_CAPACITY; i++) {
        map->table[i] = NULL;
    }
}

void map_put(struct map* map, char* key, char* data) {
    size_t index = hash(key);

    struct node** current = &map->table[index];
    if (*current == NULL) {
        *current = (struct node*) malloc(sizeof(struct node));
        (*current)->next = NULL;
        strcpy((*current)->data, data);
        strcpy((*current)->key, key);
        return;
    }

    while ((*current)->next != NULL) {
        (*current) = (*current)->next;
    }

    (*current)->next = (struct node*) malloc(sizeof(struct node));
    strcpy((*current)->next->data, data);
    strcpy((*current)->next->key, key);
    (*current)->next->next = NULL;
}

char* map_get(struct map* map, char* key) {
    size_t index = hash(key);

    struct node* current = map->table[index];
    if (current == NULL) return NULL;
    if (strcmp(current->key, key) == 0) {
        return current->data;
    }

    while (current != NULL) {
        if (strcmp(current->key, key) == 0) {
            return current->data;
        }
        current = current->next;
    }

    return NULL;
}

void free_map(struct map* map) {
    for (size_t i = 0; i < MAP_CAPACITY; i++) {
        if (map->table[i] != NULL) {
            free_linked_list(&map->table[i]);
        }
    }
}