/* Encoding: UTF-8
@Authors: Pedro Sobral (33641)
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

typedef struct _book{
	uint64_t uid;      					// Unique ID, ISBN-13
	char name[100];         			// Name
	uint8_t quantity;					// How many books are there in the bibl system
	char description[100]; 				// Very long string :), probably end up using offsets like name in account
	LinkedList* requested_by;  			// Contains a list of users that currently have the book
	LinkedList* queue_for_students;	    // Maybe later a file??
} BOOK;
