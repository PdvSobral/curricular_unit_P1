/* Encoding: UTF-8
@Authors: Pedro Sobral (33641), Alexandre Domingos (27641), Diogo Meneses (33826)
@Date: 29/05/2025
@Links: Project github repository -> https://github.com/PdvSobral/curricular_unit_P1

This program was developed as an answer to a problem described in a pdf file provided to us.
The pdf is present in the same repository as this program, just not now.
For now, it's just an adaptation in progress of another program.
*/
#define __main__	// because of it is the first, the other will not compile if not this line

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <ctype.h>
#include <stdint.h>
#include <termios.h>    // -> Disabl_ctrl_d (IN) functions.c
#include <signal.h>		// -> To remap CTRL+C
#include <string.h>		// strcmp, strlens
#include <unistd.h>		// sleep, STDIN_FILENO
#include <dirent.h>		// for directory listing
#include <inttypes.h>	// for string formating

#ifndef psystem
	#pragma GCC warning "Loaded standard modules. Please use strlen2 instead of strlen."
#else
	#include "good_practices.c"
	// Basicly imports the four modules above
	// Makes so that int, unsigned and other lables/macros are not usable
	#pragma GCC warning "Loaded custom module."
#endif

#include "functions.c"
#include "linked_lists.c"
#include "database_helper.c"
#include "typedefs.c"
#include "md5.c"

#define AGGRESSIVE
#define MAX_PASSWORD_LENGTH 30
#define MAX_TITLE_LENGTH 	100
#define MAX_NAME_LENGHT 	70

#ifndef ISBN_FORMAT
// TODO: Implement this in all ISBN printings and uses.
#define ISBN_FORMAT "%013" PRIu64
#endif

const char* USER_DATABASE = "./assets/sys_shadow.csv";
const char* BOOK_ARCHIVE_DIR = "./assets/books/"; // MUST INCLUDE THE SLASH (/), parts of the code and buffers depend on that
const char* MAIN_LOG = "./assets/main.log";
const char* HIST_LOG = "./assets/history.log";
static ACCOUNT CURRENT_LOGIN = {0, 999999999, 2, ""};

// Defenition of the menu arrays
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
	"Check return history",
	"Remove old return history entries",
	"Create new account",
	"Reset password to an account",
	"Change current account password",
	"Return to main menu"
};

// FUNÇÕES

uint8_t log_action(BOOK* book, const char* log_file_path, const char mode[4]){
	/*
	return:
		0 -> Correcto!
		1 -> File log NULL
		2 -> File book NULL
		3 -> Not suposed to get to the end, flux was broken
	*/
    FILE* file = fopen(log_file_path, "at");
    if (file == NULL) return 1;

    // Escreve data e UID do usuário
    DATE date = get_current_date(date);
    TIME times = get_current_time(times);
    // [*] 2025-05-25 14:54 REQ uID_Aluno NomeLivro
    fprintf(file, "[*] %04d-%02d-%02d %02d:%02d %s %d ", date.year, date.month, date.day, times.hour, times.minutes, mode, CURRENT_LOGIN.uid);

    // Determina o tamanho do nome de pasta (até encontrar 0x00)
    char buffer[7];
    buffer[6] = 0x00;
    buffer[0] = 0x00;
    while ((uint8_t)BOOK_ARCHIVE_DIR[(uint8_t)buffer[0]] != 0x00) buffer[0]++;

    // Cria o caminho do arquivo original
    char file_path2[buffer[0] + 18];
    snprintf(file_path2, sizeof(file_path2), "%s" ISBN_FORMAT ".csv", BOOK_ARCHIVE_DIR, book->uid);

    // Abre o segundo arquivo (arquivo de onde o nome do livro serás lido)
    FILE* file2 = fopen(file_path2, "rb");
    if (file2 == NULL) {
        fclose(file);
        printf("ERROR OPENING BOOK FILE!\n");
        fflush(stdout);
        return 2;
    }

    int8_t bytesRead;
    fseek(file2, 6, SEEK_SET);
    while (1) {
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
	return:
		0 -> Correcto!
		1 -> File log NULL
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
	return:
		0 -> Correcto!
		1 -> File log NULL
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
	char account_id[6];
	printf("ID Account: ");
	read_n_chars(5, account_id);
	ACCOUNT* user=get_user_by_id(USER_DATABASE, account_id);
	overwrite_password(USER_DATABASE, user->name_offset, "5f4dcc3b5aa765d61d8327deb882cf99");
	free(user);
}

uint8_t change_password(){
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
	// FIXME: To make new interface | PEDRO
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

int32_t list_books_alphabeticly_key(void* a, void* b){ // receives uint64*
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
	LinkedList* all_isbns = get_book_ids(BOOK_ARCHIVE_DIR);
	three_way_quick_sort(all_isbns, list_books_alphabeticly_key);
	traverse_list(all_isbns, print_isbn_name);
	pause_();
}

int32_t is_current(void* a){
	return *(uint32_t*) a - CURRENT_LOGIN.uid + 1;
}
void checkout_book(){
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
    fclose(file);
	printf("\nPlease enter your new name now: ");
	fflush(stdout);
	read_text_and_append_to_file(USER_DATABASE, MAX_NAME_LENGHT, 1);
    printf("\nAccount registered successfully.\n");
	fflush(stdout);
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
			// TODO: case 3) Check return history | Any + Logging (MacUser)
			// TODO: case 4) Remove old return history entries | Any + Logging (MacUser)
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
