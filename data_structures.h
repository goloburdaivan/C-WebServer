#include<stddef.h>
#include<stdlib.h>
#include<string.h>
#include<stdio.h>
#include "structures.h"

#ifndef DATA_STRUCTURES_H
#define DATA_STRUCTURES_H


#define MAP_CAPACITY 100

/*
* Linked list used to resolve collisions in map.
* struct node - represents the node of the linked list and stores key-value pair as a string
* Author: flaksie
*/

struct node {
    struct node* next;
    char key[255];
    char data[1024];
};

void free_linked_list(struct node** head);

/*
    * Map data structure
    * Stores data in key-value pairs
    * Needs to store http headers from browser request
    * Author: flaksie
*/

struct map {
    struct node* table[MAP_CAPACITY];
};

size_t hash(char* buffer);
void init_map(struct map* map);
void map_put(struct map* map, char* key, char* data);
char* map_get(struct map* map, char* key);
void free_map(struct map* map);

/*
    * Handlers list
    * Storing the handlers and needed to find the right handler to process the request
    * Built using linked list data structure.
    * Author: flaksie
*/
struct handlers {
    struct handlers* next;
    char path[255];
    void (*controller)(int, struct http_request*);
};

#endif