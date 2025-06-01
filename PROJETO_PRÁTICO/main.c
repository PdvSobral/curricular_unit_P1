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
const char* USER_DATABASE = "./assets/sys_shadow.csv";
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
const uint8_t len_mng_biblman_account_menu = 7;
const char mng_biblman_account_menu[][CABECALHO_LEN] = {
	"Add book",
	"Remove book",
	"Check return history",
	"Remove old return history entries",
	"Create new account",
	"Reset password to an account",
	"Return to main menu"
};

// FUNÇÕES
void print_account_data(ACCOUNT* data){
		printf("uID: %d\n", data->uid);
		printf("MD5: %s\n", data->password);
		printf("Type: %s\n", data->type==1?"Librarian":"Student");
		printf("Name Offset: %d\n", data->name_offset);
	}

uint8_t login2(uint8_t account_flag_type){
	char buffer[26];
	uint8_t flag=0;
	uint32_t account_id;
	char account_id_str[6];
	char md5_hash[33];
	char header[CABECALHO_LEN];
	char header2[CABECALHO_LEN+1]; // to shut up a warning
	while (1){
		clear_screen();
		snprintf(header, sizeof(header), "LOGIN AS %s", account_flag_type == 0 ? "LIBRARIAN" : "STUDENT");
		cabecalho(header, CABECALHO_LEN);
		reset_line(CABECALHO_LEN);
		print_between("Account ID: ", CABECALHO_LEN, 1);
		print_bottom(CABECALHO_LEN, 0);
		printf("\033[%uD\033[1A", CABECALHO_LEN - 14);
		read_n_chars(6, buffer);
		account_id = (int64_t) str_to_int64_t_flag(buffer, &flag);
		if (flag==1 && account_id <= 99999 && strlen2(buffer)==5) break;
		clear_screen();
		snprintf(header, sizeof(header), "INVALID ID '%s'!", buffer);
		if (strlen2(header) % 2 != 0) snprintf(header2, sizeof(header2), "%s ", header);
		else strcpy(header2, header);
		account_id = confirmation_with_cabecalho(header2, "Do you wish to input a new ID?", CABECALHO_LEN);
		if(!(account_id==0)) return 1;
	} strcpy(account_id_str, buffer);
	while (1){
		clear_screen();
		snprintf(header, sizeof(header), "LOGIN AS %s", account_flag_type == 0 ? "LIBRARIAN" : "STUDENT");
		cabecalho(header, CABECALHO_LEN);
		reset_line(CABECALHO_LEN);
		snprintf(header, sizeof(header), "Account ID: %s", account_id_str);
		print_between(header, CABECALHO_LEN, 1);
		print_between("Password: ", CABECALHO_LEN, 1);
		print_bottom(CABECALHO_LEN, 0);
		printf("\033[%uD\033[1A", CABECALHO_LEN - 12);
		get_password(buffer, MAX_PASSWORD_LENGTH);
		if (strlen2(buffer) <= MAX_PASSWORD_LENGTH) break;
		clear_screen();
		account_id = confirmation_with_cabecalho("INVALID PASSWORD TYPE FOR SYSTEM", "Do you wish to re-input the password?", CABECALHO_LEN);
		if(!(account_id==0)) return 1;
	}
	ACCOUNT* my_user = get_user_by_id(USER_DATABASE, account_id_str);
	hash_md5(buffer, md5_hash);
	if (my_user == NULL || my_user->type == account_flag_type || strcmp(my_user->password, md5_hash)!=0){
		free(my_user);
		print_middle(CABECALHO_LEN, 1);
		print_between_format("INVALID CREDENTIALS!", "\033[31m", CABECALHO_LEN, 1);
		print_bottom(CABECALHO_LEN, 1);
		pause_();
		return 1;
	}
	/*
	printf("Login Sucessfull as '");
	print_name(USER_DATABASE, my_user->name_offset);
	printf("'!\n");
	*/
	print_middle(CABECALHO_LEN, 1);
	print_between_format("LOGIN SUCESSFUL!", "\033[32m", CABECALHO_LEN, 1);
	print_bottom(CABECALHO_LEN, 1);
	CURRENT_LOGIN = *my_user;
	free(my_user);
	pause_();
	return 0;  // sucessfull
}

uint8_t login(uint8_t account_flag_type){
	clear_screen();
	cabecalho("LOGIN MENU", CABECALHO_LEN);
	reset_line(CABECALHO_LEN);
	printf("│ [*] Account ID  :");
	printf("\033[%uC│\n", CABECALHO_LEN - 20);
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
	printf("\033[4A\033[999D\033[20C");
	fflush(stdout);
	uint8_t state=0; //0 - account is  | 1 - password id   | login 'button'
	uint8_t temp;
	char ch;
	char account_id[6]; account_id[0] = 0x00;
	char password[MAX_PASSWORD_LENGTH + 1]; password[0] = 0x00;
	char md5_hash[33];
	while (1){
		// chars especiais tipo setas
		if ((ch = getch()) == 27) {
			if ((ch = getch()) == 91) {
				if ((ch = getch()) == 65 && state != 0){  // up arrow pressed
					// printf("↑");
					switch(state){
						case 1: printf("\033[999D\033[3C \033[1A\033[5D\033[3C*\033[%dC", 16 + strlen2(account_id)); break;
						case 2: printf(" \033[1D\033[2A*\033[%dC", 12 + strlen2(password)); break;
						case 3: printf(" \033[13D*\033[1D"); break;
					}
					state--;
				}
				else if (ch == 66 && state != 3){		 // down arrow pressed
					// printf("↓");
					switch(state){
						case 0: printf("\033[999D\033[3C \033[1B\033[5D\033[3C*\033[%dC", 12 + strlen2(password)); break;
						case 1: printf("\033[999D\033[3C \033[2B\033[5D\033[3C*\033[1D"); break;
						case 2: printf(" \033[11C*\033[1D"); break;
					}
					state++;
				}
			}
		}
		else {
			if (state == 0 && strlen2(account_id) <= 5){
				// printf("%d", ch);
				if ((ch == 0x08 || ch == 0x7F) && strlen2(account_id) != 0){   //  Delete and backspace
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
				if ((ch == 0x08 || ch == 0x7F) && strlen2(password) != 0){   //  Delete and backspace
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
								if (my_user == NULL || my_user->type == account_flag_type || strcmp(my_user->password, md5_hash)!=0){
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
	CURRENT_LOGIN.uid = 0;
	CURRENT_LOGIN.name_offset = 999999999;
	CURRENT_LOGIN.type = 2;
	strcpy(CURRENT_LOGIN.password, "");
	return;
}

uint8_t regist() {
    char buffer[LEN_NAME];		// string maior a ser lida
    char account_id_str[6];
	char password1[MAX_PASSWORD_LENGTH + 1];
    char md5_hash[33];
    uint8_t account_type;
	// TODO: Add confirmations to retype or return to menu.
    char name[LEN_NAME + 1];
    uint8_t flag;
    int64_t account_id;
    // 1. Account ID
    while (1) {
        printf("Account ID (5 digits): ");
        read_n_chars(5, buffer);
        buffer[5] = '\0';
        account_id = str_to_int64_t_flag(buffer, &flag);
        if (strlen2(buffer) != 5 || flag != 1) {
            printf("Invalid ID!\n");
            return 1;
        }
		ACCOUNT* my_user = get_user_by_id(USER_DATABASE, buffer);
		if(my_user == NULL){
			strcpy(account_id_str, buffer);
			free(my_user);
        	break;
		}
		printf("Account ID already exists!\n");
        free(my_user);
    }
    // 2. Password
    while (1) {
        printf("Password: ");
        get_password(buffer, MAX_PASSWORD_LENGTH);
		strcpy(password1, buffer);
        if (strlen2(buffer) > 0 && strlen2(buffer) <= MAX_PASSWORD_LENGTH){
			printf("Confirm Password: ");
        	get_password(buffer, MAX_PASSWORD_LENGTH);
			if(strcmp(password1, buffer) == 0) break;
			printf("Password is not the same!\n");
			return 1;
		}
		printf("Password too long or empty.\n");
        return 1;
	}
	hash_md5(buffer, md5_hash);
    // 3. Tipo de Conta
    while (1) {
        printf("Choose account type (Librarian: 1, Student: 0): ");
        read_n_chars(1, buffer);
        account_type = buffer[0] - 0x30;
        if (account_type == 0 || account_type == 1) break;
        printf("Invalid account type.\n");
    }
    // 4. Nome completo
    printf("Full name: ");
    read_n_chars(LEN_NAME, name);
    // 5. Escrever no CSV
    FILE* file = fopen("assets/sys_shadow.csv", "a");
    if (file == NULL) return 1;
    fprintf(file, "%05ld:%s:%1u:%s\n", account_id, md5_hash, account_type, name);
    fclose(file);
    printf("Account registered successfully.\n");
    pause_();
    return 0;
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
			default: printf("\nFunção ainda não implementada!!\n");
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
			case 6: mng_student_account(); break;
			default: printf("\nFunção ainda não implementada!!\n");
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
			case 5: regist(); break;
			default: printf("\nFunção ainda não implementada!!\n");
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
			case 6: mng_biblman_account(); break;
			default: printf("\nFunção ainda não implementada!!\n");
		}
	}
	return;
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
		signal(SIGINT, handle_sigint);
		disable_ctrl_d();
	#endif
    fflush(stdin);
	uint8_t escolha_menu;

	while (1){
		clear_screen();
		escolha_menu = menu("PLEASE CHOOSE ACCOUNT TYPE", CABECALHO_LEN, main_menu, len_main_menu, 1);
		if(escolha_menu==0) break;
		switch(escolha_menu){
			case 1: if(login(0)==0) biblman_account(); break;
			case 2: if(login(1)==0) student_account(); break;
			default: printf("\nFunção ainda não implementada!!\n");
		}
	}
	printf("Saindo...\n");
	#ifdef AGGRESSIVE
		enable_ctrl_d();
	#endif
	return 0;
};
