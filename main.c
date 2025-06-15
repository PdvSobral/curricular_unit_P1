/* Encoding: UTF-8
@Authors: Pedro Sobral (33641), Alexandre Domingos (27641), Diogo Meneses (33826)
@Date: 15/06/2025
@Links: Project github repository -> https://github.com/PdvSobral/curricular_unit_P1

This program was developed as an answer to a problem described in a pdf file provided to us.
The pdf is present in the same repository as this program.
*/
#define __main__	// because of it is the first, the other will not compile if not this line

#include <stdio.h>		// For printf
#include <stdlib.h>		// malloc()
#include <time.h>		// To get current time and date
#include <ctype.h>		// tolower()
#include <stdint.h>		// For uint8_t
#include <termios.h>    // Disabl_ctrl_d (IN) functions.c
#include <signal.h>		// To remap CTRL+C
#include <string.h>		// strcmp, strlens
#include <unistd.h>		// sleep, STDIN_FILENO
#include <dirent.h>		// for directory listing
#include <inttypes.h>	// for string formating (like PRIu64)

// Check for a macro defined in the command line. If present, then also compile 'good_practices.c'
#ifndef psystem
	// warning just to make sure in what mode the code is being compiled
	#pragma GCC warning "Loaded standard modules. Please use strlen2 instead of strlen."
#else
	#include "good_practices.c"
	// Makes so that int, unsigned and other lables/macros are not usable, as well as other good practises I like to adhere
	#pragma GCC warning "Loaded custom module."
#endif

// Import all custom libraries. Some are imported by others before, but nothing is lost in doing it again. For some reason pragma onces exist.
#include "functions.c"
#include "linked_lists.c"
#include "database_helper.c"
#include "typedefs.c"
#include "md5.c"

// Define some macros for use in the "in the program" (yes, I know tecnicly they are not in the final binary directly, but whatever).
#define AGGRESSIVE					// If defined, CTRL+D is disabled in the current terminal, anc a custom hadler for CTRL+C is activated.
#define MAX_PASSWORD_LENGTH 30		// Maximum characters/bytes a password can have
#define MAX_TITLE_LENGTH 	100		//    =         =    /  =   =  book title can have
#define MAX_NAME_LENGHT 	70		//    =         =    /  =   =  user name can have

// Custom str formatting for ISBN, will check during comenting if it is used everywhere, as it should
#ifndef ISBN_FORMAT
	// TODO: Check it is implement in all ISBN printings and castings !!
	#define ISBN_FORMAT "%013" PRIu64
#endif

// Definition of some constants
const char* USER_DATABASE = "./assets/sys_shadow.csv";	// Path to the file containing the users
const char* BOOK_ARCHIVE_DIR = "./assets/books/"; 		// Path where the book files are stored, MUST INCLUDE THE SLASH (/), parts of the code and buffers depend on that
const char* MAIN_LOG = "./assets/main.log";				// Path to the main log file, where all required evends and some more are stored
const char* HIST_LOG = "./assets/history.log";			// Path to the log containing only the history of book returns
static ACCOUNT CURRENT_LOGIN = {0, 999999999, 2, ""};	// Struct that contains the user currently logd in. Initiated to umpossible values.

// Definition of the menu arrays, always in the format len / options
const uint8_t len_main_menu = 3;
const char main_menu[][CABECALHO_LEN] = {
	"Log in as Librarian",
	"Log in as Student",
	"Exit"
};

const uint8_t len_student_account_menu = 7;
const char student_account_menu[][CABECALHO_LEN] = {
	"Check out book",
	"List books by ISBN",
	"List books alphabeticly",
	"List available books",
	"Check book information",
	"Manage account",
	"Logout"
};
const uint8_t len_mng_student_account_menu = 3;
const char mng_student_account_menu[][CABECALHO_LEN] = {
	"Change account name",
	"Change account password",
	"Return to main menu"
};

const uint8_t len_biblman_account_menu = 7;
const char biblman_account_menu[][CABECALHO_LEN] = {
	"Return book",
	"List books by ISBN",
	"List books alphabeticly",
	"List available books",
	"Check book information",
	"Manage system",
	"Logout"
};
const uint8_t len_mng_biblman_account_menu = 8;
const char mng_biblman_account_menu[][CABECALHO_LEN] = {
	"Add book",
	"Remove book",
	"Check book return history",
	"Remove old return history entries",
	"Create new account",
	"Reset password to an account",
	"Change current account password",
	"Return to main menu"
};

// Functions and methods

uint8_t log_action(BOOK* book, const char* log_file_path, const char mode[4]){
	/*
	Function used to write an action that took place to a log file in a specific format.
	The format that is logged is:
		[*] year-month-day hour:minute mode currently_logdin_user book_name

	Global variables or macros expected:
		char* BOOK_ARCHIVE_DIR		-> String to the directory where the book files are stored.
									   MUST CONTAIN THE FINAL SLASH ( / )
		ACCOUNT CURRENT_LOGIN		-> Currently loged-in user. Only the UID (in .uid) field is used.

	Arguments:
		BOOK* book					-> Pointer to a BOOK struct.
										Only the ISBN (in .uid) and NAME OFFSET (in .name_offset) fields are used.
		const char* log_file_path	-> Pointer to null terminated string to the file where the log message is writen.
		const char mode[4]			-> Three character string (3 + 0x00) to be written as mode.

	Return (uint8_t):
		0 -> Everything went fine.
		1 -> When trying opening the log file, NULL was returned by fopen
		2 -> When trying opening the book file based on the ISBN, NULL was returned by fopen
		3 -> Not suposed to get to the end, the flow was broken or another error happened
	*/
    FILE* file = fopen(log_file_path, "at");
    if (file == NULL){
        fclose(file);
        printf("ERROR OPENING LOG FILE!\n");
        fflush(stdout);
        return 1;
    }
    DATE date = get_current_date(date);
    TIME times = get_current_time(times);
    fprintf(file, "[*] %04d-%02d-%02d %02d:%02d %s %d ", date.year, date.month, date.day, times.hour, times.minutes, mode, CURRENT_LOGIN.uid);

    char buffer[7];
    buffer[6] = 0x00; // to prevent from runaway strings
    buffer[0] = 0x00; // to use as counter for len of BOOK_ARCHIVE_DIR, macro defined in this file
    while ((uint8_t)BOOK_ARCHIVE_DIR[(uint8_t)buffer[0]] != 0x00) buffer[0]++;

    // 'Compile' the complete book path and open the file
    char file_path2[buffer[0] + 18];
    snprintf(file_path2, sizeof(file_path2), "%s" ISBN_FORMAT ".csv", BOOK_ARCHIVE_DIR, book->uid);
    FILE* file2 = fopen(file_path2, "rb");
    if (file2 == NULL){
        fclose(file);
        printf("ERROR OPENING BOOK FILE!\n");
        fflush(stdout);
        return 2;
    }

	// Copy the book name from the book file to the log in chunks of 6 bytes.
    int8_t bytesRead;
    fseek(file2, 6, SEEK_SET);
    while (1){
        bytesRead = fread(buffer, 1, 6, file2);
        if (bytesRead == 0) break;

        for (bytesRead--; bytesRead >= 0; bytesRead--) {
            if (buffer[bytesRead] == 0x0A) {
                buffer[bytesRead] = 0x00;
                fprintf(file, "%s\n", buffer); // Fim da linha, imprime com quebra
                fclose(file2);
                fclose(file);
                return 0;
            }
        }
        fprintf(file, "%s", buffer); // Continua imprimindo a linha
    }
	printf("Not suposed to get here!\n");
	fflush(stdout);
    fclose(file2);
    fclose(file);
    return 3;
}

uint8_t log_(const char* log_file_path, const char* msg){
	/*
	Function used to write a system action that took place to a log file in a specific format.
	The format that is logged is:
		[*] year-month-day hour:minute LOG 00000 message

	Arguments:
		const char* log_file_path	-> Pointer to null terminated string to the file where the log message is writen.
		const char* msg				-> Pointer to null terminated string containing the message to be logged.

	Return (uint8_t):
		0 -> Everything went fine.
		1 -> When trying opening the log file, NULL was returned by fopen
	*/
    FILE* file = fopen(log_file_path, "at");
    if (file == NULL) return 1;

    DATE date = get_current_date(date);
    TIME times = get_current_time(times);
    fprintf(file, "[*] %04d-%02d-%02d %02d:%02d LOG 00000 %s\n", date.year, date.month, date.day, times.hour, times.minutes, msg);
    fclose(file);
    return 0;
}

uint8_t logc_(const char* log_file_path, const char* msg){
	/*
	Function used to write a custom action from the current user that took place to a log file in a specific format.
	The format that is logged is:
		[*] year-month-day hour:minute LOG currently_logdin_uid message

	Global variables or macros expected:
		ACCOUNT CURRENT_LOGIN		-> Currently loged-in user. Only the UID (in .uid) field is used.

	Arguments:
		const char* log_file_path	-> Pointer to null terminated string to the file where the log message is writen.
		const char* msg				-> Pointer to null terminated string containing the message to be logged.

	Return (uint8_t):
		0 -> Everything went fine.
		1 -> When trying opening the log file, NULL was returned by fopen
	*/
    FILE* file = fopen(log_file_path, "at");
    if (file == NULL) return 1;

    DATE date = get_current_date(date);
    TIME times = get_current_time(times);
    fprintf(file, "[*] %04d-%02d-%02d %02d:%02d LOG %5u %s\n", date.year, date.month, date.day, times.hour, times.minutes, CURRENT_LOGIN.uid, msg);
    fclose(file);
    return 0;
}

uint8_t login(uint8_t account_flag_type){
	/*
	Function used to handle the login process for a user.
	It displays a login interface, accepts user input for account ID and password,
	and checks the credentials against a user database.

	Global variables or macros expected:
		ACCOUNT CURRENT_LOGIN		-> Currently loged-in user. All field will be set if a login is sucessful.
		uint8_t CABECALHO_LEN		-> Ammount of chars to be taken up by the interface

	Arguments:
		uint8_t account_flag_type	-> Account type expected and acepted for sucessful login.

	Return (uint8_t):
		0 -> Login successful
		1 -> Login cancelled
 	*/
	clear_screen();
	cabecalho("LOGIN ", CABECALHO_LEN);
	reset_line(CABECALHO_LEN);
	printf("│ [*] Account ID:");
	printf("\033[%uC│\n", CABECALHO_LEN - 18);
	printf("│ [ ] Password:");
	printf("\033[%uC│\n", CABECALHO_LEN - 16);
	for (uint8_t i = 0; i < CABECALHO_LEN; i++) printf("─");
	printf("\n");
	reset_line(CABECALHO_LEN);
	printf("│ [ ] Login   [ ] Cancel");
	printf("\033[%uC│\n", CABECALHO_LEN - 25);
	printf("└");
	for(uint8_t _index = 0; _index<CABECALHO_LEN-2; _index++){
		printf("─");
	} printf("┘");
	printf("\033[4A\033[999D\033[18C");
	fflush(stdout);
	uint8_t state=0; //0 - account is  | 1 - password id   | login 'button'
	uint8_t temp;
	char ch;
	char account_id[6]; account_id[0] = 0x00;
	char password[MAX_PASSWORD_LENGTH + 1]; password[0] = 0x00;
	char md5_hash[33];
	while (1){
		// chars especiais tipo setas
		if ((ch = getch()) == 27 && (ch = getch()) == 91) {
			if (((ch = getch()) == 65 || ch == 68) && state != 0){  // up arrow pressed
				// printf("↑");
				switch(state){
					case 1: printf("\033[999D\033[3C \033[1A\033[5D\033[3C*\033[%dC", 14 + strlen2(account_id)); break;
					case 2: printf(" \033[1D\033[2A*\033[%dC", 12 + strlen2(password)); break;
					case 3: printf(" \033[13D*\033[1D"); break;
				}
				state--;
			}
			else if ((ch == 66 || ch == 67) && state != 3){		 // down arrow pressed
				// printf("↓");
				switch(state){
					case 0: printf("\033[999D\033[3C \033[1B\033[5D\033[3C*\033[%dC", 12 + strlen2(password)); break;
					case 1: printf("\033[999D\033[3C \033[2B\033[5D\033[3C*\033[1D"); break;
					case 2: printf(" \033[11C*\033[1D"); break;
				}
				state++;
			}
		}
		else {
			if (state == 0 && strlen2(account_id) <= 5){
				// printf("%d", ch);
				if ((ch == 0x08 || ch == 0x7F)){   //  Delete and backspace
					if (strlen2(account_id) > 0){
						printf("\033[1D \033[1D");
						temp = strlen2(account_id);
						account_id[temp-1] = 0x00;
					}
				} else {
					if (ch >= 0x30 && ch <= 0x39 && strlen2(account_id) < 5){
						putchar(ch);
						temp = strlen2(account_id);
						account_id[temp] = ch;
						account_id[temp+1] = 0x00;
					}
				}
			}
			if (state == 1 && strlen2(password) <= MAX_PASSWORD_LENGTH){
				//printf("%d ", ch);
				if ((ch == 0x08 || ch == 0x7F)){   //  Delete and backspace
					if (strlen2(password) > 0){
						printf("\033[1D \033[1D");
						temp = strlen2(password);
						password[temp-1] = 0x00;
					}
				} else {
					if ((ch >= 0x20 || (int32_t) ch < 0x00) && strlen2(password) < MAX_PASSWORD_LENGTH){
						putchar('*');
						temp = strlen2(password);
						password[temp] = ch;
						password[temp+1] = 0x00;
					}
				}
			}
			if (state == 2){
				if (ch == 0x0A){
					if (strlen2(account_id) == 0){
						printf("\n\n");
						reset_line(CABECALHO_LEN);
						print_between_format("Nothing in the ID fiel!", "\033[31m", CABECALHO_LEN, 1);
						print_bottom(CABECALHO_LEN, 1);
						printf("\033[4A\033[3C");
					} else {
						if (strlen2(account_id) < 5){
							printf("\n\n");
							reset_line(CABECALHO_LEN);
							print_between_format("Invalid account ID!", "\033[31m", CABECALHO_LEN, 1);
							print_bottom(CABECALHO_LEN, 1);
							printf("\033[4A\033[3C");
						} else {
							if (strlen2(password) == 0){
								printf("\n\n");
								reset_line(CABECALHO_LEN);
								print_between_format("Nothing in the password field!", "\033[31m", CABECALHO_LEN, 1);
								print_bottom(CABECALHO_LEN, 1);
								printf("\033[4A\033[3C");
							} else {
								ACCOUNT* my_user = get_user_by_id(USER_DATABASE, account_id);
								hash_md5(password, md5_hash);
								if (my_user == NULL || my_user->type != account_flag_type || strcmp(my_user->password, md5_hash)!=0){
									free(my_user);
									printf("\n\n");
									reset_line(CABECALHO_LEN);
									print_between_format("INVALID CREDENTIALS!", "\033[31m", CABECALHO_LEN, 1);
									print_bottom(CABECALHO_LEN, 1);
									printf("\033[4A\033[3C");
								} else {
									printf("\n\n");
									reset_line(CABECALHO_LEN);
									print_between_format("LOGIN SUCESSFUL!", "\033[32m", CABECALHO_LEN, 1);
									print_bottom(CABECALHO_LEN, 1);
									char buffer_log[14+6];
									snprintf(buffer_log, sizeof(buffer_log),"User %d logd in.", my_user->uid);
									log_(MAIN_LOG, buffer_log);
									CURRENT_LOGIN = *my_user;
									free(my_user);
									pause_();
									return 0;  // sucessfull
								}
							}
						}
					}
				}
			}
			if (state == 3){
				if (ch == 0x0A){
					printf("\n\n");
					fflush(stdout);
					return 1;
				}
			}
		}
		fflush(stdout);
	}
	// Not suposed to get here
	return 1;
}

void logout(){
	/*
	Function used to handle the logout process for the currently logd-in user.
	It updates the CURRENT_LOGIN global variable to reflect a logged-out state and logs the logout event. (Very proud of that tongue twister, btw)

	Global variables or macros expected:
		ACCOUNT CURRENT_LOGIN		-> Currently logd-in user. All fields will be reset to default impossible values.
		const char* MAIN_LOG		-> Path to the main log file.

	Return:
		None
	*/
	char buffer[15+6];
	snprintf(buffer, sizeof(buffer),"User %d logd out.", CURRENT_LOGIN.uid);
	CURRENT_LOGIN.uid = 0;
	CURRENT_LOGIN.name_offset = 999999999;
	CURRENT_LOGIN.type = 2;
	strcpy(CURRENT_LOGIN.password, "");
	log_(MAIN_LOG, buffer);
	return;
}

uint8_t regist(){
	/*
	Function used to handle the registration process of a new user.
	It displays a registration interface, accepts user input for account ID, password (with *, took a hell of long time),
	name and checks the input correctness before creating a new user account. (My english is not the best, be glad that there is any documentation)
	Also, the data is all kept volatily right up to the name inputing, where the current data is flushed
	and the user then strats inputting the name directly into the file. (That took hours to implements, so better work)

	Global variables or macros expected:
		const char* USER_DATABASE	-> Path to the user database file.
		uint8_t CABECALHO_LEN		-> Amount of characters to be taken up by the interface.
		uint8_t MAX_PASSWORD_LENGTH	-> Maximum length of a password.
		uint8_t MAX_NAME_LENGHT		-> Maximum length of a user name.

	Return (uint8_t):
		0 -> Registration successful.
		1 -> Registration failed (likely due to file opening returning NULL).
	*/
	clear_screen();
	cabecalho("REGISTER ACCOUNT", CABECALHO_LEN);
	reset_line(CABECALHO_LEN);
	printf("│ [*] Account ID:");
	printf("\033[%uC│\n", CABECALHO_LEN - 18);
	printf("│ [ ] Password:");
	printf("\033[%uC│\n", CABECALHO_LEN - 16);
	printf("│ [ ] Retype Password:");
	printf("\033[%uC│\n", CABECALHO_LEN - 23);
	for (uint8_t i = 0; i < CABECALHO_LEN; i++) printf("─");
	printf("\n");
	reset_line(CABECALHO_LEN);
	printf("│ [ ] Create  [ ] Cancel");
	printf("\033[%uC│\n", CABECALHO_LEN - 25);
	printf("└");
	for(uint8_t _index = 0; _index<CABECALHO_LEN-2; _index++){
		printf("─");
	} printf("┘");
	printf("\033[5A\033[999D\033[18C");
	fflush(stdout);
	uint8_t state=0; //0 - account is  | 1 - password id   | login 'button'
	uint8_t temp;
	char ch;
	char account_id[6]; account_id[0] = 0x00;
	char password[MAX_PASSWORD_LENGTH + 1]; password[0] = 0x00;
	char password2[MAX_PASSWORD_LENGTH + 1]; password2[0] = 0x00;
	while (1){
		// chars especiais tipo setas
		if (((ch = getch()) == 27) && ((ch = getch()) == 91)) {
			if (((ch = getch()) == 65 || ch == 68 )&& state != 0){  // up arrow pressed
				// printf("↑");
				switch(state){
					case 1: printf("\033[999D\033[3C \033[1A\033[5D\033[3C*\033[%dC", 14 + strlen2(account_id)); break;
					case 2: printf("\033[999D\033[3C \033[1A\033[5D\033[3C*\033[%dC", 12 + strlen2(password)); break;
					case 3: printf(" \033[1D\033[2A*\033[%dC", 19 + strlen2(password2)); break;
					case 4: printf(" \033[13D*\033[1D"); break;
				}
				state--;
			}
			else if ((ch == 66 || ch == 67) && state != 4){		 // down arrow pressed
				// printf("↓");
				switch(state){
					case 0: printf("\033[999D\033[3C \033[1B\033[5D\033[3C*\033[%dC", 12 + strlen2(password)); break;
					case 1: printf("\033[999D\033[3C \033[1B\033[5D\033[3C*\033[%dC", 19 + strlen2(password2)); break;
					case 2: printf("\033[999D\033[3C \033[2B\033[5D\033[3C*\033[1D"); break;
					case 3: printf(" \033[11C*\033[1D"); break;
				}
				state++;
			}
		}
		else {
			if (state == 0 && strlen2(account_id) <= 5){
				// printf("%d", ch);
				if ((ch == 0x08 || ch == 0x7F)){   //  Delete and backspace
					if (strlen2(account_id) > 0){
						printf("\033[1D \033[1D");
						temp = strlen2(account_id);
						account_id[temp-1] = 0x00;
					}
				} else {
					if (ch >= 0x30 && ch <= 0x39 && strlen2(account_id) < 5){
						putchar(ch);
						temp = strlen2(account_id);
						account_id[temp] = ch;
						account_id[temp+1] = 0x00;
					}
				}
			}
			if (state == 1 && strlen2(password) <= MAX_PASSWORD_LENGTH){
				//printf("%d ", ch);
				if ((ch == 0x08 || ch == 0x7F)){   //  Delete and backspace
					if (strlen2(password) > 0){
						printf("\033[1D \033[1D");
						temp = strlen2(password);
						password[temp-1] = 0x00;
					}
				} else {
					if ((ch >= 0x20 || (int32_t) ch < 0x00) && strlen2(password) < MAX_PASSWORD_LENGTH){
						putchar('*');
						temp = strlen2(password);
						password[temp] = ch;
						password[temp+1] = 0x00;
					}
				}
			}
			if (state == 2 && strlen2(password2) <= MAX_PASSWORD_LENGTH){
				//printf("%d ", ch);
				if ((ch == 0x08 || ch == 0x7F)){   //  Delete and backspace
					if (strlen2(password2) > 0){
						printf("\033[1D \033[1D");
						temp = strlen2(password2);
						password2[temp-1] = 0x00;
					}
				} else {
					if ((ch >= 0x20 || (int32_t) ch < 0x00) && strlen2(password2) < MAX_PASSWORD_LENGTH){
						putchar('*');
						temp = strlen2(password2);
						password2[temp] = ch;
						password2[temp+1] = 0x00;
					}
				}
			}
			if (state == 3){
				if (ch == 0x0A){
					if (strlen2(account_id) == 0){
						printf("\n\n");
						reset_line(CABECALHO_LEN);
						print_between_format("Nothing in the ID fiel!", "\033[31m", CABECALHO_LEN, 1);
						print_bottom(CABECALHO_LEN, 1);
						printf("\033[4A\033[3C");
					} else {
						if (strlen2(account_id) < 5){
							printf("\n\n");
							reset_line(CABECALHO_LEN);
							print_between_format("Invalid account ID (5 characters needed)!", "\033[31m", CABECALHO_LEN, 1);
							print_bottom(CABECALHO_LEN, 1);
							printf("\033[4A\033[3C");
						} else {
							if (strlen2(password) == 0){
								printf("\n\n");
								reset_line(CABECALHO_LEN);
								print_between_format("Nothing in the password field!", "\033[31m", CABECALHO_LEN, 1);
								print_bottom(CABECALHO_LEN, 1);
								printf("\033[4A\033[3C");
							} else {
								if (strcmp(password, password2) != 0){
									printf("\n\n");
									reset_line(CABECALHO_LEN);
									print_between_format("Passwords do not match!", "\033[31m", CABECALHO_LEN, 1);
									print_bottom(CABECALHO_LEN, 1);
									printf("\033[4A\033[3C");
								} else {
									ACCOUNT* my_user = get_user_by_id(USER_DATABASE, account_id);
									if (my_user == NULL) break;
									else {
										printf("\n\n");
										reset_line(CABECALHO_LEN);
										print_between_format("User already exists!", "\033[31m", CABECALHO_LEN, 1);
										print_bottom(CABECALHO_LEN, 1);
										printf("\033[4A\033[3C");
									}
								}
							}
						}
					}
				}
			}
			if (state == 4){
				if (ch == 0x0A){
					printf("\n\n");
					fflush(stdout);
					return 1;
				}
			}
		}
		fflush(stdout);
	}
	printf("\n\n");
	reset_line(CABECALHO_LEN);
	print_between_format("User available and passwords match!", "\033[32m", CABECALHO_LEN, 1);
	cabecalho("CHOOSE ACCOUNT TYPE ", CABECALHO_LEN);
	printf("\033[2A");
	reset_line(CABECALHO_LEN);
	printf("\033[2B");
	reset_line(CABECALHO_LEN);
	printf("│ [*] Student [ ] Librarian");
	printf("\033[%uC│\n", CABECALHO_LEN - 28);
	printf("└");
	for(uint8_t _index = 0; _index<CABECALHO_LEN-2; _index++){
		printf("─");
	} printf("┘");
	printf("\033[1A\033[999D\033[3C");
	fflush(stdout);
	state=0; //0 - Student 1 - Librarian
	while (1){
		// chars especiais tipo setas
		if (((ch = getch()) == 27) && ((ch = getch()) == 91)) {
			if (((ch = getch()) == 65 || ch == 68) && state != 0){  // up arrow and left arrow pressed
				printf(" \033[13D*\033[1D");
				state--;
			}
			else if ((ch == 66 || ch == 67) && state != 1){ // down arrow and right arrow pressed
				printf(" \033[11C*\033[1D");
				state++;
			}
		}
		else if (ch == 0x0A) break;
		fflush(stdout);
	}
	printf("\n\n");
	reset_line(CABECALHO_LEN);
	print_between_format("Please enter your name now:", "\033[32m", CABECALHO_LEN, 1);
	FILE* file = fopen(USER_DATABASE, "a");
    if (file == NULL) return 1;
	char md5_hash[33];
    hash_md5(password, md5_hash);
    fprintf(file, "%s:%s:%1u:", account_id, md5_hash, state);  // 0 should be 'account_type'
    fclose(file);
	print_between_format("->", "\033[32m", CABECALHO_LEN, 1);
	print_bottom(CABECALHO_LEN, 1);
	printf("\033[2A\033[5C");
	fflush(stdout);
	read_text_and_append_to_file(USER_DATABASE, MAX_NAME_LENGHT, 1);
    printf("\nAccount registered successfully.\n");
	pause_();
	return 0;
}

void reset_password(){
	/*
	Function used to reset the password of a user account.
	It prompts the user for the account ID to reset the password, retrieves the corresponding user data,
	and resets the password to a default value (that value is harcoded, currently as 'password').
	To change, go to the marked line and replace the MD5 hash. BEWARE!!!!! The used md5 hashing algorithm only works
	as expected when no multibyte characters are used! So the default must be a simple password if an external md5
	implementations is used.

	Global variables or macros expected:
		const char* USER_DATABASE	-> Path to the user database file.

	Arguments:
		None

	Return:
		None
	*/
	char account_id[6];
	printf("ID Account: ");
	read_n_chars(5, account_id);
	ACCOUNT* user=get_user_by_id(USER_DATABASE, account_id);
	overwrite_password(USER_DATABASE, user->name_offset, "5f4dcc3b5aa765d61d8327deb882cf99");
	free(user);
}

uint8_t change_password(){
	/*
	Function used to change the password of the currently loged-in user.
	It prompts the user for a new password, confirms and updates the user's password in the database.
	Has the normal fancy and prety cool interface (in the humble opinion of the programmer).

	Global variables or macros expected:
		const char* USER_DATABASE	-> Path to the user database file.
		ACCOUNT CURRENT_LOGIN		-> Currently logged-in user.
		uint8_t MAX_PASSWORD_LENGTH	-> Maximum length of a password.

	Arguments:
		None

	Return (uint8_t):
		0 -> Password change successful.
		1 -> Password change canceled.
	*/
	char password[MAX_PASSWORD_LENGTH], buffer[MAX_PASSWORD_LENGTH];
	char md5_hash[33];
	while(1) {
		printf("New Password: ");
		get_password(buffer, MAX_PASSWORD_LENGTH);
		strcpy(password, buffer);
		if (strlen2(buffer) > 0 && strlen2(buffer) <= MAX_PASSWORD_LENGTH){
			printf("Confirm Password: ");
			get_password(buffer, MAX_PASSWORD_LENGTH);
			if(strcmp(password, buffer) == 0) break;
			printf("Password is not the same!\n");
			pause_();
		} else {
			printf("Password too long or empty.\n");
			return 1;
		}
	}
	printf("Passwords equal, proceding with overwrite...\n");
	hash_md5(buffer, md5_hash);
	overwrite_password(USER_DATABASE, CURRENT_LOGIN.name_offset, md5_hash);
	printf("Automaticly reloggin in...\n");  // maoir mentira de sempre, mas pronto
	strcpy(CURRENT_LOGIN.password, md5_hash);
	printf("Update sucessfull!\n");
	pause_();
	return 0;
}

uint8_t add_book(){
	/*
	Function used to add a new book to the system.
	It prompts the user for the book's ISBN, title, and description and creates a new file to store the book's data.
	Then asks the name directly to the file.

	Global variables or macros expected:
		const char* MAIN_LOG		-> Path to the main log file.

	Arguments:
		None

	Return (uint8_t):
		0 -> Book added successfully.
		1 -> Book addition failed (likely due to file open returning NULL).
		2 -> Invalid ISBN provided.
	*/
	// FIXME: To make new interface | PEDRO
	clear_screen();
	char title[MAX_TITLE_LENGTH], caminho[31]="assets/books/1111111111111.csv", description[MAX_TITLE_LENGTH];
	char id_book_str[15];
	
	printf("Insert Book's ISBN: ");
	read_n_chars(14, id_book_str);
	
	if(strlen2(id_book_str)!=13) {printf("Invalid ISBN!\n"); fflush(stdout); pause_(); return 2;};
	strcpy(caminho+13, id_book_str);
	strcpy(caminho+26, ".csv");

	printf("Insert Title: ");
	read_n_chars(MAX_TITLE_LENGTH, title);
	printf("Insert Description: ");
	read_n_chars(MAX_TITLE_LENGTH, description);
	
	FILE* file = fopen(caminho, "a");
    if (file == NULL) return 1;
    fprintf(file, "%05d:%s\n%s\n%05d\n", 0, title, description, 0);
    fclose(file);
    printf("Book registered successfully.\n");
    char buffer[24+15];
	snprintf(buffer, sizeof(buffer),"New book added (ISBN: %s).", id_book_str);
	logc_(MAIN_LOG, buffer);
    pause_();
	return 0;
}

uint8_t remove_book(){
	/*
	Function used to remove a book from the system.
	It prompts the user for the book's ISBN, checks if the book exists
	and deletes the corresponding file.

	Global variables or macros expected:
		const char* MAIN_LOG			-> Path to the main log file.
		const char* BOOK_ARCHIVE_DIR	-> Directory path where book files are stored. MUST INCLUDE THE SLASH ( / )

	Arguments:
		None

	Return (uint8_t):
		0 -> Book removed successfully.
		1 -> Book removal failed (likely due to file not found).
		2 -> Invalid ISBN provided or error deleting the file.
	*/
	// FIXME: To make new interface | PEDRO
	clear_screen();
	char id_book_str[15];
	printf("Insert ISBN Book: ");
	read_n_chars(14, id_book_str);
	if(strlen2(id_book_str)!=13) {printf("Invalid ISBN!\n"); fflush(stdout); pause_(); return 2;};

	uint8_t _temp = 0;
	while(BOOK_ARCHIVE_DIR[_temp] != 0x00) _temp++;
    char file_path[_temp+18];
    snprintf(file_path,sizeof(file_path), "%s%s.csv", BOOK_ARCHIVE_DIR, id_book_str);
    FILE* file = fopen(file_path, "r");
    if (file == NULL) return 1;
	fclose(file);
	if (remove(file_path) != 0) {printf("Error: Unable to delete the file.\n"); pause_(); return 2;}

	char buffer_log[22+15];
	snprintf(buffer_log, sizeof(buffer_log), "Book removed (ISBN: %s).", id_book_str);
	logc_(MAIN_LOG, buffer_log);
	printf("Book removed successfully!\n");
	pause_();
	return 0;
}

uint8_t check_book_info(){
	/*
	Function used to display information about a book.
	It prompts the user for the book's ISBN, retrieves the book's data
	and prints the book's information.

	Global variables or macros expected:
		const char* BOOK_ARCHIVE_DIR	-> Directory path where book files are stored. MUST CONTAIN THE FINAL SLASH ( / )

	Arguments:
		None

	Return (uint8_t):
		0 -> Book information displayed successfully.
		2 -> Invalid ISBN provided or book not found.
	*/
	// FIXME: To make new interface | PEDRO
	clear_screen();
	char id_book_str[15];
	printf("Insert ISBN Book: ");
	read_n_chars(14, id_book_str);
	if(strlen2(id_book_str)!=13){
		printf("ISBN is not valid!\n");
		pause_();
		return 2;
	}
	BOOK* book = get_book_by_id(BOOK_ARCHIVE_DIR, id_book_str);
	if (book==NULL){
		printf("Book not found!\n");
		pause_();
		return 2;
	}
	print_book_data(book);
	delete_linked_list(book->queue_for_students, free);
	free(book);
	pause_();
	return 0;
}

void print_isbn_name(void* a){
	/*
	Function used to print the ISBN and title of a book.
	It takes a void pointer to an unsigned 64-bit integer representing the book's ISBN,
	retrieves the book's title from the corresponding file and prints the ISBN and title.

	Global variables or macros expected:
		const char* BOOK_ARCHIVE_DIR	-> Directory path where book files are stored. MUST CONTAIN THE FINAL SLASH ( / )

	Arguments:
		void* a	-> Void pointer to an unsigned 64-bit (uint64_t) integer representing the book's ISBN.

	Return:
		None
	*/
	// FIXME: To make new interface | PEDRO
	clear_screen();
    uint64_t* book_isbn_int = (uint64_t*)a;
    char buffer[7];
    buffer[6] = 0x00;
    buffer[0] = 0x00;
    while(BOOK_ARCHIVE_DIR[(uint8_t) buffer[0]]!=0x00) buffer[0]++;
    printf("ISBN: " ISBN_FORMAT " -> Tittle: ", *book_isbn_int);
    fflush(stdout);
    char file_path[buffer[0]+18];
    snprintf(file_path, sizeof(file_path), "%s" ISBN_FORMAT ".csv", BOOK_ARCHIVE_DIR, *book_isbn_int);
    FILE* file = fopen(file_path, "rb");
    if(file==NULL){printf("ERROR!\n"); fflush(stdout); return;}
    fseek(file, 6, SEEK_SET);
    int8_t bytesRead;
    while(1){
        bytesRead = fread(buffer, 1, 6, file);
        if (bytesRead == 0) break;
        for (bytesRead--; bytesRead >= 0; bytesRead--) {
            if (buffer[bytesRead] == 0x0A) {
                buffer[bytesRead] = 0x00;
                printf("%s\n", buffer);
                fclose(file);
                return;
            }
        }
        printf("%s", buffer);
    }
}

void list_book_by_ISBN(){
	/*
	Function used to list all books in the system by their ISBN.
	It loads the book IDs from the database, and prints the ISBN and title of each book.
	Thanks to the system returning the file names ordered alphabeticly by default, no ordering has to be performed.
	(Yes, programers are lazy, any news there??)

	Global variables or macros expected:
		const char* BOOK_ARCHIVE_DIR	-> Directory path where book files are stored. MUST CONTAIN THE FINAL SLASH ( / )

	Arguments:
		None

	Return:
		None
	*/
	// FIXME: To make new interface | PEDRO
	clear_screen();
	LinkedList* books;
	printf("Loading database...\n");
	fflush(stdout);
	books = get_book_ids(BOOK_ARCHIVE_DIR);
	if (books == NULL || books->size == 0){
		printf("No books found!\n");
		fflush(stdout);
		pause_();
		return;
	}
	printf("Loading sucessfull. Printing requested data:\n");
	fflush(stdout);
	traverse_list(books, print_isbn_name);
	pause_();
	return;
}

int32_t is_book_available(void* a){
	/*
	Function used to check if a book is available.
	It takes a void pointer to an unsigned 64-bit integer representing the book's ISBN,
	retrieves the book's availability status from the corresponding file and returns the status.

	Global variables or macros expected:
		const char* BOOK_ARCHIVE_DIR	-> Directory path where book files are stored. MUST CONTAIN THE FINAL SLASH ( / )

	Arguments:
		void* a	-> Void pointer to an unsigned 64-bit integer representing the book's ISBN.

	Return (int32_t):
		0 -> Book is available.
		1 -> Book is not available or file is corrupted.
		3 -> Error opening file (likely due to file open returning NULL).
*/
	uint64_t* book_isbn_int = (uint64_t*)a;
    char buffer[6];
    buffer[5] = 0x00;
    buffer[0] = 0x00;
    while(BOOK_ARCHIVE_DIR[(uint8_t) buffer[0]]!=0x00) buffer[0]++;
    char file_path[buffer[0]+18];
    snprintf(file_path, sizeof(file_path), "%s" ISBN_FORMAT ".csv", BOOK_ARCHIVE_DIR, *book_isbn_int);
    FILE* file = fopen(file_path, "rb");
    if(file==NULL){printf("ERROR!\n"); fflush(stdout); return 3;}
	int8_t bytesRead;
	bytesRead = fread(buffer, 1, 5, file);
	fclose(file);
	if (bytesRead == 5) return strcmp(buffer, "00000")==0? 0 : 1;
	printf("Corrupted file detected for ISBN: " ISBN_FORMAT "!\n", *book_isbn_int);
	fflush(stdout);
	return 1;
}
void list_available_books(){
	/*
	Function used to list all available books in the system.
	It loads the book IDs from the database, filters out the unavailable books
	and prints the ISBN and title of each available book.

	Global variables or macros expected:
		const char* BOOK_ARCHIVE_DIR	-> Directory path where book files are stored.

	Arguments:
		None

	Return:
		None
	*/
	// FIXME: To make new interface | PEDRO
	clear_screen();
    LinkedList* books;
    books = get_book_ids(BOOK_ARCHIVE_DIR);
    if (books == NULL || books->size == 0) {
        printf("Nenhum livro disponível encontrado!\n");
        pause_();
        return;
    }
    printf("Livros disponíveis:\n");
	remove_nodes(books, is_book_available, free);
    traverse_list(books, print_isbn_name);
    pause_();
}

int32_t list_books_alphabeticly_key(void* a, void* b){
	/*
		Function used to compare two book titles alphabetically.
		It takes two void pointers to unsigned 64-bit integers representing the books' ISBNs,
		opens both files related to the books, and returns a comparison between the book's names.
		That comparison is made byte by byte.
		WARNING: Due to the nature of byte by byte comparison and no multibyte handling,
		any multi byte char will appear first before any other letter, so ź appears first than a. [ Feature, not bug :) ]

		Global variables or macros expected:
			const char* BOOK_ARCHIVE_DIR	-> Directory path where book files are stored. MUST CONTAIN THE FINAL SLASH ( / )

		Arguments:
			void* a	-> Pointer to an unsigned 64-bit integer representing the first book's ISBN.
			void* b	-> Pointer to an unsigned 64-bit integer representing the second book's ISBN.

		Return (int32_t):
			-1 -> First book's title comes before the second book's title alphabetically.
			 0 -> Both book titles are equal.
			 1 -> First book's title comes after the second book's title alphabetically.
	*/
	FILE* file1;
	FILE* file2;
	char buffer1 = 0x00;
	char buffer2 = 0x00;

	while(BOOK_ARCHIVE_DIR[(uint8_t) buffer1]!=0x00) buffer1++;
	char file_path[buffer1+18];

	snprintf(file_path, sizeof(file_path), "%s%13lu.csv", BOOK_ARCHIVE_DIR, *(uint64_t*) a);
	file1 = fopen(file_path, "rb");
	if(file1==NULL){printf("ERROR!\n"); fflush(stdout); return 0;}
	fseek(file1, 6, SEEK_SET);

	snprintf(file_path, sizeof(file_path), "%s%13lu.csv", BOOK_ARCHIVE_DIR, *(uint64_t*) b);
	file2 = fopen(file_path, "rb");
	if(file2==NULL){printf("ERROR!\n"); fclose(file1); fflush(stdout); return 0;}
	fseek(file2, 6, SEEK_SET);

	while(1) {
        buffer1 = tolower(fgetc(file1));
        buffer2 = tolower(fgetc(file2));
        if (buffer1==0x0A && buffer2==0x0A) return 0;
		if (buffer1 == 0x0A) return 1;
		else if (buffer2 == 0x0A) return -1;
        if (buffer1 != buffer2) return buffer1 - buffer2;
    }
}
void list_books_alphabeticly(){
	/*
	Function used to list all books in the system in alphabetical order by title.
	It loads the book IDs from the database (directory, more acuratly), sorts the list alphabetically by title
	and prints the ISBN and title of each book.

	Global variables or macros expected:
		const char* BOOK_ARCHIVE_DIR	-> Directory path where book files are stored. MUST CONTAIN THE FINAL SLASH ( / )

	Arguments:
		None

	Return:
		None
	*/
	// FIXME: To make new interface | PEDRO
	clear_screen();
	LinkedList* all_isbns = get_book_ids(BOOK_ARCHIVE_DIR);
	three_way_quick_sort(all_isbns, list_books_alphabeticly_key);
	traverse_list(all_isbns, print_isbn_name);
	pause_();
}

int32_t is_current(void* a){
	/*
	Function used to check if a user's uID matches the current loged-in user's uID.
	It takes a void pointer to a 32-bit integer representing the user's uID,
	and returns a comparison result.

	Global variables or macros expected:
		ACCOUNT CURRENT_LOGIN	-> Currently logged-in user. Only the uID (in .uid) field is used.

	Arguments:
		void* a	-> Pointer to a 32-bit integer representing the user ID.

	Return (int32_t):
		Comparison user's uID minus current loged-in user's uID plus one.
	*/
	return *(uint32_t*) a - CURRENT_LOGIN.uid + 1;
}
void checkout_book(){
	/*
		Function used to checkout a book.
		It prompts the user for the book's ISBN, checks the book's availability
		and if the book is not available adds to the queue, after confirming the user is not already there.

		Global variables or macros expected:
			ACCOUNT CURRENT_LOGIN			-> Currently logged-in user. Only the uID (in .uid) field is used.
			const char* BOOK_ARCHIVE_DIR	-> Directory path where book files are stored. MUST CONTAIN THE FINAL SLASH ( / )
			const char* MAIN_LOG			-> Path to the main log file.

		Arguments:
			None

		Return:
			None
	*/
	// FIXME: To make new interface | PEDRO
	clear_screen();
	char id_book_str[15];
	printf("Insert ISBN Book: ");
	read_n_chars(14, id_book_str);
	if(strlen2(id_book_str)!=13){
		printf("ISBN is not valid!\n");
		pause_();
		return;
	}
	BOOK* book = get_book_by_id(BOOK_ARCHIVE_DIR, id_book_str);
	if (book == NULL){
		printf("Book not found!\n");
		pause_();
		return;
	}
	if (book->requested_by == 0){
		while (BOOK_ARCHIVE_DIR[(uint8_t) book->requested_by] != 0x00) book->requested_by++;
		char file_path[(uint8_t) book->requested_by + 18]; // 18 = 13 + 5 + .csv + \0
		snprintf(file_path, sizeof(file_path), "%s" ISBN_FORMAT ".csv", BOOK_ARCHIVE_DIR, book->uid);
		FILE* file = fopen(file_path, "r+");
		if (file == NULL) {
			printf("Book file not found!\n");
			delete_linked_list(book->queue_for_students, free);
			free(book);
			pause_();
			return;
		}
		fprintf(file, "%u", CURRENT_LOGIN.uid);
		fclose(file);
		printf("Book requested sucessfully!\n");
		log_action(book, MAIN_LOG, "REQ");
		delete_linked_list(book->queue_for_students, free);
		free(book);
		pause_();
		return;
	}
	else if (book->requested_by == CURRENT_LOGIN.uid){
		printf("Book is already checked out by current user!\n");
		fflush(stdout);
		delete_linked_list(book->queue_for_students, free);
		free(book);
		pause_();
		return;
	}
	else { // if user is not checking the book, and current is not null, then let's check queue
		int32_t in_queue = find_node(book->queue_for_students, is_current);
		if (in_queue >= 0){
			printf("User is already in queue for this book!\n");
			fflush(stdout);
			delete_linked_list(book->queue_for_students, free);
			free(book);
			pause_();
			return;
		}
		while (BOOK_ARCHIVE_DIR[(uint8_t) book->requested_by] != 0x00) book->requested_by++;
		char file_path[(uint8_t) book->requested_by + 18]; // 18 = 13 + 5 + .csv + \0
		snprintf(file_path, sizeof(file_path), "%s" ISBN_FORMAT ".csv", BOOK_ARCHIVE_DIR, book->uid);
		FILE* file = fopen(file_path, "r+");
		if (file == NULL) {
			printf("Book file not found!\n");
			delete_linked_list(book->queue_for_students, free);
			free(book);
			pause_();
			return;
		}
		fseek(file, 0, SEEK_END);  // 9789727221561
		if (book->queue_for_students->size == 0) fprintf(file, "%u\n", CURRENT_LOGIN.uid);
		else { fseek(file, -1, SEEK_CUR); fprintf(file, ":%u\n", CURRENT_LOGIN.uid);}
		fclose(file);
		printf("Book already requested by another user!\nCurrent user added to request queue sucessfully!\n");
		log_action(book, MAIN_LOG, "QUE");
		delete_linked_list(book->queue_for_students, free);
		free(book);
		pause_();
		return;
	}
	delete_linked_list(book->queue_for_students, free);
    free(book);
	pause_();
	return;
}

void return_book(){
	/*
	Function used to return a book to the library.
	It prompts the user for the book's ISBN, checks the book's request status,
	removing the request status and if needed 'giving' to the first person on queue.

	Global variables or macros expected:
		ACCOUNT CURRENT_LOGIN			-> Currently logged-in user.
		const char* BOOK_ARCHIVE_DIR	-> Directory path where book files are stored. MUST CONTAIN THE FINAL SLASH ( / )
		const char* MAIN_LOG			-> Path to the main log file.
		const char* HIST_LOG			-> Path to the history log file.

	Arguments:
		None

	Return:
		None
	*/
	// FIXME: To make new interface | PEDRO
	clear_screen();
	char id_book_str[15];
	printf("Insert ISBN Book: ");
	read_n_chars(14, id_book_str);
	if(strlen2(id_book_str)!=13){
		printf("ISBN is not valid!\n");
		pause_();
		return;
	}
	BOOK* book = get_book_by_id(BOOK_ARCHIVE_DIR, id_book_str);
	if (book == NULL){
		printf("Book not found!\n");
		pause_();
		return;
	}
	if (book->requested_by == 0){
		printf("Book not currently requested!\n");
		delete_linked_list(book->queue_for_students, free);
		free(book);
		pause_();
		return;
	}
	uint32_t _temp = CURRENT_LOGIN.uid;
	CURRENT_LOGIN.uid = book->requested_by;
	char buffer[15+6];
	snprintf(buffer, sizeof(buffer),"User %d logd out.", CURRENT_LOGIN.uid);
	log_action(book, MAIN_LOG, "RET");
	log_action(book, HIST_LOG, "RET");

	book->requested_by = 0;
	while (BOOK_ARCHIVE_DIR[(uint8_t) book->requested_by] != 0x00) book->requested_by++;
	char file_path[(uint8_t) book->requested_by + 18]; // 18 = 13 + 5 + .csv + \0
	book->requested_by = CURRENT_LOGIN.uid;
	CURRENT_LOGIN.uid = _temp;

	snprintf(file_path, sizeof(file_path), "%s" ISBN_FORMAT ".csv", BOOK_ARCHIVE_DIR, book->uid);
	FILE* file = fopen(file_path, "r+");
	if (file == NULL) {
		printf("Book file not found!\n");
		delete_linked_list(book->queue_for_students, free);
		free(book);
		pause_();
		return;
	}

	if (book->queue_for_students->size != 0){
		fseek(file, 0, SEEK_SET);  // 9789727221561
		fprintf(file, "%05d", *( (uint32_t*) book->queue_for_students->head->data));

		fseek(file, book->queue_offset, SEEK_SET);
		shift_bytes_up(file, 6);
		end_file(file);
		fclose(file);

		_temp = CURRENT_LOGIN.uid;
		CURRENT_LOGIN.uid = *( (uint32_t*) book->queue_for_students->head->data);

		printf("Book returned sucsessfully!\nFirst student on waiting list is now with the book!\n");
		log_action(book, MAIN_LOG, "DQU");

		CURRENT_LOGIN.uid = _temp;

		delete_linked_list(book->queue_for_students, free);
		free(book);
		pause_();
		return;
	}
	else { // there is not queue
		fseek(file, 0, SEEK_SET);  // 9789727221561
		fprintf(file, "00000");
		fclose(file);
		printf("Book returned sucsessfully!\n");
		delete_linked_list(book->queue_for_students, free);
		free(book);
		pause_();
		return;
	}
	fclose(file);
	delete_linked_list(book->queue_for_students, free);
    free(book);
	pause_();
	return;
}

void change_name(){
	/*
		Function used to change the name of the currently logged-in user.
		It updates the user's name in the user database file directly.

		Global variables or macros expected:
			ACCOUNT CURRENT_LOGIN		-> Currently logged-in user. Must be up to date, as many fields are used.
			const char* USER_DATABASE	-> Path to the user database file.
			uint8_t MAX_NAME_LENGHT		-> Maximum length of a user name.

		Arguments:
			None

		Return:
			None
	*/
	// FIXME: To make new interface | PEDRO
	clear_screen();
	printf("Loading database for update...\n");
	fflush(stdout);
	FILE* file = fopen(USER_DATABASE, "r+");
    if (file == NULL) return;
    printf("Searching for user regist...\n");
	fflush(stdout);
    uint32_t beggining_of_line = CURRENT_LOGIN.name_offset - 41;
	uint8_t buffer[6];
	buffer[5] = 0x00;
	uint8_t bytesRead;
	fseek(file, CURRENT_LOGIN.name_offset, SEEK_SET);
	while (buffer[5] != 0xFF) {
		bytesRead = fread(buffer, 1, 5, file);
		if (bytesRead == 0) break;
		for (uint8_t i = 0; i < bytesRead; i++) {
			if (buffer[i] == '\n') {
				fseek(file, -1 * (bytesRead-i), SEEK_CUR);
				buffer[5] = 0xFF;
				break;
			}
		}
	}
	uint32_t offset = ftell(file) - beggining_of_line;
    printf("Reseting user regist...\n");
   	fflush(stdout);
	fseek(file, beggining_of_line, SEEK_SET);
	shift_bytes_up(file, offset+1); // + \n
	end_file(file);
	printf("Reseting user regist...\n");
	fflush(stdout);
	fprintf(file, "%05d:%s:%1u:", CURRENT_LOGIN.uid, CURRENT_LOGIN.password, CURRENT_LOGIN.type);  // 0 should be 'account_type'
    CURRENT_LOGIN.name_offset = ftell(file);
    fclose(file);
	printf("\nPlease enter your new name now: ");
	fflush(stdout);
	read_text_and_append_to_file(USER_DATABASE, MAX_NAME_LENGHT, 1);
    printf("\nAccount registered successfully.\n");
	fflush(stdout);
	pause_();
	return;
}

void print_return_history(){
	/*
		Function used to print the book return history.
		It reads the history log file and displays its contents.

		Global variables or macros expected:
			const char* HIST_LOG	-> Path to the history log file.

		Arguments:
			None

		Return:
			None
	*/
	// FIXME: To make new interface | PEDRO
	clear_screen();
	printf("Loading book return history...\n");
	FILE* file = fopen(HIST_LOG, "r");
	if (file == NULL){
		printf("Return history log file not found.\n");
		pause_();
		return;
	}
	char buffer[21];
	buffer[20] = 0x00;
	if (fread(buffer, 1, 20, file) == 0){
		printf("No history to show!\n");
		fclose(file);
		pause_();
		return;
	}
	printf("Current history:\n");
	printf("%s", buffer);
    while (fread(buffer, 1, 20, file) != 0) printf("%s", buffer);
    fclose(file);
    pause_();
    return;
}

void remove_old_history_entries() {
	/*
		Function used to remove old history entries from the history log file.
		It prompts the user for the number of entries to remove and
		removes the specified number of entries from the beginning of the file, unless
		not enough line are present.

		Global variables or macros expected:
			const char* HIST_LOG	-> Path to the history log file.

		Arguments:
			None

		Return:
			None
	*/
	// FIXME: To make new interface | PEDRO
	clear_screen();
    char buffer[21];
    uint16_t how_many_to_remove;

    printf("How many entries do you wish to remove? ");
    read_n_chars(20, buffer);
    how_many_to_remove = (uint16_t) str_to_int64_t_flag(buffer, (uint8_t*) &(buffer[20]));
	if (buffer[0] == 0){
		printf("Invalid number of entries to remove!");
		pause_();
		return;
	}
	buffer[20] = 0x00;

    FILE* file = fopen(HIST_LOG, "r+");
	if (file == NULL){
		printf("Return history log file not found.\n");
		pause_();
		return;
	}

	if (fread(buffer, 1, 20, file) == 0){
		printf("No history to remove!\n");
		fclose(file);
		pause_();
		return;
	}

	uint8_t bytesRead;
	fseek(file, CURRENT_LOGIN.name_offset, SEEK_SET);
	how_many_to_remove--;
	while ((uint8_t) buffer[5] != 0xFF) {
		bytesRead = fread(buffer, 1, 5, file);
		if (bytesRead == 0) break;
		for (uint8_t i = 0; i < bytesRead; i++) {
			if (buffer[i] == '\n') {
				fseek(file, (-1 * (bytesRead-i))+1, SEEK_CUR);
				how_many_to_remove--;
				if (how_many_to_remove == 0) buffer[5] = 0xFF;
			}
		}
	}
	if(how_many_to_remove != 0){
		printf("There are not enough history entries to cover such a large number!\nPlease try a lower one.\n");
        pause_();
        return;
	}
	uint32_t offset = ftell(file);
	printf("Proceding with removal...\n");
	fseek(file, 0, SEEK_SET);
	shift_bytes_up(file, offset);
	end_file(file);
	fclose(file);
	printf("Done!\n");
	pause_();
	return;
}

// MENUS
void mng_student_account(){
	/*
	Procedimento para criar e correr o menu de gestão dos alunos
	Argumentos:
		SELF* self -> Struct do tipo _self que contém o contexto de chamada.
	Retorno:
		Nenhum
	*/
	uint8_t _escolha_menu;
	while (1){
		clear_screen();
		_escolha_menu = menu("MANAGE ACCOUNT", CABECALHO_LEN, mng_student_account_menu, len_mng_student_account_menu, 1);
		if(_escolha_menu==0) break;
		clear_screen();
		switch(_escolha_menu){
			case 1: change_name(); break;
			case 2: change_password(); break;
			default: printf("\nFunção ainda não implementada!!\n"); pause_();
		}
	}
	return;
}
void student_account(){
	/*
	Procedimento para criar e correr o menu de gestão dos alunos
	Argumentos:
		SELF* self -> Struct do tipo _self que contém o contexto de chamada.
	Retorno:
		Nenhum
	*/
	uint8_t _escolha_menu;
	while (1){
		clear_screen();
		_escolha_menu = menu("MAIN MENU ", CABECALHO_LEN, student_account_menu, len_student_account_menu, 1);
		if(_escolha_menu==0){
			clear_screen();
			_escolha_menu = confirmation_with_cabecalho("LOGOUT", "Are you sure you want to log out?", CABECALHO_LEN);
			if(_escolha_menu==0) {
				logout();
				return;
			};
		} else
		switch(_escolha_menu){
			case 1: checkout_book(); break;
			case 2: list_book_by_ISBN(); break;
			case 3: list_books_alphabeticly(); break;
			case 4: list_available_books(); break;
			case 5: check_book_info(); break;
			case 6: mng_student_account(); break;
			default: printf("\nFunção ainda não implementada!!\n"); pause_();
		}
	}
	return;
}

void mng_biblman_account(){
	/*
	Procedimento para criar e correr o menu de gestão dos instrutores
	Argumentos:
		SELF* self -> Struct do tipo _self que contém o contexto de chamada.
	Retorno:
		Nenhum
	*/
	uint8_t _escolha_menu;
	while (1){
		clear_screen();
		_escolha_menu = menu("MANAGE SYSTEM ", CABECALHO_LEN, mng_biblman_account_menu, len_mng_biblman_account_menu, 1);
		if(_escolha_menu==0) break;
		switch(_escolha_menu){
			case 1: add_book(); break;
			case 2: remove_book(); break;
			case 3: print_return_history(); break;
			case 4: remove_old_history_entries(); break;
			case 5: regist(); break;
			case 6: reset_password(); break;
			case 7: change_password(); break;
			default: printf("\nFunção ainda não implementada!!\n"); pause_();
		}
	}
	return;
}
void biblman_account(){
	/*
	Procedimento para criar e correr o menu de gestão dos instrutores
	Argumentos:
		SELF* self -> Struct do tipo _self que contém o contexto de chamada.
	Retorno:
		Nenhum
	*/
	uint8_t _escolha_menu;
	while (1){
		clear_screen();
		_escolha_menu = menu("MAIN MENU ", CABECALHO_LEN, biblman_account_menu, len_biblman_account_menu, 1);
		if(_escolha_menu==0){
			clear_screen();
			_escolha_menu = confirmation_with_cabecalho("LOGOUT", "Are you sure you want to log out?", CABECALHO_LEN);
			if(_escolha_menu==0) {
				logout();
				return;
			};
		} else
		switch(_escolha_menu){
			case 0: break;
			case 1: return_book(); break;
			case 2: list_book_by_ISBN(); break;
			case 3: list_books_alphabeticly(); break;
			case 4: list_available_books(); break;
			case 5: check_book_info(); break;
			case 6: mng_biblman_account(); break;
			default: printf("\nFunção ainda não implementada!!\n"); pause_();
		}
	}
	return;
}

void handle_sigint_temp(int32_t sig){
	// simple intermidiary for the normal handler, just for loggin purposes.
	char buffer[35+12];
	snprintf(buffer, sizeof(buffer),"Received control interrupt signal %d.", sig);
	log_(MAIN_LOG, buffer);
	handle_sigint(sig);
}
int32_t main(void){
	/*
	Função primária do programa
	Argumentos:
		Nenhum
	Retorno:
		Nenhum
	*/
	#ifdef AGGRESSIVE
		signal(SIGINT, handle_sigint_temp);
		disable_ctrl_d();
	#endif
    fflush(stdin);
	uint8_t escolha_menu;
	log_(MAIN_LOG, "Program initiated.");
	while (1){
		clear_screen();
		escolha_menu = menu("PLEASE CHOOSE ACCOUNT TYPE", CABECALHO_LEN, main_menu, len_main_menu, 1);
		if(escolha_menu==0) break;
		switch(escolha_menu){
			case 1: if(login(1)==0) biblman_account(); break;
			case 2: if(login(0)==0) student_account(); break;
			default: printf("\nFunção ainda não implementada!!\n");
		}
	}
	printf("Saindo...\n");
	#ifdef AGGRESSIVE
		enable_ctrl_d();
	#endif
	log_(MAIN_LOG, "Program finished sucsesfully.");
	return 0;
}
