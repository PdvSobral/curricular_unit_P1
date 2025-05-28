/* Encoding: UTF-8
@Authors: Pedro Sobral (33641)
@Date: 29/05/2025
@Links: Project github repository -> https://github.com/PdvSobral/curricular_unit_P1

This file contains the functions developed to abstract the main program to how the pseudo database is stored
*/
// Makes so that if "#define __main__" is not somewhere before this program is compiled an error ocurs, stopping compilation
#ifndef __main__
	#pragma GCC warning "This code ('database_helper.c') is not meant to be compiled directly. Be sure you know what you are doing."
	#define __main__
	#define __database_helper__
#else
	// Makes so that this file is only included once
	#pragma once
#endif

#include <stdlib.h>			// malloc, alloc (sem ela tmb tive algumas instabilidades com o uso de unsigneds)
#include <stdint.h>			// uint8_t
#include <stdio.h>	    	// printf
#include <string.h>			// strcpy
#include "linked_lists.c"
#include "typedefs.c"
#include "functions.c"

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
				break;
			}
		}
	}

	// Close the file
	fclose(file);
	return to_return;
}

ACCOUNT* get_user_with_id(const char* database_name, const char* id){
	FILE* file;
	uint8_t buffer[34];
	uint8_t to_read=5;
	uint8_t bytesRead;
	ACCOUNT* to_return = NULL;
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
			to_return = (ACCOUNT*) malloc(sizeof(ACCOUNT));
			buffer[5] = 0x00;
			printf("Debug: Comparing buffer with id: %s vs %s\n", buffer, id);
			if (strcmp((char*) buffer, (char*) id) == 0){
				to_return->uid = str_to_int64_t((char*) buffer);
				printf("Debug: UID found: %u\n", to_return->uid);
				fread(buffer, 1, 33, file);
				buffer[33] = 0x00;
				strcpy((char*) to_return->password, (char*) buffer + 1);
				printf("Debug: Password read: %s\n", to_return->password);
				fread(buffer, 1, 3, file);
				to_return->type = buffer[1] - 0x30;
				printf("Debug: Account type: %d\n", to_return->type);
				to_return->name_offset = (uint64_t) ftell(file);
				printf("Debug: Name offset: %u\n", to_return->name_offset);
				fclose(file);
				return to_return;
			}
			fseek(file, 36, SEEK_CUR);
			next_is_valid = 0;
		}
		printf("BUFFER: %s\n", buffer);
		to_read = 5;
		// Check for end of line (newline character) or end of file
		for (uint8_t i = 0; i < bytesRead; i++) {
			if (buffer[i] == '\n') {
				printf("Found \\n on index: %u\n", i);
				to_read = i + 1;
				next_is_valid = 1;
				strcpy((char*) buffer, (char*) buffer + to_read);
				break;
			}
		}
	}
	// Close the file
	fclose(file);
	return NULL;
}

#ifdef __database_helper__
	void print_account_data(ACCOUNT* data){
		printf("uID: %d\n", data->uid);
		printf("MD5: %s\n", data->password);
		printf("Type: %s\n", data->type==1?"Librarian":"Student");
		printf("Name Offset: %d\n", data->name_offset);
	}

	int main() {
		const char* filename = "./assets/sys_shadow.csv";
		ACCOUNT* my_user = get_user_with_id(filename, "29659");
		if (my_user == NULL) printf("NO USER FOUND!\n");
		else print_account_data(my_user);
		free(my_user);
	}
#endif
