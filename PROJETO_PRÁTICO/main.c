/* Encoding: UTF-8
@Authors: Pedro Sobral (33641), Alexandre Domingos (27641), Diogo Meneses (TODO: Diogo, põe o teu número aqui)
@Date: 29/05/2025
@Links: Project github repository -> https://github.com/PdvSobral/curricular_unit_P1

This program was developed as an answer to a problem described in a pdf file provided to us.
The pdf is present in the same repository as this program, just not now.
For now, it's just an adaptation in progress of another program.
*/
#define __main__	// because of it is the first, the other will not compile if not this line

#include <unistd.h>		// sleep
#include <time.h>		// tm, localtimeb
#include <ctype.h> 		// isdigit
#include <termios.h>    // -> Disabl_ctrl_d (IN) functions.c
#include <signal.h>		// -> To remap CTRL+C

#ifndef psystem
	#include <stdio.h>		// printf, scanf
	#include <stdlib.h>		// malloc, alloc (sem ela tmb tive algumas instabilidades com o uso de unsigneds)
	#include <stdint.h>		// uint8_t
	#include <string.h>		// strcmp, strlen
	#pragma GCC warning "Loaded standard modules. Please use strlen2 instead of strlen."
#else
	#include "good_practices.c"
	// Basicly imports the four modules above
	// Makes so that int, unsigned and other lables/macros are not usable
	#pragma GCC warning "Loaded custom module."
#endif

#include "functions.c"
#include "linked_lists.c"


typedef struct _account{
	uint32_t _uid;      // Unique ID, normal unsigned int for now
	char* name[100];    // Name?
	char* password; 	// Very long string, hopefully, maybe implemente a hash system? Like /etc/passwd? :)
} ACCOUNT;

typedef struct _book{
	uint64_t _uid;      				// Unique ID, ISBN-13
	char* name[100];         			// Name
	uint8_t quantity;					// How many books are there in the bibl system
	char* description;  				// Very long string :)
	LinkedList* requested_by;  			// Contains a list of users that currently have the book
	LinkedList* queue_for_students;	    // Maybe later a file??
} BOOK;

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
		if(_escolha_menu==0){
			break;
		}
		switch(_escolha_menu){
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

uint8_t login(uint8_t account_flag_type){
	char buffer[26];
	uint8_t flag=0;
	uint32_t account_id;
	while (1){
		printf("Account ID: ");
		read_n_chars(6, buffer);
		account_id = (int64_t) str_to_int64_t_flag(buffer, &flag);
		if (flag==1 && account_id <= 99999){
			break; // TODO: make the check also
		}
		printf("Invalid ID for %s account!\n", account_flag_type == 0 ? "librarian" : "student");
		pause_();
		return 1;  // TODO: Adicionar confirmação se quer reintroduzir ou voltar ao menu inicial
	}
	while (1){
		printf("Password: ");
		read_n_chars(25, buffer);
		if (strlen2(buffer) <= 24){
			break; // TODO: also make the check
		}
		printf("Invalid password!\n");
		pause_();
		return 1;  // TODO: Adicionar confirmação se quer reintroduzir ou voltar ao menu inicial
	}
	// TODO: De alguma forma registar a conta "logada" atualmente
	printf("Login Sucessfull as %s!\n", "TO_GET_NAME");
	pause_();
	return 0;  // sucessfull
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
