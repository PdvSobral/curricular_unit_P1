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


typedef struct _student{
	uint32_t _uid;      // Unique ID, normal unsigned int for now
	char* name;         // Maybe make it have a fixed lenght?? And be a username, so unique too?
	char* password; 	// Very long string, hopefully, maybe implemente a hash system? Like /etc/passwd? :)
} STUDENT;

typedef struct _biblman{
	uint32_t _uid;      // Unique ID, normal unsigned int for now
	char* name;         // Maybe make it have a fixed lenght?? And be a username, so unique too?
	char* password; 	// Very long string, hopefully, maybe implemente a hash system? Like /etc/passwd? :)
} BIBLMAN;

typedef struct _book{
	uint32_t _uid;      				// Unique ID, normal unsigned int for now
	char* name;         				// Maybe make it have a fixed lenght??
	char* description;  				// Very long string :)
	uint8_t state;	 					// 1: Ocupied  &&   0: Free
	STUDENT* requested_by;  			// Alumni that has
	LinkedList*  queue_for_students;	// Maybe later a file??
} BOOK;

typedef struct _self{
	LinkedList* sudent_list;
	LinkedList* biblman_list;
	LinkedList* book_list;
} SELF;

// Defenition of the menu arrays
const uint8_t len_main_menu = 3;
const char main_menu[][CABECALHO_LEN] = {
	"Student",
	"Librarian",
	"Exit"
};
const uint8_t len_logreg_menu = 3;
const char logreg_menu[][CABECALHO_LEN] = {
	"Login",
	"Register",
	"Return to previous menu"
};


// MENUS
void student_account(SELF* self){
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
		_escolha_menu = menu("STUDENT ACCOUNT ", CABECALHO_LEN, logreg_menu, len_logreg_menu, 1);
		if(_escolha_menu==0) break;
		clear_screen();
		switch(_escolha_menu){
			default: printf("\nFunção ainda não implementada!!\n");
		}
		sleep(3);
	}
	return;
}
void biblman_account(SELF* self){
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
		_escolha_menu = menu("LIBRARIAN ACCOUNT ", CABECALHO_LEN, logreg_menu, len_logreg_menu, 1);
		if(_escolha_menu==0) break;
		switch(_escolha_menu){
			default: printf("\nFunção ainda não implementada!!\n");
		}
		sleep(3);
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
	SELF self;
	self.sudent_list = create_linked_list();
	self.biblman_list = create_linked_list();
	self.book_list = create_linked_list();

	while (1){
		clear_screen();
		escolha_menu = menu("PLEASE CHOOSE ACCOUNT TYPE", CABECALHO_LEN, main_menu, len_main_menu, 1);
		if(escolha_menu==0) break;
		switch(escolha_menu){
			case 1: student_account(&self); break;
			case 2: biblman_account(&self); break;
			default: printf("\nFunção ainda não implementada!!\n");
		}
	}
	printf("Saindo...\n");
	#ifdef AGGRESSIVE
		enable_ctrl_d();
	#endif
	return 0;
};
