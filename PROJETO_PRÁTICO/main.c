/* Encoding: UTF-8
@Authors: Pedro Sobral (33641), Alexandre Domingos (27641), Diogo Meneses (TODO: Diogo, põe o teu número aqui)
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

uint8_t login(uint8_t account_flag_type){
	char buffer[26];
	uint8_t flag=0;
	uint32_t account_id;
	char account_id_str[6];
	char md5_hash[33];
	while (1){
		printf("Account ID: ");
		read_n_chars(6, buffer);
		account_id = (int64_t) str_to_int64_t_flag(buffer, &flag);
		if (flag==1 && account_id <= 99999 && strlen2(buffer)==5) break;
		printf("Invalid ID!\n");
		pause_();
		return 1;  // TODO: Adicionar confirmação se quer reintroduzir ou voltar ao menu inicial
	} strcpy(account_id_str, buffer);
	while (1){
		printf("Password: ");
		get_password(buffer, MAX_PASSWORD_LENGTH);
		if (strlen2(buffer) <= MAX_PASSWORD_LENGTH) break;
		printf("Invalid password type for system!\n");
		pause_();
		return 1;  // TODO: Adicionar confirmação se quer reintroduzir ou voltar ao menu inicial
	}
	ACCOUNT* my_user = get_user_by_id(USER_DATABASE, account_id_str);
	hash_md5(buffer, md5_hash);
	if (my_user == NULL || my_user->type == account_flag_type || strcmp(my_user->password, md5_hash)!=0){
		free(my_user);
		printf("INVALID CREDENTIALS!\n");
		pause_();
		return 1;
	}
	// else print_account_data(my_user);
	// TODO: De alguma forma registar a conta "logada" atualmente
	printf("Login Sucessfull as '");
	print_name(USER_DATABASE, my_user->name_offset);
	printf("'!\n");
	free(my_user);
	pause_();
	return 0;  // sucessfull
}

uint8_t regist() {
    char buffer[64];
    char account_id_str[6];
    char md5_hash[33];
    char account_type_str[2];
	char password1[MAX_PASSWORD_LENGTH + 1];
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
			printf("Pass1: %s, Pass2: %s\n", password1, buffer);
			if(strcmp(password1, buffer) == 0) break;
			printf("Password is not the same!\n");
			pause_();
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
        buffer[1] = '\0';
        if (buffer[0] == '0' || buffer[0] == '1') {
            strcpy(account_type_str, buffer);
            break;
        }
        printf("Invalid account type.\n");
    }
    // 4. Nome completo
    printf("Full name: ");
    fgets(name, LEN_NAME, stdin);
    name[strcspn(name, "\n")] = 0;  // remover newline

    // 5. Escrever no CSV
    FILE* fp = fopen("assets/sys_shadow.csv", "a");
    if (!fp) {
        perror("Erro ao abrir ficheiro");
        return 1;
    }

    fprintf(fp, "%05ld:%s:%c:%s\n", account_id, md5_hash, *account_type_str, name);
    fclose(fp);

    printf("Account registered successfully.\n");
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
		if(_escolha_menu==0) break;
		clear_screen();
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
		if(_escolha_menu==0) break;
		switch(_escolha_menu){
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
