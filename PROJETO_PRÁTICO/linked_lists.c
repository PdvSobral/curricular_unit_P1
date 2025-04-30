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
        func(current_node);					// Apply the function to the node
        current_node = current_node->next;
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
    	// Compare data using the provided function
        if (contition(current->data) == 1) {
        	// It's the first node, so simply reassign the head node
            if (previous == NULL) {
            	list->head = current->next;
            	free(current);
            	current = list->head;
            } else {
            	// If not head, skip the current node in the list
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

uint8_t insert_at_index(LinkedList* list, void* data, size_t index) {
	/*
	Function to insert a new node at a certain index.
	Arguments:
		LinkedList* list	-> Linked list to add the nodes to.
		void* data			-> Data for the node
		size_t index		-> Index to insert on
	Return:
		uint8_t 			-> Status: 0 all right, 1: index out of bounds, 2: memory allocation for node failed
	*/
    if (index > list->size) return 1;
    NODE* new_node = create_node(data);
    if (new_node == NULL) return 2;
    if (index == 0) {
        new_node->next = list->head;
        list->head = new_node;
    } else {
        NODE* current = list->head;
        for (size_t i = 0; i < index - 1; i++) current = current->next;
        new_node->next = current->next;
        current->next = new_node;
    }
    list->size++;
    return 0;
}

NODE* get_node_at_index(LinkedList* list, size_t index) {
	/*
	Function to insert a new node at a certain index.
	Arguments:
		LinkedList* list	-> Linked list to get the node from.
		size_t index		-> Index to get node from
	Return:
		NODE* 				-> Address for the node, NULL id index does not exist.
	*/
    if (index >= list->size) return NULL; // Index out of bounds
    NODE* current = list->head;
    for (size_t i = 0; i < index; i++) current = current->next;
    return current;
}

uint8_t remove_node_at_index(LinkedList* list, size_t index) {
	/*
	Function to insert a new node at a certain index.
	Arguments:
		LinkedList* list	-> Linked list to delete the node from.
		size_t index		-> Index to delete node on
	Return:
		uint8_t 				-> Exit status. 0: All right, 1: Index out of bounds.
	*/
    if (index >= list->size) return 1;
    NODE* current = list->head;
    if (index == 0) {
        list->head = current->next;
        free(current);
    } else {
        NODE* previous = NULL;
        for (size_t i = 0; i < index; i++) {
            previous = current;
            current = current->next;
        }
        previous->next = current->next;
        free(current);
    }
    list->size--;
    return 0;
}

int8_t find_node(LinkedList* list, uint8_t (*check)(void*)) {
	/*
	Function to find the first node that meets a certain criteria.
	Arguments:
		LinkedList* list			-> Linked list to search on.
		uint8_t (*contition)(void*)	-> Function to be used to check if the node is the wanted one.
									   If the return is 1, then the index for the node in cause is returned.
	Return:
		int8_t						-> If positive, index for the node found with the lowest index.
									   Else, no index found
	*/
    NODE* current = list->head;
    int8_t index = 0;
    while (current != NULL) {
        if (check(current) == 1) return index;
        current = current->next;
        index++;
    }
    return -1;
}

int8_t find_node_from(LinkedList* list, uint8_t (*check)(void*), uint8_t from_index) {
	/*
	Function to find the first node that meets a certain criteria, but only from a certain address.
	Arguments:
		LinkedList* list			-> Linked list to search on.
		uint8_t (*contition)(void*)	-> Function to be used to check if the node is the wanted one.
									   If the return is 1, then the index for the node in cause is returned.
		uint8_t from_index			-> Only search from that index onwards, index included
	Return:
		int8_t						-> If positive, index for the node found with the lowest index.
									   Else, no index found
	*/
    NODE* current = list->head;
    int8_t index = 0;
    while (current != NULL) {
        if ((check(current) == 1)&&(index>=from_index)) return index;
        current = current->next;
        index++;
    }
    return -1;
}

uint8_t count_occurences(LinkedList* list, uint8_t (*check)(void*)) {
	/*
	Function to count the numbers of nodes that meet a certain criteria.
	Arguments:
		LinkedList* list			-> Linked list to search on.
		uint8_t (*contition)(void*)	-> Function to be used to check if the node is the wanted one.
									   If the return is 1, then the node is considered found.
	Return:
		uint8_t						-> Number of ocurrences found in the list
	*/
    NODE* current = list->head;
    int8_t count = 0;
    while (current != NULL) {
        if (check(current) == 1) count++;
        current = current->next;
    }
    return count;
}

void sort_list(LinkedList* list, uint8_t (*compare)(void*, void*)) {
    /*
    Sorts a LinkedList using an optimized bubble sort.
    If the return of compare is 1, then the first argument is passed to the right
    */
    if (list->head == NULL || list->head->next == NULL) return;
    uint8_t swapped;
    NODE* first_correct_element = NULL;
    do {
        swapped = 0;
        NODE* current = list->head;
        while (current->next != first_correct_element) {
            if (compare(current, current->next) == 1) {
                void* temp = current->data;
                current->data = current->next->data;
                current->next->data = temp;
                swapped = 1;
            }
            current = current->next;
        }
        first_correct_element = current; // Update the end to the last sorted node
    } while (swapped);
}


// TODO: CHECK THE LAST FUNCTION

void three_way_partition(LinkedList* list, LinkedList* less, LinkedList* equal, LinkedList* greater, int32_t (*compare)(void*, void*)) {
    /*
    Used in three-way quick sort. If compare is less than 0, left. If 0, middle. Else right.
    */
    if (list->head == NULL) return;

    NODE* current = list->head;
    void* pivot = current->data;

    while (current != NULL) {
        if (compare(current->data, pivot) < 0) {
            // Add to less
            NODE* next = current->next;
            add_node(less, current->data); // Use add_node to add to the less list
            current = next;
        } else if (compare(current->data, pivot) == 0) {
            // Add to equal
            NODE* next = current->next;
            add_node(equal, current->data); // Use add_node to add to the equal list
            current = next;
        } else {
            // Add to greater
            NODE* next = current->next;
            add_node(greater, current->data); // Use add_node to add to the greater list
            current = next;
        }
    }
}

LinkedList* concatenate(LinkedList* less, LinkedList* equal, LinkedList* greater) {
    /*
    Concatenates three linked lists into a single linked list.
    Arguments:
        LinkedList* less      -> Pointer to the 'less' linked list.
        LinkedList* equal     -> Pointer to the 'equal' linked list.
        LinkedList* greater   -> Pointer to the 'greater' linked list.
    Return:
        LinkedList*          -> Pointer to the head of the concatenated linked list.
                                If all input lists are NULL, it returns NULL.
    */
    LinkedList* result = create_linked_list();
    if (result == NULL) return NULL;
    NODE* current = less->head;
    while (current != NULL) {
        add_node(result, current->data);
        current = current->next;
    }
    current = equal->head;
    while (current != NULL) {
        add_node(result, current->data);
        current = current->next;
    }
    current = greater->head;
    while (current != NULL) {
        add_node(result, current->data);
        current = current->next;
    }
    return result;
}

void three_way_quick_sort(LinkedList* list, int32_t (*compare)(void*, void*)) {
    if (list->head == NULL || list->head->next == NULL) return;

    LinkedList* less = create_linked_list();
    LinkedList* equal = create_linked_list();
    LinkedList* greater = create_linked_list();

    // Partition the list
    three_way_partition(list, less, equal, greater, compare);

    // Recursively sort the left and right parts
    three_way_quick_sort(less, compare);
    three_way_quick_sort(greater, compare);

    // Concatenate the sorted lists
    LinkedList* sorted_list = concatenate(less, equal, greater);

    // Update the original list
    list->head = sorted_list->head;
    list->size = less->size + equal->size + greater->size;

    // Clean up temporary lists
    delete_linked_list(less);
    delete_linked_list(equal);
    delete_linked_list(greater);
    delete_linked_list(sorted_list);
}
