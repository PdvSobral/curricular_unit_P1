/* Encoding: UTF-8
@Authors: Pedro Sobral (33641), Alexandre Domingos (27641)
@Date: 29/05/2025
@Links: Project github repository -> https://github.com/PdvSobral/curricular_unit_P1

This file contains the functions developed for a simple implementation of linked lists.
For now it does not have much, but the idea is to have simple and double linked lists support.
*/
// Makes so that if "#define __main__" is not somewhere before this program is compiled an error ocurs, stopping compilation
#ifndef __main__
#pragma GCC error "This code is not meant to be compiled directly."
#else
// Makes so that this file is only included once
#pragma once
#endif



// Typedef for a NODE in a LinkedList
typedef struct _node {
    void* data;
    struct _node* next;
} NODE;

// Typedef for a LinkedList
typedef struct _linkedlist {
    NODE* head;
    size_t size;
} LinkedList;



LinkedList* create_linked_list() {
	/*
	Function to create a new linked list
	Arguments:
		None
	Return:
		Pointer to the newly created LinkedList.
		If it returns NULL, the allocation failed.
	*/
    LinkedList* list = (LinkedList*) malloc(sizeof(LinkedList));
    if (list == NULL) {
        return NULL;
    }
    list->head = NULL;
    list->size = 0;
    return list;
}

NODE* create_node(void *data) {
	/*
	Function to create a new node.
	If it returns NULL, the allocation failed.
	Arguments:
		None
	Return:
		Pointer to the newly created node
	*/
    NODE* new_node = (NODE *) malloc(sizeof(NODE));
    if (new_node == NULL) {
        return NULL;
    }
    new_node->data = data;
    new_node->next = NULL;
    return new_node;
}

NODE* add_node(LinkedList* list, void* data) {
	/*
	Function to create a new node at the end of a linked list
	Arguments:
		LinkedList* list	-> Linked List to add the node to
		void* data			-> Data to be contained in the node
	Return:
		Pointer to the newly created Node
	*/
    NODE *new_node = create_node(data);
    if (list->head == NULL) {
        list->head = new_node;			// If the list is empty, set the new node as the head
    } else {
        NODE *current = list->head;
        while (current->next != NULL) {
            current = current->next;	// Search for the end
        }
        current->next = new_node;		// Add the new node at the end
    }
    list->size++;
    return new_node;
}

void delete_linked_list(LinkedList* list) {
	/*
	Function to delete and free the space of a linked list
	Arguments:
		LinkedList* list	-> Linked List to delete
	Return:
		None
	*/
    NODE *current = list->head;
    NODE *next_node;
    while (current != NULL) {
        next_node = current->next;
        free(current);
        current = next_node;
    }
    free(list);
    return;
}

void traverse_list(LinkedList* list, void (*func)(void*)) {
	/*
	Function to aply a function to every node.
	It will probably never be used, but whatever.
	Arguments:
		LinkedList* list	-> Linked List to delete
	Return:
		None
	*/
    NODE *current_node = list->head;
    while (current_node != NULL) {
        func(current_node);				// Apply the function to the node
        current = current_node->next;
    }
}

void remove_nodes(LinkedList* list, uint8_t (*contition)(void*)) {
	/*
	Function to remove every node that meets a certain criteria.
	Arguments:
		LinkedList* list			-> Linked list to delete the nodes from.
		uint8_t (*contition)(void*)	-> Function to be used to check if the node should be deleted.
									   If the return is 1, then the node in cause is deleted.
	Return:
		None
	*/
    if (list->head == NULL) return;
    NODE *current = list->head;
    NODE *previous = NULL;
    while (current != NULL) {
        if (contition(current->data) == 1) { // Compare data using the provided function
        	// It's the first node, so simply reassign the head node
            if (previous == NULL) {
            	list->head = current->next;
            	free(current);
            	current = list->head;
            }
            // If not head, skip the current node in the list
            else {
            	previous->next = current->next;
            	free(current);
            	current = previous->next;
            }
            list->size--;	// decrease size by one
        } else {
			previous = current;
			current = previous->next;
        }
    }
    return;
}
