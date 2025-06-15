/* Encoding: UTF-8
@Authors: Pedro Sobral (33641), Alexandre Domingos (27641), Diogo Meneses (33826)
@Date: 28/05/2025
@Links: Project github repository -> https://github.com/PdvSobral/curricular_unit_P1

This file contains the types used throughout this work.
*/
// Makes so that if "#define __main__" is not somewhere before this program is compiled an error ocurs, stopping compilation
#ifndef __main__
#pragma GCC error "This code is not meant to be compiled directly."
#else
// Makes so that this file is only included once
#pragma once
#endif

#include "linked_lists.c"

typedef struct _account{
	uint32_t uid;      		// Unique ID, normal unsigned int for now
	uint32_t name_offset;	// Name pos in the database file until newline (uint32_t serves for files up to 4GB)
	uint8_t type;			// 0 = Student   | 1 = Librarian
	char password[33]; 		// MD5, 32 + \0
} ACCOUNT;

typedef struct _book{					// name offset is always 6
	uint64_t uid;      					// Unique ID, ISBN-13
	uint32_t description_offset; 		// Description position in the file, reads until \n
	uint32_t queue_offset; 				// Qeue position in the file, reads until \n, write from here on
	uint32_t requested_by;  			// uID of the user that currently has the book
	LinkedList* queue_for_students;	    // List of uint32_t, the uIDs for the accounts in queue
} BOOK;
