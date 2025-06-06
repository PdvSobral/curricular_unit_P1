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
#include <dirent.h>
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

// FIXME: Check what would happen if the name was left empty due to crash in account registry
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

//TODO: Define this prototype
BOOK* get_book_by_id(const char* archive_folder, const char id[14]){
	/*
	Return NULL if no book, else book info in struct
	Arguments:
		const char* archive_folder: Name of the folder containing the book files. Must exist
		const char id[14]:			String with 13 numbers (ISBN-13) + 0x00
	Return:
		BOOK* | NULL : Returns a pointer to a book object corresponding to the id. Null if an error ocurred or it was not found.
	*/
    char file_path[sizeof(archive_folder)+14];
    sprintf(file_path, "%s/%s.txt", archive_folder, id);

	BOOK* book = (BOOK*) malloc(sizeof(BOOK));
	if (book == NULL) return NULL;
    FILE* file = fopen(file_path, "rb");
    if (file == NULL) return NULL;

    // Read the book's information from the file
    // ...

    fclose(file);
    return book;
}

//TODO: Verificar o prototipo, por favor
LinkedList* get_book_ids(const char* archive_folder) {
    DIR* dir;
    struct dirent* entry;
    LinkedList* list = create_linked_list();
	// Abrir o diretório
    dir = opendir(archive_folder);
    if (dir == NULL) return list;
	// Percorre os ficheiros do diretório
    while ((entry = readdir(dir)) != NULL) {
    	// FIXME: Probably later make it 16
        uint32_t len = strlen2(entry->d_name);
        // Verifica se é ficheiro .csv com 13 dígitos no nome
        if (len == 17 && strcmp(entry->d_name + len - 4, ".csv") == 0) {
            char isbn_str[14] = {0};
			// Copia os 13 caracteres do ISBN para uma string e converte para uint64_t
            strncpy(isbn_str, entry->d_name, 13);
            uint64_t isbn = strtoull(isbn_str, NULL, 10);
			//Adição do livro à lista
            BOOK* book = (BOOK*)malloc(sizeof(BOOK));
            book->uid = isbn;
            book->name[0] = '\0'; // Para preencher o nome do livro
            append_data_to_list(list, book);
        }
    }
    closedir(dir);
    return list;
}
// Return LinkedList with apontador to null

//TODO: Define this prototype
void print_book_name(const char* archive_folder, uint32_t uid);
// Prints a not newline terminated name for the uid provided

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

void overwrite_password(const char* database_name, uint32_t name_offset, char* buffer){
	FILE* file;
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

uint8_t utf8_char_length(uint8_t ch) {
	if (ch < 0x80) return 1;
	else if ((ch >> 5) == 0x6) return 2;
	else if ((ch >> 4) == 0xE) return 3;
	else if ((ch >> 3) == 0x1E) return 4;
	return 1;
}
uint8_t is_utf8_continuation(uint8_t ch) {
	return ch >= 0x80 && ch < 0xC0;
}
void end_file(FILE* file_to_end){
	uint32_t original_pos = ftell(file_to_end);
	fseek(file_to_end, 0, SEEK_END);
	uint32_t file_length = ftell(file_to_end);
	fseek(file_to_end, original_pos, SEEK_SET);
	if (file_length > original_pos) {
		ftruncate(fileno(file_to_end), original_pos);
	}
	return;
}
uint8_t read_text_and_append_to_file(const char* file_name, uint8_t max_characters_length, uint8_t new_line){
    touch(file_name);
    FILE* file = fopen(file_name, "rb+");  // for testing
    if (file == NULL) return 1;
    fseek(file, 0, SEEK_END);
    uint8_t ch;
    uint8_t string_len_in_chars = 0;
    uint8_t char_len;
    while (1){
        ch = getch();
        if (ch == 0x0A) break;
        if (ch == 0x1B) {
			if ((ch = getch()) == 0x5B) {
				if ((ch = getch()) >= 'A' && ch <= 'F'){}
				else if (ch == '2'){
					if( (ch = getch()) == '~'){}
					if( ( ch >= '0' && ch <= '4' ) && (ch = getch()) == '~' ){}
				}
				else if ((ch >= '3' && ch <= '6') && (ch = getch()) == '~') {}
				else if (ch == '1' && ( (ch = getch()) == '5' || (ch >= '7' && ch <= '9') ) && (ch = getch()) == '~'){}
			} else if (ch == 0x4F) {
				ch = getch();
				if (ch >= 'P' && ch <= 'S') {}
			}
		}
        else if (ch == 0x7F || ch == 0x08){  // Backspace && Delete
            if (string_len_in_chars > 0) {
                string_len_in_chars--;
                printf("\033[1D \033[1D");
                fseek(file, -1, SEEK_CUR);
                do {  // check if the byte is an UTF continuation bite. If it is, 'delete' too.
                    ch = fgetc(file);
                    fseek(file, -1, SEEK_CUR);
					if (utf8_char_length(ch)!=1) break;
                    if (is_utf8_continuation(ch) == 0) break;
                    fseek(file, -1, SEEK_CUR); // if it is a continuation byte, delete it and continue the loop
                } while (1);
            }
            fflush(stdout);
        }
        else if (string_len_in_chars < max_characters_length){
            fwrite(&ch, sizeof(uint8_t), 1, file);
            putchar(ch);
            // Write remaining bytes if multibyte
			char_len = utf8_char_length(ch);
            for (uint8_t i = 1; i < char_len; ++i) {
                ch = getch();
                fputc(ch, file); putchar(ch);
            }
            string_len_in_chars++;
        }
        fflush(stdout);
    }
    putchar(0x0A);
    if(new_line==1) fputc(0x0A, file);
    end_file(file);
    fclose(file);
    return 0;
}

void print_account_data(ACCOUNT* data){
	printf("uID: %d\n", data->uid);
	printf("MD5: %s\n", data->password);
	printf("Type: %s\n", data->type==1?"Librarian":"Student");
	printf("Name Offset: %d\n", data->name_offset);
}

#ifdef __database_helper__
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
