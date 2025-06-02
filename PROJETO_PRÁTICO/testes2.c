//TODO: Review this GPT code
#pragma GCC error " DO NOT COMPILE AND RUN, IT'S FULL GPT!!"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <termios.h>

#define MAX_INPUT 256
#define FILE_PATH "./names.txt"

// Enable raw mode to read char-by-char without Enter
void enable_raw_mode(struct termios* orig_termios) {
	struct termios raw;
	tcgetattr(STDIN_FILENO, orig_termios);
	raw = *orig_termios;
	raw.c_lflag &= ~(ECHO | ICANON);
	tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

// Restore terminal
void disable_raw_mode(const struct termios* orig_termios) {
	tcsetattr(STDIN_FILENO, TCSAFLUSH, orig_termios);
}

// Returns the length in bytes of the UTF-8 character starting with byte ch
int utf8_char_length(unsigned char ch) {
	if (ch < 0x80) return 1;
	else if ((ch >> 5) == 0x6) return 2;
	else if ((ch >> 4) == 0xE) return 3;
	else if ((ch >> 3) == 0x1E) return 4;
	return 1;
}

// Check if byte is a UTF-8 continuation byte (i.e., starts with 10xxxxxx)
int is_utf8_continuation(unsigned char ch) {
	return (ch & 0xC0) == 0x80;
}

void read_name_and_append_to_file() {
	struct termios orig_termios;
	enable_raw_mode(&orig_termios);

	printf("Enter name: ");
	fflush(stdout);

	unsigned char name[MAX_INPUT] = {0};
	int len = 0;

	while (1) {
		unsigned char ch;
		if (read(STDIN_FILENO, &ch, 1) != 1) break;

		if (ch == 0x7F || ch == 0x08) {  // Backspace/Delete
			if (len > 0) {
				// Remove UTF-8 multibyte char correctly
				do {
					len--;
				} while (len > 0 && is_utf8_continuation(name[len]));
				name[len] = '\0';
				printf("\b \b");  // Erase from screen
				fflush(stdout);
			}
		}
		else if (ch == '\n' || ch == '\r') {
			break;
		}
		else {
			int char_len = utf8_char_length(ch);
			if (len + char_len >= MAX_INPUT) continue;
			name[len++] = ch;
			putchar(ch);
			// Read remaining bytes if multibyte
			for (int i = 1; i < char_len; ++i) {
				if (read(STDIN_FILENO, &ch, 1) != 1) break;
				name[len++] = ch;
				putchar(ch);
			}
			fflush(stdout);
		}
	}

	disable_raw_mode(&orig_termios);
	putchar('\n');

	// Append directly to file
	FILE* fp = fopen(FILE_PATH, "r+");
	if (!fp) {
		perror("Error opening file");
		return;
	}
	fseek(fp, 0, SEEK_END);  // Move to end
	fwrite(name, 1, len, fp);
	fputc('\n', fp);         // Optional newline
	// fputc(0x1A, fp);      // Optional EOF marker for legacy compatibility
	fclose(fp);
}
