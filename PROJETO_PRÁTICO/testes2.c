//TODO: Review this GPT code
#pragma GCC warning " DO NOT COMPILE AND RUN, IT'S FULL GPT!!"
#define __main__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include "functions.c"
#include "database_helper.c"

#define MAX_INPUT 7
#define FILE_PATH "./none.txt"

// Returns the length in bytes of the UTF-8 character starting with byte ch
int utf8_char_length(uint8_t ch) {
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

int32_t main(){
	printf("Introduce the message: ");
	fflush(stdout);
	uint8_t returned;
	returned = read_text_and_append_to_file(FILE_PATH, MAX_INPUT, 0);
	printf("\n\nReturned %u\n", returned);
	return 0;
}
