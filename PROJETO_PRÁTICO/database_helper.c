/* Encoding: UTF-8
@Authors: Pedro Sobral (33641), Alexandre Domingos (27641)
@Date: 29/05/2025
@Links: Project github repository -> https://github.com/PdvSobral/curricular_unit_P1

This file contains the functions developed to abstract the main program to how the pseudo database is stored
*/
// Makes so that if "#define __main__" is not somewhere before this program is compiled an error ocurs, stopping compilation
#ifndef __main__
	#pragma GCC warning "This code ('database_helper.c') is not meant to be compiled directly. Be sure you know what you are doing."
	#include <stdlib.h>		// malloc, alloc (sem ela tmb tive algumas instabilidades com o uso de unsigneds)
	#include <stdint.h>		// uint8_t
	#include <stdio.h>		// printf
	#include <string.h>		// strcpy
	#define __main__
	#define __database_helper__
	#include "linked_lists.c"
#else
	// Makes so that this file is only included once
	#pragma once
#endif

LinkedList* get_users_ids(const char* database_name){
	FILE* file;
	uint8_t buffer[6]; // 5 + \0
	uint8_t to_read=5;
	uint8_t bytesRead;
	LinkedList* to_return = create_linked_list();
	char* to_hold_pointer;
	uint8_t next_is_valid=1;

	// Open the file for reading bytes (I'm getting chars, so it's the same)
	file = fopen(database_name, "rb");
	if (file == NULL) {
		perror("Error opening file");
		return NULL;
	}

	while (1) {
		// Read up to 5 bytes from the file
		bytesRead = fread(buffer+5-to_read, 1, to_read, file);
		// If no bytes were read, break the loop (end of file)
		if (bytesRead == 0) break;

		// Print the bytes read if they are what we want (later make a LinkedList):
		if (next_is_valid == 1){
			to_hold_pointer = (char*) malloc(sizeof(buffer));
			buffer[5] = 0x00;
			strcpy((char*) to_hold_pointer, (char*) buffer);
			append_data_to_list(to_return, to_hold_pointer);
			next_is_valid=0;
		}

		to_read = 5;
		// Check for end of line (newline character) or end of file
		for (uint8_t i=0; i < bytesRead; i++){
			if (buffer[i] == '\n') {
				to_read = i+1;
				next_is_valid=1;
				strcpy((char*) buffer, (char*) buffer+i+1);
				//break;
			}
		}
	}

	// Close the file
	fclose(file);
	return to_return;
}

#ifdef __database_helper__
	void print_data(void* data){
		printf("%s\n", (char*) data);
	}

	int main() {
		const char* filename = "./assets/sys_shadow.csv";
		LinkedList* all_ids = get_users_ids(filename);
		traverse_list(all_ids, print_data);
		delete_linked_list(all_ids, free);
	}
#endif