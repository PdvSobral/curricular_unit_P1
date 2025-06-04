/* Encoding: UTF-8
@Authors: Pedro Sobral (33641), Alexandre Domingos (27641)
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
	uint8_t buffer[6];
	uint8_t to_read=5;
	uint8_t bytesRead;
	LinkedList* to_return = create_linked_list();
	char* to_hold_pointer;
	uint8_t next_is_valid=1;
	file = fopen(database_name, "rb");
	if (file == NULL) return NULL;
	while (1) {
		bytesRead = fread(buffer+5-to_read, 1, to_read, file);
		if (bytesRead == 0) break;
		if (next_is_valid == 1){
			to_hold_pointer = (char*) malloc(sizeof(buffer));
			buffer[5] = 0x00;
			strcpy((char*) to_hold_pointer, (char*) buffer);
			append_data_to_list(to_return, to_hold_pointer);
			next_is_valid=0;
		}
		to_read = 5;
		for (uint8_t i=0; i < bytesRead; i++){
			if (buffer[i] == '\n') {
				to_read = i+1;
				next_is_valid=1;
				strcpy((char*) buffer, (char*) buffer+i+1);
				break;
			}
		}
	}
	fclose(file);
	return to_return;
}

ACCOUNT* get_user_by_id(const char* database_name, const char* id){
	FILE* file;
	uint8_t buffer[34];
	uint8_t to_read=5;
	uint8_t bytesRead;
	ACCOUNT* to_return = NULL;
	uint8_t next_is_valid=1;
	file = fopen(database_name, "rb");
	if (file == NULL) return NULL;
	while (1) {
		bytesRead = fread(buffer+5-to_read, 1, to_read, file);
		if (bytesRead == 0) break;
		if (next_is_valid == 1){
			to_return = (ACCOUNT*) malloc(sizeof(ACCOUNT));
			buffer[5] = 0x00;
			if (strcmp((char*) buffer, (char*) id) == 0){
				to_return->uid = str_to_int64_t((char*) buffer);
				fread(buffer, 1, 33, file);
				buffer[33] = 0x00;
				strcpy((char*) to_return->password, (char*) buffer + 1);
				fread(buffer, 1, 3, file);
				to_return->type = buffer[1] - 0x30;
				to_return->name_offset = (uint64_t) ftell(file);
				fclose(file);
				return to_return;
			}
			fseek(file, 36, SEEK_CUR);
			next_is_valid = 0;
		}
		to_read = 5;
		for (uint8_t i = 0; i < bytesRead; i++) {
			if (buffer[i] == '\n') {
				to_read = i + 1;
				next_is_valid = 1;
				strcpy((char*) buffer+7, (char*) buffer+to_read);
				strcpy((char*) buffer, (char*) buffer+7);
				break;
			}
		}
	}
	fclose(file);
	return NULL;
}

void print_name(const char* database_name, uint32_t name_offset){
	FILE* file;
	uint8_t buffer[31];
	uint8_t bytesRead;
	file = fopen(database_name, "rb");
	if (file == NULL) return;
	fseek(file, name_offset, SEEK_SET);
	while (1) {
		bytesRead = fread(buffer, 1, 10, file);
		if (bytesRead == 0) break;
		for (uint8_t i = 0; i < bytesRead; i++) {
			if (buffer[i] == '\n') {
				buffer[i] = 0x00;
				printf("%s", buffer);
				fclose(file);
				return;
			}
		}
		buffer[10] = 0x00;
		printf("%s", buffer);
	}
	fclose(file);
	return;
}

void overwrite_password(const char* database_name, uint32_t name_offset){
	FILE* file;
	char* buffer = "5f4dcc3b5aa765d61d8327deb882cf99"; // password hash default (pass='password')
	file = fopen(database_name, "rb+");
	if (file == NULL) return;
	fseek(file, name_offset-35, SEEK_SET);
	fwrite(buffer, sizeof(uint8_t), strlen2(buffer), file);
	fclose(file);
	return;
}

void touch(const char* database_name){
	if (access(database_name, F_OK) == -1) {
		FILE* file = fopen(database_name, "wb");
		fclose(file);
	}
	return;
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
		char to_check[10];
		printf("Enter ID to search: ");
		scanf("%5[^\n]", to_check);
		printf("Checking for: '%s'\n", to_check);
		ACCOUNT* my_user = get_user_by_id(filename, to_check);
		if (my_user == NULL) printf("NO USER FOUND!\n");
		else print_account_data(my_user);
		free(my_user);
	}
#endif
