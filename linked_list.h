#ifndef LINKED_LIST_H
#define LINKED_LIST_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>

#include "memory_manager.h"


// Node structure for the linked list
typedef struct Node {
    uint16_t data;
    struct Node* next;
} Node;

// Function to initialize the linked list
void list_init(Node** head, size_t size);

// Function to insert a node at the end of the listclea
void list_insert(Node** head, uint16_t data);

// Function to insert a node after a given node
void list_insert_after(Node* prev_node, uint16_t data);

// Function to insert a node before a given node
void list_insert_before(Node** head, Node* next_node, uint16_t data);

// Function to delete a node with the specified data
void list_delete(Node** head, uint16_t data);

// Function to search for a node with the specified data
Node* list_search(Node** head, uint16_t data);

// Function to display all elements in the list
void list_display(Node** head);

// Function to display elements between two nodes
void list_display_range(Node** head, Node* start_node, Node* end_node);

// Function to count the number of nodes in the list
int list_count_nodes(Node** head);

// Function to clean up the list and free all nodes
void list_cleanup(Node** head);

#endif // LINKED_LIST_H