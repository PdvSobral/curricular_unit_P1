#define __main__
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "typedefs.c"
#include "database_helper.c"
#include "functions.c"

BOOK* get_book_by_id2(const char* archive_folder, char id[14]){
	/*
	Return NULL if no book, else book info in struct
	Arguments:
		const char* archive_folder: Name of the folder containing the book files. Must exist and contain a slash (/) at the end
		const char id[14]:			String with 13 numbers (ISBN-13) + 0x00
	Return:
		BOOK* | NULL : Returns a pointer to a book object corresponding to the id. Null if an error ocurred or it was not found.
	*/
	BOOK* book = (BOOK*) malloc(sizeof(BOOK));
	if (book == NULL) return NULL;
	book->uid = 0;
	while(archive_folder[book->uid] != 0x00) book->uid++;
    char file_path[book->uid+18];
    snprintf(file_path, book->uid+18, "%s%s.csv", archive_folder, id); // 18 because it counts 0x00
    FILE* file = fopen(file_path, "rb");
    if (file == NULL) {free(book); return NULL;};
	book->uid = (uint64_t) str_to_int64_t(id); // Define book isbn
	// Until here it's fine
	char buffer[7]; // it is going to read 6 + 0x00;
	int8_t bytesRead;
	buffer[6] = 0x00;  // protect against runaway string
	// Just defines above, now read current
	bytesRead = fread(buffer, 1, 6, file);
	if (bytesRead != 6){fclose(file); free(book); return NULL;};
	book->requested_by = (uint32_t) str_to_int64_t(buffer);
	// Now read name (offset is always 6)
	bytesRead = -1;
	while(bytesRead==-1){
		bytesRead = fread(buffer, 1, 6, file);
		if (bytesRead != 6) {fclose(file); free(book); return NULL;};
		for (bytesRead--; bytesRead >= 0; bytesRead--) {
			if (buffer[bytesRead] == 0x0A) {
				fseek(file, -1*(5-bytesRead), SEEK_CUR); // Take back the ammount read from the description
				break;
			}
		}
	}
	book->description_offset = ftell(file); // first byte of the description will be here.

	// Now read description
	bytesRead = -1;
	while(bytesRead==-1){
		bytesRead = fread(buffer, 1, 6, file);
		if (bytesRead != 6) {
			// if this happened, it's because either the loop failed to exit, or, most likely, there is no one on queue.
			book->queue_offset = ftell(file);
			fclose(file);
			book->queue_for_students = create_linked_list();
			return book;
		}
		for (bytesRead--; bytesRead >= 0; bytesRead--) {
			if (buffer[bytesRead] == 0x0A) {
				fseek(file, -1*(5-bytesRead), SEEK_CUR);
				break;
			}
		}
	}
	book->queue_offset = ftell(file);
	book->queue_for_students = create_linked_list();
	uint32_t* lol;
	while(1){
		bytesRead = fread(buffer, 1, 6, file);
		if (bytesRead == 0 || buffer[0] == 0x0A) break;
		lol = malloc(sizeof(uint32_t));
		*lol = (uint32_t) str_to_int64_t(buffer);
		append_data_to_list(book->queue_for_students, lol);
	}
    fclose(file);
    return book;
}

void main(){
	char* id_book_str = "9789727221561";
	BOOK* book = get_book_by_id2("./assets/books/", id_book_str);
	if (book==NULL){ printf("ERROR!\n"); return;}
	print_book_data(book);
	delete_linked_list(book->queue_for_students, free);
	free(book);
}
