/* Encoding: UTF-8
@Authors: Pedro Sobral (33641), <Name> (<Numero>)
@Date: 08/04/2025
@Links: Project github repository -> https://github.com/PdvSobral/<GITHUB>

This program was developed as an answer to a problem described in a pdf file provided to us.
The pdf is present in the same repository as this program.
*/
#define __main__	// because of it is the first, the other will not compile if not this line

#include <unistd.h>	// sleep
#include <time.h>	// tm, localtime
#include <ctype.h>	// isdigit

#ifndef psystem
	#include <stdio.h>		// printf, scanf
	#include <stdlib.h>		// malloc, alloc (sem ela tmb tive algumas instabilidades com o uso de unsigneds)
	#include <stdint.h>		// uint8_t
	#include <string.h>		// strcmp, strlen
	#pragma GCC warning "Loaded standard modules."
#else
	#include <good_practices.c>
	// Basicly imports the four modules above
	// Makes so that int, unsigned and other lables/macros are not usable
	#pragma GCC warning "Loaded custom module."
#endif

#include "functions.c"


// Defenition of the menu arrays
const uint8_t len_main_menu = 4;  // MENU PRINCIPAL
const char main_menu[][CABECALHO_LEN] = {
	"Gestão de Alunos",
	"Gestão de Instrutores",
	"Gestão de Aulas",
	"Sair"
};
const uint8_t len_alumin_mngmt_menu = 11;  // MENU DE GESTÃO DE ALUNOS
const char alumin_mngmt_menu[][CABECALHO_LEN] = {
	"Registar Novo Aluno",
	"Pesquisar Aluno por Nome",
	"Consultar Dados de um Aluno",
	"Alterar Dados de um Aluno",
	"Definir Aluno como Ativo ou Inativo",
	"Listar Alunos Ativos",
	"Listar Alunos Com Carta",
	"Listar Alunos por Código Postal",
	"Listar Alunos com Filtro pela Idade",
	"Listar Alunos por Ordem Alfabética",
	"Voltar"
};
const uint8_t len_instructor_mngmt_menu = 8;  // MENU DE GESTÃO DE INSTRUTORES
const char instructor_mngmt_menu[][CABECALHO_LEN] = {
	"Registar Novo Instrutor",
	"Alterar dados de um Instrutor",
	"Definir Instrutor como Ativo ou Inativo",
	"Pesquisar Instrutor por Nome",
	"Consultar Dados de um Instrutor",
	"Listar Instrutores Ativos",
	"Listar Instrutores por Ordem Alfabética",
	"Voltar"
};
const uint8_t len_change_data_instructor_submenu = 5;  // SUBMENU ALTERAÇÂO DADOS INSTRUTOR
const char change_data_instructor_submenu[][CABECALHO_LEN] = {
	"Nome",
	"Número do cartão de cidadão",
	"Ano de Entrada para a escola",
	"Email",
	"Voltar"
};
const uint8_t len_change_data_alumni_submenu = 8;  // SUBMENU ALTERAÇÂO DADOS ALUNO
const char change_data_alumni_submenu[][CABECALHO_LEN] = {
	"Nome",
	"Email",
	"Número do cartão de cidadão",
	"NIF",
	"Endereço",
	"Data de Nascimento",
	"Dados Carta de Condução",
	"Voltar"
};
const uint8_t len_classes_menu = 3;  // MENU AULAS
const char classes_menu[][CABECALHO_LEN] = {
	"Marcar Nova Aula",
	"Consultar Aulas",
	"Voltar"
};

//funções
int8_t checkNif(char *str){
	/*
	Esta função verifica se uma string contém apenas números, tem um tamanho de 9 e que o número é válido
	Para um número NIF ser válido tem de seguir um algoritmo:
		1 -> Multiplicar os primeiros 8 números por um peso, pesos esses 10 menos a sua posição (9, 8, 7, 6, 5, 4, 3 e 2)
		2 -> Somar os resultados das multiplicações
		3 -> Fazer o módulo 11 e guardar esse valor
		4 -> Se o módulo for 0 ou 1, o último dígito deverá ser 0
		5 -> Para outro valor, o último dígito será 11 menos o resultado do módulo
	Argumentos:
		const char *str -> Apontador para o início da string terminada com '\0' a analisar
	Retorno:
		int8_t 0  -> A string passada é válida
		int8_t -1 -> A string passada não tem um tamanho de 9
		int8_t -2 -> A string passada não contém apenas números
		int8_t -3 -> O numero representado na string passada não é válido pelo algoritmo de verificação
	*/
    if (strIsNum(str) == -1) return -2;
    if (strlen(str) != 9) return -1;
    // O máximo que o buffer pode receber é 396, caso o número passado seja 99999999x, e nunca será negativo
    uint16_t buffer = 0;
    for (uint8_t i = 0; i < 8; i++) buffer += (str[i] - 0x30) * (9 - i);
	buffer = buffer % 11;
	if (buffer==0 || buffer==1){
		if (str[8]- 0x30 == 0) return 0;
		return -3;
	}
	else if (11-buffer != (str[8]- 0x30)) return -3;
    return 0;
}
int8_t checkEmail(char *str){
	/*
	Esta função verifica se uma string contém um @ e pelo menos um ponto (.) depois deste
	Argumentos:
		const char *str -> Apontador para o início da string terminada com '\0' a analisar
	Retorno:
		int8_t 0  -> A string passada apenas contém um @ e pelo menos um ponto (.)
		int8_t -1 -> A string passada não contém pelo menos um ponto (.) depois do @
		int8_t -2 -> A string passada não contém @
		int8_t -3 -> A string passada contém mais do que um @
	*/
    int8_t status = -2;
    for (uint8_t i = 0; str[i] != '\0'; i++) {
        if (str[i] == '@'){
        	if(status==-2) status++;
        	else return -3;
        } else if (str[i] == '.' && status == -1) status++;
    }
    return status;
}
int8_t checkPostalCode(char* str){
	/*
	Esta função verifica se uma string é um código postal válido e se está formatado ou não
	Argumentos:
		const char *str -> Apontador para o início da string terminada com '\0' a analisar
	Retorno:
		int8_t  0 -> A string passada é válida e não está formatada
		int8_t  1 -> A string passada é válida e está formatada
		int8_t -1 -> A string passada não é válida
		int8_t -2 -> A string passada não contém o tamanho certo (8: formatada, 7: não formatada)
	*/
	if (strlen(str) == 8) {
		for (uint8_t i = 0; i < 8; i++){
			if (i==4 && str[i] != '-') return -1;
			if (i!=4 && !isdigit(str[i])) return -1;
		}
		return 1;
	}
	if (strlen(str) == 7) return strIsNum(str);
    return -2;
}
int8_t checkMonth(int64_t month, DATE *_temp){
	/*
	Esta função verifica se um número é válido dado os dados passados
	Argumentos:
		int64_t month -> Número do mês
		DATE *_temp   -> Data de onde vai ser lido o dia
	Retorno:
		int8_t  0 -> O número é um mês válido
		int8_t -1 -> O número não é um mês válido
		int8_t -2 -> Fevereiro não tem dia 30 ou 31
		int8_t -3 -> O mês selecionado não tem dia 31
	*/
	if(month<=0 || month>12) return -1;
	if(month==2 && _temp->day>29) return -2;
	if(!(month==1 || month==3 || month==5 || month==7 || month==8 || month==10 || month==12)) if(_temp->day>30) return -3;
	return 0;
}
int8_t checkYear(int64_t year, DATE *_temp){
	/*
	Esta função verifica se um número é válido dado os dados passados
	Argumentos:
		int64_t year  -> Número do ano
		DATE *_temp   -> Data de onde vai ser lido o dia e o mês
	Retorno:
		int8_t  0 -> O número é um ano válido
		int8_t -1 -> O número é inferior a 1900
		int8_t -2 -> Fevereiro não tem dia 29 nesse ano
	*/
	if(year<1900) return -1;
	if(is_leap(year)==-1 && _temp->month==2 && _temp->day==29) return -2;
	return 0;
}

uint8_t read_date_tabed(DATE *_temp, char *buffer){
	/*
	Função pare ler uma data.
	Argumentos:
		DATE *_temp  -> Apontador para uma struct tipo DATE, onde será guardada a data
		char *buffer -> Buffer utilizado para a leitura dos diversos dados, mínimo de 5 chars
	Retorno:
		uint8_t  0 -> Leitura terminada com sucesso.
		uint8_t -1 -> Leitura terinada com ordem de voltar ao menu.
	*/
	int64_t int64_t_buffer;
	int8_t return_code;
	while(1){
		printf("\tDia: ");
		read_n_chars(3, buffer);
		int64_t_buffer = str_to_int64_t(buffer);
		if(!(int64_t_buffer<=0 || int64_t_buffer>31)) break;
		printf("Dia inválido.\n");
		printf("Deseja reintroduzir o valor (1) ou voltar ao menu (qualquer outro valor)? -> ");
		read_n_chars(2, buffer);
		int64_t_buffer = str_to_int64_t(buffer);
		if(!(int64_t_buffer==1)) return -1;
	} _temp->day = (uint8_t) int64_t_buffer;
	while(1){
		printf("\tMês: ");
		read_n_chars(3, buffer);
		int64_t_buffer = str_to_int64_t(buffer);
		return_code = checkMonth(int64_t_buffer, _temp);
		if (!return_code) break;
		printf("Mês inválido: ");
		switch (return_code){
			case -1: printf("Mês não existe!\n"); break;
			case -2: printf("Fevereiro tem no máximo 29 dias!\n"); break;
			case -3: printf("O mês que intoduziu não contém o dia %hhu!\n", _temp->day); break;
			default: printf("Erro não reconhecido!\n");
		}
		printf("Deseja reintroduzir o valor (1) ou voltar ao menu (qualquer outro valor)? -> ");
		read_n_chars(2, buffer);
		int64_t_buffer = str_to_int64_t(buffer);
		if(!(int64_t_buffer==1)) return -1;
	} _temp->month = (uint8_t) int64_t_buffer;
	while(1){
		printf("\tAno: ");
		read_n_chars(5, buffer);
		int64_t_buffer = str_to_int64_t(buffer);

		return_code = checkYear(int64_t_buffer, _temp);
		if (!return_code) break;
		printf("Ano inválido: ");
		switch (return_code){
			case -1: printf("Ano Inferior a 1900!\n"); break;
			case -2: printf("Ano não é bissexto mas foi introduzida como data 29 de Fevereiro.!\n"); break;
			default: printf("Erro não reconhecido!\n");
		}
		printf("Deseja reintroduzir o valor (1) ou voltar ao menu (qualquer outro valor)? -> ");
		read_n_chars(2, buffer);
		int64_t_buffer = str_to_int64_t(buffer);
		if(!(int64_t_buffer==1)) return -1;
	} _temp->year = (uint16_t) int64_t_buffer;
	return 0;
}
int8_t read_address_tabed(ADDRESS *_temp, char *buffer){
	/*
	Função pare ler um endereço.
	Argumentos:
		ADDRESS *_temp  -> Apontador para uma struct tipo ADDRESS, onde será guardado o endereço
		char *buffer    -> Buffer utilizado para a leitura dos diversos dados
	Retorno:
		uint8_t  0 -> Leitura terminada com sucesso.
		uint8_t -1 -> Leitura terinada com ordem de voltar ao menu.
	*/
	int64_t int64_t_buffer;
	while(1){
		printf("\tRua (max. %d caracteres): ", LEN_STREET);
		read_n_chars(LEN_STREET+1, buffer);
		if(strlen(buffer)<=LEN_STREET) break;
		printf("Comprimento excedido.\n");
		printf("Deseja reintroduzir o valor (1) ou voltar ao menu (qualquer outro valor)? -> ");
		read_n_chars(2, buffer);
		int64_t_buffer = str_to_int64_t(buffer);
		if(!(int64_t_buffer==1)) return -1;
	} strcpy(_temp->street, buffer);
	while(1){
		printf("\tNº Porta (max. 4 dígitos): ");
		read_n_chars(5, buffer);
		int64_t_buffer = strIsNum(buffer);
		if (int64_t_buffer==0){
			int64_t_buffer = str_to_int64_t(buffer);
			if(int64_t_buffer>9999 || int64_t_buffer<0) int64_t_buffer = -1;
			else break;
		}
		printf("Nº Porta inválido: ");
		switch (int64_t_buffer){
			case -1: printf("O valor introduzido não é um número válido!\n"); break;
			default: printf("Erro não reconhecido!\n");
		}
		printf("Deseja reintroduzir o valor (1) ou voltar ao menu (qualquer outro valor)? -> ");
		read_n_chars(2, buffer);
		int64_t_buffer = str_to_int64_t(buffer);
		if(!(int64_t_buffer==1)) return -1;
	} _temp->door_number = (uint16_t) int64_t_buffer;
	while(1){
		printf("\tCódigo postal (xxxx-xxx): ");
		read_n_chars(LEN_POSTAL_CODE+1, buffer);
		int64_t_buffer = checkPostalCode(buffer);
		if (int64_t_buffer==1) break;
		printf("Código postal inválido: ");
		switch (int64_t_buffer){
			case  0: printf("O Código Postal Não se encontra bem formatado.\n"); break;
			case -1:
			case -2: printf("O valor introduzido não é um Código postal válido!\n"); break;
			default: printf("Erro não reconhecido!\n");
		}
		printf("Deseja reintroduzir o valor (1) ou voltar ao menu (qualquer outro valor)? -> ");
		read_n_chars(2, buffer);
		int64_t_buffer = str_to_int64_t(buffer);
		if(!(int64_t_buffer==1)) return -1;
	} strcpy(_temp->postal_code, buffer);
	while(1){
		printf("\tLocalidade (max. %d caracteres): ", LEN_LOCATTION);
		read_n_chars(LEN_LOCATTION+1, buffer);
		if(strlen(buffer)>LEN_LOCATTION){
			printf("Comprimento excedido.\n");
			printf("Deseja reintroduzir o valor (1) ou voltar ao menu (qualquer outro valor)? -> ");
			read_n_chars(2, buffer);
			int64_t_buffer = str_to_int64_t(buffer);
			if(!(int64_t_buffer==1)) return -1;
		} else break;
	} strcpy(_temp->location, buffer);
	return 0;
}
uint8_t read_time_tabed(TIME *_temp, char *buffer){
	/*
	Função pare ler uma hora.
	Argumentos:
		TIME *_temp  -> Apontador para uma struct tipo TIME, onde será guardado o tempo
		char *buffer -> Buffer utilizado para a leitura dos diversos dados
	Retorno:
		uint8_t  0 -> Leitura terminada com sucesso.
		uint8_t -1 -> Leitura terinada com ordem de voltar ao menu.
	*/
	int64_t int64_t_buffer;
	int8_t return_cd;
	while(1){
		printf("\tHora: ");
		read_n_chars(5, buffer);
		int64_t_buffer = str_to_int64_t(buffer);
		if(!(int64_t_buffer<0 || int64_t_buffer>23)) break;
		printf("Hora inválida: ");
		printf("Hora não contida no intervalo 0 -> 23!\n");
		printf("Deseja reintroduzir o valor (1) ou voltar ao menu (qualquer outro valor)? -> ");
		read_n_chars(2, buffer);
		return_cd = str_to_int64_t(buffer);
		if(!(return_cd==1)) return -1;
	} _temp->hour = (uint8_t) int64_t_buffer;
	while(1){
		printf("\tMinutos: ");
		read_n_chars(5, buffer);
		int64_t_buffer = str_to_int64_t(buffer);
		if(!(int64_t_buffer<0 || int64_t_buffer>59)) break;
		printf("Minutos inválidos: ");
		printf("Minutos não contidos no intervalo 0 -> 59!\n");
		printf("Deseja reintroduzir o valor (1) ou voltar ao menu (qualquer outro valor)? -> ");
		read_n_chars(2, buffer);
		return_cd = str_to_int64_t(buffer);
		if(!(return_cd==1)) return -1;

	} _temp->minutes = (uint8_t) int64_t_buffer;
	return 0;
}
uint64_t time_diference(DATE class_date1, TIME class_time1, DATE class_date2, TIME class_time2){
	/*
	Função usada para calcular o tempo em segundos entre duas datas e respetivas horas
	Argumentos:
		DATE class_date1 -> Primeira Data
		TIME class_time1 -> Hora da Primeira Data
		DATE class_date2 -> Segunda Data
		TIME class_time2 -> Hora da Segunda Data
	Retorno:
		uint64_t -> Tempo em segundos que separa as duas datas
	*/
	struct tm date1 = {0};
    struct tm date2 = {0};

    date1.tm_year = class_date1.year - 1900;
    date1.tm_mon = class_date1.month - 1;
    date1.tm_mday = class_date1.day;
    date1.tm_hour = class_time1.hour;
    date1.tm_min = class_time1.minutes;
    date1.tm_sec = 0;

    date2.tm_year = class_date2.year - 1900;
    date2.tm_mon = class_date2.month - 1;
    date2.tm_mday = class_date2.day;
    date2.tm_hour = class_time2.hour;
    date2.tm_min = class_time2.minutes;
    date2.tm_sec = 0;

    time_t timestamp1 = mktime(&date1);
    time_t timestamp2 = mktime(&date2);

    if (timestamp1 == -1 || timestamp2 == -1) {
        printf("Error converting dates to UNIX timestamps.\n");
        return -1;
    }

    double difference = difftime(timestamp2, timestamp1);
	if(difference<=0) difference *= -1;
	if( ((uint64_t) difference) < difference) difference += 1;

    return (uint64_t) difference;
};

// Funções instrutores
void print_instructors(SELF *self, uint8_t *_indexes, uint8_t len_array){
	/*
	Procedimento que mostra no terminal os dados de um ou mais instrutores, baseado nos argumentos passados
	Formato de impressão:
		----------------------------------------------------------------------
		Nome: Jota
		Número do cartão de cidadão: 999999999
		Email: pedro@pedro.pt
		Ano de Entrada para a escola: 2005
		Estado: Ativo
		----------------------------------------------------------------------
		\n
	Repete quantas vezes necessário.
	Argumentos:
		SELF *self        -> Struct do tipo _self de onde será usado o array "instructors_array"
		uint8_t *_indexes -> Endereço do primeiro elemento de um array de uint8_t de onde serão lidos os indexes dos instrutores a mostrar.
							 Pode ser passado um endereço de memória de um valor simples uint8_t, dado que o valor 1 seja fornecido no argumento len_array
		uint8_t len_array -> Número de elementos tipo uint8_t a ler com base no endereço fornecido
	*/
	for(uint8_t index=0; index<len_array; index++){
		for(uint8_t _index = 0; _index<CABECALHO_LEN; _index++) printf("-");
		printf("\n");
		printf("Nome: %s\n", self->instructors_array[_indexes[index]].name);
		printf("Número do cartão de cidadão: %lu\n", self->instructors_array[_indexes[index]].number_citizen_card);
		printf("Email: %s\n", self->instructors_array[_indexes[index]].email);
		printf("Ano de Entrada para a escola: %u\n", self->instructors_array[_indexes[index]].year_start);
		printf("Estado: %s\n", self->instructors_array[_indexes[index]].active_state ? "Ativo" : "Inativo");  // interessante
		for(uint8_t _index = 0; _index<CABECALHO_LEN; _index++) printf("-");
		printf("\n\n");
	}
	return;
}
void add_instructor(SELF *self){
	/*
	Procedimento para registar um novo instrutor
	Argumentos:
		SELF *self = Estrutura de contexto
	Retorno:
		Nenhum
	*/
	cabecalho("REGISTO DE UM NOVO INSTRUTOR", CABECALHO_LEN);
	if(self->instructors_array_insert_index>MAX_INSTRUCTORS){
		printf("Máximo de instrutores atingido!");
		return;
	}
	INSTRUCTOR _temp;
	char buffer[BUFFER_LEN+1];
	int64_t int64_t_buffer;
	while(1){
		printf("Nome Do Instrutor: ");
		read_n_chars(LEN_NAME, buffer);
		int8_t found = 0;
		do {
			if(strcmp(self->alumni_array[found].name,buffer)==0){
				printf("Nome Inválido: Nome coincide com aluno nº%hhu (%s)\nPor favor use um identificador para os distinguir.\n", self->alumni_array[found].number, self->alumni_array[found].name);
				found = -10;
			}
			found++;
		} while((found < self->alumni_array_insert_index) && (found > 0));
		if (found>0){
			found = 0;
			do {
				if(strcmp(self->instructors_array[found].name, buffer)==0){
					printf("Nome Inválido: Nome coincide com instrutor %s\nPor favor use um identificador para os distinguir.\n", self->instructors_array[found].name);
					found = -10;
				}
				found++;
			} while((found < self->instructors_array_insert_index) && (found > 0));
		}
		if(found<0){
			printf("Deseja reintroduzir o valor (1) ou voltar ao menu (qualquer outro valor)? -> ");
			read_n_chars(2, buffer);
			int64_t_buffer = str_to_int64_t(buffer);
			if(!(int64_t_buffer==1)) return;
		} else break;
	} strcpy(_temp.name, buffer);
	while(1){
		printf("Email: ");
		read_n_chars(LEN_EMAIL, buffer);
		int64_t_buffer = checkEmail(buffer);
		if (!int64_t_buffer) {int64_t_buffer = str_to_int64_t(buffer); break;}
		printf("Email inválido: ");
		switch (int64_t_buffer){
			case -1: printf("O email introduzido não tem um domínio válido!\n"); break;
			case -2: printf("O email introduzido não contém @!\n"); break;
			case -3: printf("O email introduzido contém mais do que um @!\n"); break;
			default: printf("Erro não reconhecido!\n");
		}
		printf("Deseja reintroduzir o valor (1) ou voltar ao menu (qualquer outro valor)? -> ");
		read_n_chars(2, buffer);
		int64_t_buffer = str_to_int64_t(buffer);
		if(!(int64_t_buffer==1)) return;
	} strcpy(_temp.email, buffer);
	while(1){
		printf("Número do cartão de cidadão (8 dígitos): ");
		read_n_chars(9, buffer);
		int64_t_buffer = str_to_int64_t(buffer);
		int8_t found = 0;
		do {
			if(self->alumni_array[found].number_citizen_card == ((uint64_t) int64_t_buffer)){
				printf("Cartão de cidadão coincide com aluno nº%hhu (%s)\n", self->alumni_array[found].number, self->alumni_array[found].name);
				found = -10;
			}
			found++;
		} while((found < self->alumni_array_insert_index) && (found > 0));
		if (found>0){
			found = 0;
			do {
				if(self->instructors_array[found].number_citizen_card == ((uint64_t) int64_t_buffer)){
					printf("Cartão de cidadão coincide com o instrutor %s\n", self->instructors_array[found].name);
					found = -10;
				}
				found++;
			} while((found < self->instructors_array_insert_index) && (found > 0));
		}
		if((int64_t_buffer<10000000) || (int64_t_buffer>99999999) || (found<0)){
			printf("Cartão de cidadão inválido.\n");
			printf("Deseja reintroduzir o valor (1) ou voltar ao menu (qualquer outro valor)? -> ");
			read_n_chars(2, buffer);
			int64_t_buffer = str_to_int64_t(buffer);
			if(!(int64_t_buffer==1)) return;
		} else break;
	} _temp.number_citizen_card = (uint64_t) int64_t_buffer;
	while(1){
		printf("Ano de Entrada para a escola: ");
		read_n_chars(4, buffer);
		int64_t_buffer = str_to_int64_t(buffer);
		if(int64_t_buffer<1900){
			printf("Ano inválido (Inferior a 1900 ou não numérico).\n");
			printf("Deseja reintroduzir o valor (1) ou voltar ao menu (qualquer outro valor)? -> ");
			read_n_chars(2, buffer);
			int64_t_buffer = str_to_int64_t(buffer);
			if(!(int64_t_buffer==1)) return;
		} else break;
	} _temp.year_start = (uint16_t) int64_t_buffer;
	while(1){
		printf("Estado [1 - Ativo, 0 - Inativo]: ");
		read_n_chars(2, buffer);
		int64_t_buffer = str_to_int64_t(buffer);
		if(!(int64_t_buffer==0 || int64_t_buffer==1)){
			printf("Estado inválido.\n");
			printf("Deseja reintroduzir o valor (1) ou voltar ao menu (qualquer outro valor)? -> ");
			read_n_chars(2, buffer);
			int64_t_buffer = str_to_int64_t(buffer);
			if(!(int64_t_buffer==1)) return;
		} else break;
	} _temp.active_state = (uint8_t) int64_t_buffer;
	printf("Deseja mesmo guardar os dados introduzidos?\n\t[1 -> Sim, Outro Valor -> Não] -> ");
	read_n_chars(2, buffer);
	int64_t_buffer = str_to_int64_t(buffer);
	if(!(int64_t_buffer==1)) return;
	self->instructors_array[self->instructors_array_insert_index] = _temp;
	self->instructors_array_insert_index++;
	printf("Instrutor salvo com sucesso.\n");
	return;
}
void search_name_instructor(SELF *self){
	/*
	Procedimento responsável pela operação opção "Pesquisar Instrutor por Nome" do menu "MENU GESTÃO DE INSTRUTORES"
	Argumentos:
		SELF *self -> Struct do tipo _self que contém o contexto de chamada.
	Retorno:
		Nenhum
	*/
	cabecalho("PROCURA DE INTRUTOR", CABECALHO_LEN);
    if (self->instructors_array_insert_index == 0) {
        printf("Não existe nenhum instrutor registado para pesquisar.\n");
        return;
    }
    char name_instructor[LEN_NAME+1];
    printf("Nome do Instrutor a pesquisar: ");
	read_n_chars(LEN_NAME, name_instructor);
    uint8_t found = 0;
    for (uint8_t index = 0; index < self->instructors_array_insert_index; index++){
        if (strncasecmp(name_instructor, self->instructors_array[index].name, strlen(name_instructor)) == 0){
        	if(found==0) printf("Instrutores encontrados incluindo esse nome:\n");
            printf("\t-> %s\n", self->instructors_array[index].name);
            found = 1;
        }
    }
    if (!found) printf("Nenhum instrutor encontrado contendo esse nome.\n");
    return;
}
void change_instructor_data(SELF *self){
	/*
	Procedimento para alterar dados de um instrutor
	Argumentos:
		SELF *self = Estrutura de contexto
	Retorno:
		Nenhum
	*/
	cabecalho("ALTERAÇÃO DOS DADOS DE UM INSTRUTOR", CABECALHO_LEN);
	if(self->instructors_array_insert_index == 0){
		printf("Não existe nenhum instrutor registado para alterar os dados.\n");
		return;
	}
	char name_to_search[LEN_NAME+1];
	printf("Insira o nome do instrutor que deseja alterar: ");
	read_n_chars(LEN_NAME, name_to_search);
	for (uint8_t index = 0; index < self->instructors_array_insert_index; index++) {
		if (strcmp(self->instructors_array[index].name, name_to_search) == 0){
			uint8_t choice;
			char buffer[30];
			int64_t int64_t_buffer;
			int64_t uint16_t_buffer;
			cabecalho("DADO A ALTERAR", CABECALHO_LEN);
			choice = menu(change_data_instructor_submenu, len_change_data_instructor_submenu, 1);
			if (choice == 0) return;
			INSTRUCTOR _temp = self->instructors_array[index];
			switch (choice){
				case 1:
					printf("Novo Nome: ");
					read_n_chars(LEN_NAME, _temp.name);
					break;
				case 2:
					while(1){
						printf("Novo Número do cartão de cidadão (8 dígitos): ");
						read_n_chars(9, buffer);
						int64_t_buffer = str_to_int64_t(buffer);
						int8_t found = 0;
						do {
							if(self->alumni_array[found].number_citizen_card == ((uint64_t) int64_t_buffer)){
								printf("Cartão de cidadão coincide com aluno nº%hhu (%s)\n", self->alumni_array[found].number, self->alumni_array[found].name);
								found = -10;
							}
							found++;
						} while((found < self->alumni_array_insert_index) && (found > 0));
						if (found>0){
							found = 0;
							do {
								if(self->instructors_array[found].number_citizen_card == ((uint64_t) int64_t_buffer)){
									printf("Cartão de cidadão coincide com instrutor %s\n", self->instructors_array[found].name);
									found = -10;
								}
								found++;
							} while((found < self->instructors_array_insert_index) && (found > 0));
						}
						if((int64_t_buffer<10000000) || (int64_t_buffer>99999999) || (found<0)){
							printf("Cartão de cidadão inválido.\n");
							printf("Deseja reintroduzir o valor (1) ou voltar ao menu (qualquer outro valor)? -> ");
							read_n_chars(2, buffer);
							int64_t_buffer = str_to_int64_t(buffer);
							if(!(int64_t_buffer==1)) return;
						} else break;
					} _temp.number_citizen_card = (uint64_t) int64_t_buffer;
					break;
				case 3:
					while(1){
						printf("Novo Ano de Entrada para a escola: ");
						read_n_chars(4, buffer);
						int64_t_buffer = str_to_int64_t(buffer);
						if(int64_t_buffer<1900){
							printf("Ano inválido (Inferior a 1900 ou não numérico).\n");
							printf("Deseja reintroduzir o valor (1) ou voltar ao menu (qualquer outro valor)? -> ");
							read_n_chars(2, buffer);
							int64_t_buffer = str_to_int64_t(buffer);
							if(!(int64_t_buffer==1)) return;
						} else break;
						uint16_t_buffer = (uint16_t) int64_t_buffer;
					} _temp.year_start = uint16_t_buffer;
					break;
				case 4:
					while(1){
						printf("Novo Email: ");
						read_n_chars(LEN_EMAIL, buffer);
						int64_t_buffer = checkEmail(buffer);
						if (!int64_t_buffer) {int64_t_buffer = str_to_int64_t(buffer); break;}
						printf("Email inválido: ");
						switch (int64_t_buffer){
							case -1: printf("O email introduzido não tem um domínio válido!\n"); break;
							case -2: printf("O email introduzido não contém @!\n"); break;
							case -3: printf("O email introduzido contém mais do que um @!\n"); break;
							default: printf("Erro não reconhecido!\n");
						}
						printf("Deseja reintroduzir o valor (1) ou voltar ao menu (qualquer outro valor)? -> ");
						read_n_chars(2, buffer);
						int64_t_buffer = str_to_int64_t(buffer);
						if(!(int64_t_buffer==1)) return;
					} strcpy(_temp.email, buffer);
					break;
				default:
					printf("Opção ainda não implementada!\n");
			}
			char confirm[3];
			printf("Deseja guardar a alteração (1) ou voltar ao menu (qualquer outro valor)? -> ");
			read_n_chars(2, confirm);
			if(!(str_to_int64_t(confirm)==1)) printf("Alteração cancelada.\n");
			else {
				self->instructors_array[index] = _temp;
				printf("Alteração realizada com sucesso.\n");
			}
			return;
		}
	}
	printf("\nInstrutor não encontrado!!\n");
	return;
}
void change_instructor_state(SELF *self){
	/*
	Procedimento responsável pela operação opção "Definir Instrutor como Ativo ou Inativo" do menu "MENU GESTÃO DE INSTRUTORES"
	Argumentos:
		SELF *self -> Struct do tipo _self que contém o contexto de chamada.
	Retorno:
		Nenhum
	*/
	cabecalho("ALTERAR ESTADO DE UM INSTRUTOR", CABECALHO_LEN);
    if (self->instructors_array_insert_index == 0) {
        printf("Não existe nenhum instrutor registado para alterar o estado.\n");
        return;
    }
	char name_instructor[LEN_NAME+1];
    printf("Nome do Instrutor a alterar o estado: ");
    read_n_chars(LEN_NAME, name_instructor);
    for (uint8_t index = 0; index < self->instructors_array_insert_index; index++){
        if (strcmp(name_instructor, self->instructors_array[index].name) == 0) {
			cabecalho("NOVO ESTADO", CABECALHO_LEN);
			char _menu[2][CABECALHO_LEN] = {"Ativo", "Inativo"};
			uint8_t choice = menu(_menu, 2, 1);
			if (choice==self->instructors_array[index].active_state) {
				printf("O instrutor já se encontra com esse estado selecionado.\nEstado Inalterato.\n");
				return;
			}
			self->instructors_array[index].active_state = choice;
			printf("Estado alterado para '%s' com sucesso.\n", _menu[(choice+1)%2]);
			return;
        }
    }
    printf("Instrutor não encontrado.\n");
    return;
}
void check_data_instructor(SELF *self){
	/*
	Procedimento responsável pela operação opção "Consultar Dados de um Instrutor" do menu "MENU GESTÃO DE INSTRUTORES"
	Argumentos:
		SELF *self -> Struct do tipo _self que contém o contexto de chamada.
	Retorno:
		Nenhum
	*/
	cabecalho("CONSULTA DE DADOS DE UM INSTRUTOR", CABECALHO_LEN);
    if (self->instructors_array_insert_index == 0) {
        printf("Não existe nenhum instrutor registado para consultar os dados.\n");
        return;
    }
    char name_instructor[LEN_NAME+1];
    printf("Nome do Instrutor a consultar: ");
   	read_n_chars(LEN_NAME, name_instructor);
    for (uint8_t index = 0; index < self->instructors_array_insert_index; index++) {
        if (strcmp(name_instructor, self->instructors_array[index].name) == 0) {
            print_instructors(self, &index, 1);
            return;
        }
    }
    printf("Instrutor não encontrado.\n");
}
void print_active_instructors(SELF *self){
	/*
	Procedimento responsável pela operação opção "Listar Instrutores Ativos" do menu "MENU GESTÃO DE INSTRUTORES"
	Argumentos:
		SELF *self -> Struct do tipo _self que contém o contexto de chamada.
	Retorno:
		Nenhum
	*/
	cabecalho("LISTA DE INSTRUTORES ATIVOS", CABECALHO_LEN);
	if(self->instructors_array_insert_index == 0){
		printf("Não existe nenhum instrutor registado para pesquisar.\n");
		return;
	}
	uint8_t couter = 0;
	uint8_t indexes[self->instructors_array_insert_index];
	uint8_t n = self->instructors_array_insert_index;
	for(uint8_t index=0; index<n; index++){
		if(self->instructors_array[index].active_state==1){
			indexes[couter] = index;
			couter++;
		}
	}
	if(couter==0){
		printf("Nenhum instrutor ativo encontrado!\n");
		return;
	}
    print_instructors(self, indexes, couter);
    return;
}
void print_instructors_alphabeticly(SELF *self){
	/*
	Procedimento responsável pela operação opção "Listar Instrutores por Ordem Alfabética" do menu "MENU GESTÃO DE INSTRUTORES"
	Argumentos:
		SELF *self -> Struct do tipo _self que contém o contexto de chamada.
	Retorno:
		Nenhum
	*/
	cabecalho("LISTA DE INSTRUTORES", CABECALHO_LEN);
	if(self->instructors_array_insert_index == 0){
		printf("Não existe nenhum instrutor registado para mostrar.\n");
		return;
	}
	char strings[self->instructors_array_insert_index][LEN_NAME];
	uint8_t indexes[self->instructors_array_insert_index];
	uint8_t n = self->instructors_array_insert_index;
	for(uint8_t index=0; index<n; index++){
		indexes[index] = index;
		strcpy(strings[index], self->instructors_array[index].name);
	}
    char temp[LEN_NAME+1];
    // Bubble sort
    for(uint8_t index = 0; index < n - 1; index++) {
        for(uint8_t j = 0; j < n - index - 1; j++) {
            if (strcmp(strings[j], strings[j + 1]) > 0) {
                strcpy(temp, strings[j]);
                strcpy(strings[j], strings[j + 1]);
                strcpy(strings[j + 1], temp);
                temp[0] = indexes[j];
                indexes[j] = indexes[j + 1];
                indexes[j + 1] = temp[0];
            }
        }
    }
    print_instructors(self, indexes, self->instructors_array_insert_index);
    return;
}
// Funções Alunos
void print_alumni(SELF *self, uint8_t _indexes[], uint8_t len_array){
	/*
	Procedimento que mostra no terminal os dados de um ou mais instrutores, baseado nos argumentos passados
	Formato de impressão:
		----------------------------------------------------------------------
		Número de aluno: 002
		Nome: Diogo2
		Email: maria@maria.pt
		Número do cartão de cidadão: 000000000
		NIF do Aluno: 987654321
		Nº Carta de Condução: 000222222
		Endereço:
				Rua:
				Número da Porta: 0
				Código postal:
				Localidade:
		Data de nascimento:
				Dia: 02
				Mês: 02
				Ano: 2001
		Data de Obtenção da carta de condução:
				Dia: 00
				Mês: 00
				Ano: 0000
		Estado: Ativo
		----------------------------------------------------------------------
		\n
	Repete quantas vezes necessário.
	Argumentos:
		SELF *self        -> Struct do tipo _self de onde será usado o array "alumni_array"
		uint8_t *_indexes -> Endereço do primeiro elemento de um array de uint8_t de onde serão lidos os indexes dos alunos a mostrar.
							 Pode ser passado um endereço de memória de um valor simples uint8_t, dado que o valor 1 seja fornecido no argumento len_array
		uint8_t len_array -> Número de elementos tipo uint8_t a ler com base no endereço fornecido
	*/
	for(uint8_t index=0; index<len_array; index++){
		for(uint8_t _index = 0; _index<CABECALHO_LEN; _index++) printf("-");
		printf("\n");
		printf("Número de aluno: %03hhu\n", self->alumni_array[_indexes[index]].number);
		printf("Nome: %s\n", self->alumni_array[_indexes[index]].name);
		printf("Email: %s\n", self->alumni_array[_indexes[index]].email);
		printf("Número do cartão de cidadão: %09lu\n", self->alumni_array[_indexes[index]].number_citizen_card);
		printf("NIF do Aluno: %09lu\n", self->alumni_array[_indexes[index]].nif);
		printf("Nº Carta de Condução: %s\n", self->alumni_array[_indexes[index]].number_driving_license);
		printf("Endereço:\n");
		printf("\tRua: %s\n", self->alumni_array[_indexes[index]].address.street);
		printf("\tNúmero da Porta: %u\n", self->alumni_array[_indexes[index]].address.door_number);
		printf("\tCódigo postal: %s\n", self->alumni_array[_indexes[index]].address.postal_code);
		printf("\tLocalidade: %s\n", self->alumni_array[_indexes[index]].address.location);
		printf("Data de nascimento:\n");
		printf("\tDia: %02hhu\n", self->alumni_array[_indexes[index]].birth_date.day);
		printf("\tMês: %02hhu\n", self->alumni_array[_indexes[index]].birth_date.month);
		printf("\tAno: %04d\n", self->alumni_array[_indexes[index]].birth_date.year);
		printf("Data de Obtenção da carta de condução:\n");
		printf("\tDia: %02hhu\n", self->alumni_array[_indexes[index]].date_license_completed.day);
		printf("\tMês: %02hhu\n", self->alumni_array[_indexes[index]].date_license_completed.month);
		printf("\tAno: %04d\n", self->alumni_array[_indexes[index]].date_license_completed.year);
		printf("Estado: %s\n", self->alumni_array[_indexes[index]].active_state ? "Ativo" : "Inativo");
		for(uint8_t _index = 0; _index<CABECALHO_LEN; _index++) printf("-");
		printf("\n\n");
	}
	return;
}
void add_alumni(SELF *self){
	/*
	Procedimento para adicionar um novo aluno
	Argumentos:
		SELF *self -> Struct do tipo _self que contém o contexto de chamada.
	Retorno:
		Nenhum
	*/
	cabecalho("REGISTO DE UM NOVO ALUNO", CABECALHO_LEN);
	if(self->alumni_array_insert_index>MAX_ALUMNI){
		printf("Máximo de alunos atingido!\n");
		return;
	}
	ALUMNI _temp;
	char buffer[BUFFER_LEN+1];
	int64_t int64_t_buffer;
	while(1){
		printf("Nome Do Aluno: ");
		read_n_chars(LEN_NAME, buffer);

		int8_t found = 0;
		do {
			if(strcmp(self->alumni_array[found].name,buffer)==0){
				printf("Nome Inválido: Nome coincide com aluno nº%hhu (%s)\nPor favor use um identificador para os distinguir.\n", self->alumni_array[found].number, self->alumni_array[found].name);
				found = -10;
			}
			found++;
		} while((found < self->alumni_array_insert_index) && (found > 0));
		if (found>0){
			found = 0;
			do {
				if(strcmp(self->instructors_array[found].name, buffer)==0){
					printf("Nome Inválido: Nome coincide com instrutor %s\nPor favor use um identificador para os distinguir.\n", self->instructors_array[found].name);
					found = -10;
				}
				found++;
			} while((found < self->instructors_array_insert_index) && (found > 0));
		}
		if(found<0){
			printf("Deseja reintroduzir o valor (1) ou voltar ao menu (qualquer outro valor)? -> ");
			read_n_chars(2, buffer);
			int64_t_buffer = str_to_int64_t(buffer);
			if(!(int64_t_buffer==1)) return;
		} else break;
	} strcpy(_temp.name, buffer);
	while(1){
		printf("Email: ");
		read_n_chars(LEN_EMAIL, buffer);
		int64_t_buffer = checkEmail(buffer);
		if (!int64_t_buffer) {int64_t_buffer = str_to_int64_t(buffer); break;}
		printf("Email inválido: ");
		switch (int64_t_buffer){
			case -1: printf("O email introduzido não tem um domínio válido!\n"); break;
			case -2: printf("O email introduzido não contém @!\n"); break;
			case -3: printf("O email introduzido contém mais do que um @!\n"); break;
			default: printf("Erro não reconhecido!\n");
		}
		printf("Deseja reintroduzir o valor (1) ou voltar ao menu (qualquer outro valor)? -> ");
		read_n_chars(2, buffer);
		int64_t_buffer = str_to_int64_t(buffer);
		if(!(int64_t_buffer==1)) return;
	} strcpy(_temp.email, buffer);
	while(1){
		printf("Número do cartão de cidadão (8 dígitos): ");
		read_n_chars(9, buffer);
		int64_t_buffer = str_to_int64_t(buffer);
		int8_t found = 0;
		do {
			if(self->alumni_array[found].number_citizen_card == ((uint64_t) int64_t_buffer)){
				printf("Cartão de cidadão coincide com aluno nº%hhu (%s)\n", self->alumni_array[found].number, self->alumni_array[found].name);
				found = -10;
			}
			found++;
		} while((found < self->alumni_array_insert_index) && (found > 0));
		if (found>0){
			found = 0;
			do {
				if(self->instructors_array[found].number_citizen_card == ((uint64_t) int64_t_buffer)){
					printf("Cartão de cidadão coincide com instrutor %s\n", self->instructors_array[found].name);
					found = -10;
				}
				found++;
			} while((found < self->instructors_array_insert_index) && (found > 0));
		}
		if((int64_t_buffer<10000000) || (int64_t_buffer>99999999) || (found<0)){
			printf("Cartão de cidadão inválido.\n");
			printf("Deseja reintroduzir o valor (1) ou voltar ao menu (qualquer outro valor)? -> ");
			read_n_chars(2, buffer);
			int64_t_buffer = str_to_int64_t(buffer);
			if(!(int64_t_buffer==1)) return;
		} else break;
	} _temp.number_citizen_card = (uint64_t) int64_t_buffer;
	while(1){
		printf("NIF do Aluno (9 dígitos): ");
		read_n_chars(10, buffer);
		int64_t_buffer = checkNif(buffer);
		uint8_t found = 0;
		if (!int64_t_buffer) {
			int64_t_buffer = str_to_int64_t(buffer);
			do {
				if(self->alumni_array[found].nif == ((uint64_t) int64_t_buffer)) found = found + MAX_ALUMNI;
				found++;
			} while(found < self->alumni_array_insert_index);
			if(found<MAX_ALUMNI) break;
			found = found - MAX_ALUMNI - 1;
			int64_t_buffer = 0;
		}
		printf("NIF inválido: ");
		switch (int64_t_buffer){
			case  0: printf("NIF coincide com aluno nº%hhu (%s)\n", self->alumni_array[found].number, self->alumni_array[found].name); break;
			case -1: printf("O valor introduzido não tem um tamanho de 9 caracteres!\n"); break;
			case -2: printf("O valor introduzido não contém apenas números!\n"); break;
			case -3: printf("O valor introduzido não é válido pelo algoritmo de verificação!\n"); break;
			default: printf("Erro não reconhecido!\n");
		}
		printf("Deseja reintroduzir o valor (1) ou voltar ao menu (qualquer outro valor)? -> ");
		read_n_chars(2, buffer);
		int64_t_buffer = str_to_int64_t(buffer);
		if(!(int64_t_buffer==1)) return;
	} _temp.nif = (uint64_t) int64_t_buffer;
	printf("Já tem carta de condução? [1 -> Sim, Outro Valor -> Não]: ");
	read_n_chars(2, buffer);
	int64_t_buffer = str_to_int64_t(buffer);
	if(int64_t_buffer==1){
		while(1){
			printf("Nº Carta de Condução (max. %d caracteres): ", LEN_DRIVING_LICENSE);
			read_n_chars(LEN_DRIVING_LICENSE+1, buffer);
			if(strlen(buffer) > LEN_DRIVING_LICENSE){
				printf("Nº Carta de Condução inválido.\n");
				printf("Deseja reintroduzir o valor (1) ou voltar ao menu (qualquer outro valor)? -> ");
				read_n_chars(2, buffer);
				int64_t_buffer = str_to_int64_t(buffer);
				if(!(int64_t_buffer==1)) return;
			} else break;
		} strcpy(_temp.number_driving_license, buffer);
		printf("Data de Obtenção da carta de condução:\n");
		if(read_date_tabed(&_temp.date_license_completed, buffer) != 0) return;
	} else {
		strcpy(_temp.number_driving_license, "-");
		_temp.date_license_completed.day = 0;
		_temp.date_license_completed.year = 0;
		_temp.date_license_completed.month = 0;
	}
	printf("Endereço:\n");
	if(read_address_tabed(&_temp.address, buffer)==-1) return;
	printf("Data de nascimento:\n");
	if(read_date_tabed(&_temp.birth_date, buffer) != 0) return;
	while(1){
		printf("Estado [1 - Ativo, 0 - Inativo]: ");
		read_n_chars(2, buffer);
		int64_t_buffer = str_to_int64_t(buffer);
		if(!(int64_t_buffer==0 || int64_t_buffer==1)){
			printf("Estado inválido.\n");
			printf("Deseja reintroduzir o valor (1) ou voltar ao menu (qualquer outro valor)? -> ");
			read_n_chars(2, buffer);
			int64_t_buffer = str_to_int64_t(buffer);
			if(!(int64_t_buffer==1)) return;
		} else break;
	} _temp.active_state = (uint8_t) int64_t_buffer;
	printf("Deseja mesmo guardar os dados introduzidos?\n\t[1 -> Sim, Outro Valor -> Não] -> ");
	read_n_chars(2, buffer);
	int64_t_buffer = str_to_int64_t(buffer);
	if(!(int64_t_buffer==1)) return;
	_temp.number = self->alumni_array_insert_index + 1;
	self->alumni_array[self->alumni_array_insert_index] = _temp;
	self->alumni_array_insert_index++;
	printf("Dados Guardados Com Sucesso!\n");
	return;
}
void search_name_alumni(SELF *self){
	/*
	Procedimento responsável pela operação opção "Pesquisar Aluno por Nome" do menu "MENU GESTÃO DE ALUNOS"
	Argumentos:
		SELF *self -> Struct do tipo _self que contém o contexto de chamada.
	Retorno:
		Nenhum
	*/
	cabecalho("PROCURA DE ALUNO", CABECALHO_LEN);
    if (self->alumni_array_insert_index == 0) {
        printf("Não existe nenhum aluno registado para pesquisar.\n");
        return;
    }
    char name_alumni[LEN_NAME+1];
    printf("Nome do Aluno a pesquisar: ");
	read_n_chars(LEN_NAME, name_alumni);
    uint8_t found = 0;
    for (uint8_t index = 0; index < self->alumni_array_insert_index; index++){
        if (strncasecmp(name_alumni, self->alumni_array[index].name, strlen(name_alumni)) == 0){
        	if(found==0) {
        		printf("Alunos encontrados incluindo esse nome:\n");
        		printf("\t   (Nº) NOME\n");
        		found = 1;
        	}
            printf("\t-> (%2d) %s\n", self->alumni_array[index].number, self->alumni_array[index].name);
        }
    }
    if (found==0) printf("Nenhum aluno encontrado contendo esse nome.\n");
    return;
}
void check_data_alumni(SELF *self){
	/*
	Procedimento responsável pela operação opção "Consultar Dados de um Aluno" do menu "MENU GESTÃO DE ALUNOS"
	Argumentos:
		SELF *self -> Struct do tipo _self que contém o contexto de chamada.
	Retorno:
		Nenhum
	*/
	cabecalho("CONSULTA DE DADOS DE UM ALUNO", CABECALHO_LEN);
    if (self->alumni_array_insert_index == 0) {
        printf("Não existe nenhum aluno registado para consultar os dados.\n");
        return;
    }
    char name_alumni[LEN_NAME+1];
    printf("Nome do Aluno a consultar: ");
	read_n_chars(LEN_NAME, name_alumni);
    for (uint8_t index = 0; index < self->alumni_array_insert_index; index++) {
        if (strcmp(name_alumni, self->alumni_array[index].name) == 0) {
            print_alumni(self, &index, 1);
            return;
        }
    }
    printf("Aluno não encontrado.\n");
}
void change_alumni_data(SELF *self){
	/*
	Procedimento para alterar dados de um aluno
	Argumentos:
		SELF *self -> Struct do tipo _self que contém o contexto de chamada.
	Retorno:
		Nenhum
	*/
	cabecalho("ALTERAÇÃO DOS DADOS DE UM ALUNO", CABECALHO_LEN);
    if (self->alumni_array_insert_index == 0) {
        printf("Não existe nenhum aluno registado para alterar os dados.\n");
        return;
    }
	char name_to_search[LEN_NAME+1];
	printf("Insira o nome do aluno que deseja alterar: ");
	read_n_chars(LEN_NAME, name_to_search);
	for (uint8_t index = 0; index < self->alumni_array_insert_index; index++){
		if (strcmp(name_to_search, self->alumni_array[index].name) == 0){
			uint8_t choice;
			char buffer[30];
			int64_t int64_t_buffer;
			cabecalho("DADO A ALTERAR", CABECALHO_LEN);
			choice = menu(change_data_alumni_submenu, len_change_data_alumni_submenu, 1);
			if (choice == 0) return;
			ALUMNI _temp = self->alumni_array[index];
			switch (choice){
				case 1:
					printf("Novo Nome: ");
					read_n_chars(LEN_NAME, _temp.name);
					break;
				case 2:
					while(1){
						printf("Email: ");
						read_n_chars(LEN_EMAIL, buffer);
						int64_t_buffer = checkEmail(buffer);
						if (!int64_t_buffer) {int64_t_buffer = str_to_int64_t(buffer); break;}
						printf("Email inválido: ");
						switch (int64_t_buffer){
							case -1: printf("O email introduzido não tem um domínio válido!\n"); break;
							case -2: printf("O email introduzido não contém @!\n"); break;
							case -3: printf("O email introduzido contém mais do que um @!\n"); break;
							default: printf("Erro não reconhecido!\n");
						}
						printf("Deseja reintroduzir o valor (1) ou voltar ao menu (qualquer outro valor)? -> ");
						read_n_chars(2, buffer);
						int64_t_buffer = str_to_int64_t(buffer);
						if(!(int64_t_buffer==1)) return;
					} strcpy(_temp.email, buffer);
					break;
				case 3:
					while(1){
						printf("Novo Número do cartão de cidadão (8 dígitos): ");
						read_n_chars(9, buffer);
						int64_t_buffer = str_to_int64_t(buffer);
						int8_t found = 0;
						do {
							if(self->alumni_array[found].number_citizen_card == ((uint64_t) int64_t_buffer)){
								printf("Cartão de cidadão coincide com aluno nº%hhu (%s)\n", self->alumni_array[found].number, self->alumni_array[found].name);
								found = -10;
							}
							found++;
						} while((found < self->alumni_array_insert_index) && (found > 0));
						if (found>0){
							found = 0;
							do {
								if(self->instructors_array[found].number_citizen_card == ((uint64_t) int64_t_buffer)){
									printf("Cartão de cidadão coincide com instrutor %s\n", self->instructors_array[found].name);
									found = -10;
								}
								found++;
							} while((found < self->instructors_array_insert_index) && (found > 0));
						}
						if((int64_t_buffer<10000000) || (int64_t_buffer>99999999) || (found<0)){
							printf("Cartão de cidadão inválido.\n");
							printf("Deseja reintroduzir o valor (1) ou voltar ao menu (qualquer outro valor)? -> ");
							read_n_chars(2, buffer);
							int64_t_buffer = str_to_int64_t(buffer);
							if(!(int64_t_buffer==1)) return;
						} else break;
					} _temp.number_citizen_card = (uint64_t) int64_t_buffer;
					break;
				case 4:
					while(1){
						printf("Novo NIF (9 dígitos): ");
						read_n_chars(10, buffer);
						int64_t_buffer = checkNif(buffer);
						uint8_t found = 0;
						if (!int64_t_buffer) {
							int64_t_buffer = str_to_int64_t(buffer);
							do {
								if(self->alumni_array[found].nif == ((uint64_t) int64_t_buffer)) found = found + MAX_ALUMNI;
								found++;
							} while(found < self->alumni_array_insert_index);
							if(found<MAX_ALUMNI) break;
							found = found - MAX_ALUMNI - 1;
							int64_t_buffer = 0;
						}
						printf("NIF inválido: ");
						switch (int64_t_buffer){
							case  0: printf("NIF coincide com aluno nº%hhu (%s)\n", self->alumni_array[found].number, self->alumni_array[found].name); break;
							case -1: printf("O valor introduzido não tem um tamanho de 9 caracteres!\n"); break;
							case -2: printf("O valor introduzido não contém apenas números!\n"); break;
							case -3: printf("O valor introduzido não é válido pelo algoritmo de verificação!\n"); break;
							default: printf("Erro não reconhecido!\n");
						}
						printf("Deseja reintroduzir o valor (1) ou voltar ao menu (qualquer outro valor)? -> ");
						read_n_chars(2, buffer);
						int64_t_buffer = str_to_int64_t(buffer);
						if(!(int64_t_buffer==1)) return;
					} _temp.nif = (uint64_t) int64_t_buffer;
					break;
				case 5:
					printf("Novo Endereço:\n");
					if(read_address_tabed(&_temp.address, buffer)==-1) return;
					break;
				case 6:
					printf("Data de nascimento:\n");
					if(read_date_tabed(&_temp.birth_date, buffer) != 0) return;
					break;
				case 7:
					printf("Já tem carta de condução? [1 -> Sim, Outro Valor -> Não]: ");
					read_n_chars(2, buffer);
					int64_t_buffer = str_to_int64_t(buffer);
					if(int64_t_buffer==1){
						while(1){
							printf("Nº Carta de Condução (max. %d caracteres): ", LEN_DRIVING_LICENSE);
							read_n_chars(LEN_DRIVING_LICENSE+1, buffer);
							if(strlen(buffer)>LEN_DRIVING_LICENSE){
								printf("Nº Carta de Condução inválido.\n");
								printf("Deseja reintroduzir o valor (1) ou voltar ao menu (qualquer outro valor)? -> ");
								read_n_chars(2, buffer);
								int64_t_buffer = str_to_int64_t(buffer);
								if(!(int64_t_buffer==1)) return;
							} else break;
						} strcpy(_temp.number_driving_license, buffer);
						printf("Nova Data de Obtenção da Carta de Condução:\n");
						if(read_date_tabed(&_temp.date_license_completed, buffer) != 0) return;
					} else {
						strcpy(_temp.number_driving_license, "-");
						_temp.date_license_completed.day = 0;
						_temp.date_license_completed.year = 0;
						_temp.date_license_completed.month = 0;
					}
					break;
				default:
					printf("\nOpção Invalida!\n");
					printf("Alteração cancelada.\n");
					return;
			}
			char confirm[3];
			printf("Deseja guardar a alteração (1) ou voltar ao menu (qualquer outro valor)? -> ");
			read_n_chars(2, confirm);
			if(!(str_to_int64_t(confirm)==1)) printf("Alteração cancelada.\n");
			else {
				self->alumni_array[index] = _temp;
				printf("Alteração realizada com sucesso.\n");
			}
			return;
		}
	}
	printf("\nAluno não encontrado!!\n");
	return;
}
void change_alumni_state(SELF *self){
	/*
	Procedimento responsável pela operação opção "Definir Aluno como Ativo ou Inativo" do menu "MENU GESTÃO DE ALUNOS"
	Argumentos:
		SELF *self -> Struct do tipo _self que contém o contexto de chamada.
	Retorno:
		Nenhum
	*/
	cabecalho("ALTERAR ESTADO DE UM ALUNO", CABECALHO_LEN);
    if (self->alumni_array_insert_index == 0) {
        printf("Não existe nenhum aluno registado para alterar o estado.\n");
        return;
    }
	char name_alumni[LEN_NAME+1];
    printf("Nome do Aluno a alterar o estado: ");
	read_n_chars(LEN_NAME, name_alumni);
    for (uint8_t index = 0; index < self->alumni_array_insert_index; index++) {
        if (strcmp(name_alumni, self->alumni_array[index].name) == 0) {
        	cabecalho("NOVO ESTADO", CABECALHO_LEN);
			char _menu[2][CABECALHO_LEN] = {"Ativo", "Inativo"};
			uint8_t choice = menu(_menu, 2, 1);
			if (choice==self->alumni_array[index].active_state) {
				printf("O aluno já se encontra com esse estado selecionado.\nEstado Inalterato.\n");
				return;
			}
			self->alumni_array[index].active_state = choice;
			printf("Estado alterado para '%s' com sucesso.\n", _menu[(choice+1)%2]);
			return;
        }
    }
    printf("Aluno não encontrado.\n");
    return;
}
void print_active_alumni(SELF *self){
	/*
	Procedimento responsável pela operação opção "Listar Alunos Ativos" do menu "MENU GESTÃO DE ALUNOS"
	Argumentos:
		SELF *self -> Struct do tipo _self que contém o contexto de chamada.
	Retorno:
		Nenhum
	*/
	cabecalho("LISTA DE ALUNOS ATIVOS", CABECALHO_LEN);
	if(self->alumni_array_insert_index == 0){
		printf("Não existe nenhum aluno registado para pesquisar.\n");
		return;
	}
	uint8_t couter = 0;
	uint8_t indexes[self->alumni_array_insert_index];
	uint8_t n = self->alumni_array_insert_index;
	for(uint8_t index=0; index<n; index++){
		if(self->alumni_array[index].active_state==1){
			indexes[couter] = index;
			couter++;
		}
	}
	if(couter==0){
		printf("Nenhum aluno ativo encontrado!\n");
		return;
	}
    print_alumni(self, indexes, couter);
    return;
}
void print_alumni_with_license(SELF *self){
	/*
	Procedimento responsável pela operação opção "Definir Aluno Com Carta" do menu "MENU GESTÃO DE ALUNOS"
	Argumentos:
		SELF *self -> Struct do tipo _self que contém o contexto de chamada.
	Retorno:
		Nenhum
	*/
	cabecalho("LISTA DE ALUNOS COM CARTA", CABECALHO_LEN);
	if(self->alumni_array_insert_index == 0){
		printf("Não existe nenhum aluno registado para pesquisar.\n");
		return;
	}
	uint8_t couter = 0;
	uint8_t indexes[self->alumni_array_insert_index];
	uint8_t n = self->alumni_array_insert_index;
	for(uint8_t index=0; index<n; index++){
		if(! (strcmp(self->alumni_array[index].number_driving_license, "-") == 0 || self->alumni_array[index].date_license_completed.day==0 || self->alumni_array[index].date_license_completed.month==0 || self->alumni_array[index].date_license_completed.year==0) ){
			indexes[couter] = index;
			couter++;
		}
	}
	if(couter==0){
		printf("Nenhum aluno com carta encontrado!\n");
		return;
	}
    print_alumni(self, indexes, couter);
    return;
}
void print_alumni_postal_code(SELF *self){
	/*
	Procedimento responsável pela operação opção "Listar Alunos por Código Postal" do menu "MENU GESTÃO DE ALUNOS"
	Argumentos:
		SELF *self -> Struct do tipo _self que contém o contexto de chamada.
	Retorno:
		Nenhum
	*/
	cabecalho("LISTA DE ALUNOS", CABECALHO_LEN);
	if(self->alumni_array_insert_index == 0){
		printf("Não existe nenhum aluno registado para mostrar.\n");
		return;
	}
	char strings[self->alumni_array_insert_index][LEN_POSTAL_CODE+1];
	uint8_t indexes[self->alumni_array_insert_index];
	uint8_t n = self->alumni_array_insert_index;
	for(uint8_t index=0; index<n; index++){
		indexes[index] = index;
		strcpy(strings[index], self->alumni_array[index].address.postal_code);
	}
    char temp[LEN_POSTAL_CODE+1];
    for(uint8_t index = 0; index < n - 1; index++) {
        for(uint8_t j = 0; j < n - index - 1; j++) {
            if (strcmp(strings[j], strings[j + 1]) > 0) {
                strcpy(temp, strings[j]);
                strcpy(strings[j], strings[j + 1]);
                strcpy(strings[j + 1], temp);
                temp[0] = indexes[j];
                indexes[j] = indexes[j + 1];
                indexes[j + 1] = temp[0];
            }
        }
    }
    print_alumni(self, indexes, self->alumni_array_insert_index);
    return;
}
void print_alumni_age(SELF *self){
	/*
	Procedimento responsável pela operação opção "Listar Alunos Ativos" do menu "MENU GESTÃO DE ALUNOS"
	Argumentos:
		SELF *self -> Struct do tipo _self que contém o contexto de chamada.
	Retorno:
		Nenhum
	*/
	cabecalho("LISTAGEM DE ALUNOS POR IDADE", CABECALHO_LEN);
	if(self->alumni_array_insert_index == 0){
		printf("Não existe nenhum aluno registado para pesquisar.\n");
		return;
	}
	uint8_t age_target;
	uint8_t couter;
	uint64_t int64_t_buffer;
	char buffer[5];
	printf("Idade para usar na pesquisa: ");

	read_n_chars(4, buffer);
	int64_t_buffer = str_to_int64_t(buffer);
	if(int64_t_buffer<=0 || int64_t_buffer>=255){
		printf("Idade inválida.\n");
		return;
	}
	age_target = (uint8_t) int64_t_buffer;
	char _my_menu[][CABECALHO_LEN] = {
		"Alunos com idades superiores ou iguais a ",
		"Alunos com idades inferiores ou iguais a ",
	};
	char age_in_string[4];
	my_itoa(age_target, age_in_string, 10);
	strcpy(_my_menu[0]+strlen(_my_menu[0]), age_in_string);
	strcpy(_my_menu[1]+strlen(_my_menu[1]), age_in_string);
	strcpy(_my_menu[0]+strlen(_my_menu[0]), " anos");
	strcpy(_my_menu[1]+strlen(_my_menu[1]), " anos");
	uint8_t escolha_menu;
	escolha_menu = menu(_my_menu, 2, 0);
	char cabecalho_msg[CABECALHO_LEN];
	switch(escolha_menu){
		case 1: strcpy(cabecalho_msg, "ALUNOS SUPERIORES OU IGUAIS A "); break;
		case 2: strcpy(cabecalho_msg, "ALUNOS INFERIORES OU IGUAIS A "); break;
		default: printf("Função ainda não implementada!");
	}
	strcpy(cabecalho_msg+strlen(cabecalho_msg), age_in_string);
	strcpy(cabecalho_msg+strlen(cabecalho_msg), " ANOS");
	cabecalho(cabecalho_msg, CABECALHO_LEN);

	DATE current_date = {1, 1, 0000};
	get_current_date(current_date);
	uint8_t age_alumni;
	couter = 0;

	uint8_t indexes[self->alumni_array_insert_index];
	uint8_t n = self->alumni_array_insert_index;
	for(uint8_t index=0; index<n; index++){
		age_alumni = (current_date.year - self->alumni_array[index].birth_date.year) - 23;
		if(age_alumni>0){
			if( (self->alumni_array[index].birth_date.month > current_date.month) || ((self->alumni_array[index].birth_date.month == current_date.month) && (self->alumni_array[index].birth_date.day > current_date.day))){
				age_alumni--;
			}
			printf("ag: %hhu\n", age_alumni);
			if(escolha_menu == 1){
				if(age_alumni>=age_target){
					indexes[couter] = index;
					couter++;
				}
			} else {
				if(age_alumni<=age_target){
					indexes[couter] = index;
					couter++;
				}
			}
		}
	}
	if(couter==0){
		printf("Nenhum aluno com idade ");
		if(escolha_menu==1) printf("superior");
		else printf("inferior");
		printf(" a %hhu encontrado!\n", age_target);
		return;
	}
    print_alumni(self, indexes, couter);
    return;
}
void print_alumni_alphabeticly(SELF *self){
	/*
	Procedimento responsável pela operação opção "Listar Alunos por Ordem Alfabética" do menu "MENU GESTÃO DE ALUNOS"
	Argumentos:
		SELF *self -> Struct do tipo _self que contém o contexto de chamada.
	Retorno:
		Nenhum
	*/
	cabecalho("LISTA DE ALUNOS", CABECALHO_LEN);
	if(self->alumni_array_insert_index == 0){
		printf("Não existe nenhum aluno registado para mostrar.\n");
		return;
	}
	char strings[self->alumni_array_insert_index][LEN_NAME+1];
	uint8_t indexes[self->alumni_array_insert_index];
	uint8_t n = self->alumni_array_insert_index;
	for(uint8_t index=0; index<n; index++){
		indexes[index] = index;
		strcpy(strings[index], self->alumni_array[index].name);
	}
    char temp[LEN_NAME+1];
    for(uint8_t index = 0; index < n - 1; index++) {
        for(uint8_t j = 0; j < n - index - 1; j++) {
            if (strcmp(strings[j], strings[j + 1]) > 0) {
                strcpy(temp, strings[j]);
                strcpy(strings[j], strings[j + 1]);
                strcpy(strings[j + 1], temp);
                temp[0] = indexes[j];
                indexes[j] = indexes[j + 1];
                indexes[j + 1] = temp[0];
            }
        }
    }
    print_alumni(self, indexes, self->alumni_array_insert_index);
    return;
}
//  Funções Aulas
void print_classes(SELF *self, uint8_t _indexes[], uint8_t len_array){
	/*
	Procedimento que mostra no terminal os dados de uma ou mais aulas, baseado nos argumentos passados
	Formato de impressão:
		----------------------------------------------------------------------
		2000/01/1 12:12
        	Instrutor: Jota
        	Aluno: Diogo (Nº 01)
		----------------------------------------------------------------------
		\n
	Repete quantas vezes necessário.
	Argumentos:
		SELF *self        -> Struct do tipo _self de onde será usado o array "classes_array"
		uint8_t *_indexes -> Endereço do primeiro elemento de um array de uint8_t de onde serão lidos os indexes das aulas a mostrar.
							 Pode ser passado um endereço de memória de um valor simples uint8_t, dado que o valor 1 seja fornecido no argumento len_array
		uint8_t len_array -> Número de elementos tipo uint8_t a ler com base no endereço fornecido
	*/
	for(uint8_t index=0; index<len_array; index++){
		for(uint8_t _index = 0 ; _index<CABECALHO_LEN; _index++) printf("-");
		printf("\n");
		printf("%02hu/%02hhu/%hhu %02hhu:%02hhu\n", self->classes_array[_indexes[index]].class_date.year, self->classes_array[_indexes[index]].class_date.month, self->classes_array[_indexes[index]].class_date.day, self->classes_array[_indexes[index]].class_time.hour, self->classes_array[_indexes[index]].class_time.minutes);
		printf("\tInstrutor: %s (%s)\n", self->classes_array[_indexes[index]].instructor->name, self->classes_array[_indexes[index]].instructor->email);
		printf("\tAluno: %s (Nº %02hhu)\n", self->classes_array[_indexes[index]].alumni->name, self->classes_array[_indexes[index]].alumni->number);
		for(uint8_t _index = 0; _index<CABECALHO_LEN; _index++) printf("-");
		printf("\n\n");
	}
	return;
}
void add_class(SELF *self){
	/*
	Procedimento para adicionar uma aula
	Argumentos:
		SELF *self -> Struct do tipo _self que contém o contexto de chamada.
	Retorno:
		Nenhum
	*/
	cabecalho("REGISTO NOVA AULA", CABECALHO_LEN);
	if(self->classes_array_insert_index>MAX_CLASSES){
		printf("Máximo de aulas atingido!");
		return;
	}
	if (self->alumni_array_insert_index == 0){
		printf("Não existe nenhum aluno registado para marcar uma aula.\n");
		return;
	}
	if (self->instructors_array_insert_index == 0){
		printf("Não existe nenhum instrutor registado para marcar uma aula.\n");
		return;
	}
	CLASS temp_aula;
	char _buffer[LEN_NAME+1];
	int64_t int64_t_buffer;
	uint8_t found_flag=0;
	while(1){
		printf("Nome do instrutor a lecionar a aula: ");
		read_n_chars(LEN_NAME, _buffer);
		for (uint8_t index = 0; index < self->instructors_array_insert_index; index++) {
			if ((strcmp(_buffer, self->instructors_array[index].name) == 0) && (self->instructors_array[index].active_state == 1)){
				found_flag = 1;
				temp_aula.instructor = &self->instructors_array[index];
			}
		}
		if (found_flag!=0) break;
		printf("Nenhum instrutor ativo encontrado com esse nome.\n");
		printf("Deseja reintroduzir o valor (1) ou voltar ao menu (qualquer outro valor)? -> ");
		read_n_chars(2, _buffer);
		int64_t_buffer = str_to_int64_t(_buffer);
		if(!(int64_t_buffer==1)) return;
	}
	found_flag = 0;
	while(1){
		printf("Nome do aluno a ser lecionado: ");
		read_n_chars(LEN_NAME, _buffer);
		for (uint8_t index = 0; index < self->alumni_array_insert_index; index++) {
			if ((strcmp(_buffer, self->alumni_array[index].name) == 0) && (self->alumni_array[index].active_state == 1)){
				found_flag = 1;
				temp_aula.alumni = &self->alumni_array[index];
			}
		}
		if (found_flag!=0) break;
		printf("Nenhum aluno ativo encontrado com esse nome.\n");
		printf("Deseja reintroduzir o valor (1) ou voltar ao menu (qualquer outro valor)? -> ");
		read_n_chars(2, _buffer);
		int64_t_buffer = str_to_int64_t(_buffer);
		if(!(int64_t_buffer==1)) return;
	}
	printf("Data da aula:\n");
	if(read_date_tabed(&temp_aula.class_date, _buffer) != 0) return;
	printf("Hora da aula:\n");
	if(read_time_tabed(&temp_aula.class_time, _buffer) != 0) return;
	for (uint8_t index = 0; index < self->classes_array_insert_index; index++) {
		if(time_diference(temp_aula.class_date, temp_aula.class_time, self->classes_array[index].class_date, self->classes_array[index].class_time) < CLASS_DURATION){
			if(self->classes_array[index].alumni == temp_aula.alumni){
				printf("A aula que tentou marcar é coincidente com uma aula já marcada para o mesmo aluno.\n");
				printf("Voltando ao menu!\n");
				return;
			}
			if (self->classes_array[index].instructor == temp_aula.instructor){
				printf("A aula que tentou marcar é coincidente com uma aula já marcada para o mesmo instrutor.\n");
				printf("Voltando ao menu!\n");
				return;
			}
		}
	}
	self->classes_array[self->classes_array_insert_index] = temp_aula;
	self->classes_array_insert_index++;
	printf("Aula introduzida com sucesso!\n");
	return;
}
void check_classes(SELF *self){
	/*
	Procedimento para preparar para mostrar todas as aulas
	Argumentos:
		SELF *self -> Struct do tipo _self que contém o contexto de chamada.
	Retorno:
		Nenhum
	*/
	cabecalho("CONSULTA DE AULAS", CABECALHO_LEN);
	if(self->classes_array_insert_index==0){
		printf("Nenhuma aula registada para mostrar.\n");
		return;
	}
	uint8_t _mock[self->classes_array_insert_index];
	for(uint8_t _i=0; _i<self->classes_array_insert_index; _i++) _mock[_i] = _i;
	print_classes(self, _mock, self->classes_array_insert_index);
}

// MENUS
void alumin_mngmt(SELF *self){
	/*
	Procedimento para criar e correr o menu de gestão dos alunos
	Argumentos:
		SELF *self -> Struct do tipo _self que contém o contexto de chamada.
	Retorno:
		Nenhum
	*/
	uint8_t _escolha_menu;
	while (1){
		clear_screen();
		cabecalho("MENU GESTÃO DE ALUNOS", CABECALHO_LEN);
		_escolha_menu = menu(alumin_mngmt_menu, len_alumin_mngmt_menu, 1);
		if(_escolha_menu==0) break;
		switch(_escolha_menu){
			case 1: add_alumni(self); break;
			case 2: search_name_alumni(self); break;
			case 3: check_data_alumni(self); break;
			case 4: change_alumni_data(self); break;
			case 5: change_alumni_state(self); break;
			case 6: print_active_alumni(self); break;
			case 7: print_alumni_with_license(self); break;
			case 8: print_alumni_postal_code(self); break;
			case 9: print_alumni_age(self); break;
			case 10: print_alumni_alphabeticly(self); break;
			default: printf("\nFunção ainda não implementada!!\n");
		}
		sleep(3);
	}
	return;
}
void instructor_mngmt(SELF *self){
	/*
	Procedimento para criar e correr o menu de gestão dos instrutores
	Argumentos:
		SELF *self -> Struct do tipo _self que contém o contexto de chamada.
	Retorno:
		Nenhum
	*/
	uint8_t _escolha_menu;
	while (1){
		clear_screen();
		cabecalho("MENU GESTÃO DE INSTRUTORES", CABECALHO_LEN);
		_escolha_menu = menu(instructor_mngmt_menu, len_instructor_mngmt_menu, 1);
		if(_escolha_menu==0) break;
		switch(_escolha_menu){
			case 1: add_instructor(self); break;
			case 2: change_instructor_data(self); break;
			case 3: change_instructor_state(self); break;
			case 4: search_name_instructor(self); break;
			case 5: check_data_instructor(self); break;
			case 6: print_active_instructors(self); break;
			case 7: print_instructors_alphabeticly(self); break;
			default: printf("\nFunção ainda não implementada!!\n");
		}
		sleep(3);
	}
	return;
}
void class_mngmt(SELF *self){
	/*
	Procedimento para criar e correr o menu de gestão das aulas
	Argumentos:
		SELF *self -> Struct do tipo _self que contém o contexto de chamada.
	Retorno:
		Nenhum
	*/
	uint8_t _escolha_menu;
	while (1){
		clear_screen();
		cabecalho("MENU GESTÃO DE AULAS", CABECALHO_LEN);
		_escolha_menu = menu(classes_menu, len_classes_menu, 1);
		if(_escolha_menu==0) break;
		switch(_escolha_menu){
			case 1: add_class(self); break;
			case 2: check_classes(self); break;
			default: printf("\nFunção ainda não implementada!!\n");
		}
		sleep(3);
	}
	return;
}

// DEBUGING
void teste(SELF *self){
	/*
	Procedimento chamado em caso de debugging para popular alguns alunos e instrutores
	Argumentos:
		SELF *self -> Struct do tipo _self que contém o contexto de chamada.
	Retorno:
		Nenhum
	*/
	ALUMNI temp_alun1 = {.number = 1, .name = "Diogo", .email = "joao@joao.pt", .birth_date = {1, 1, 2000}, .nif = 123456789, .number_driving_license = "111111", .active_state = 1};
	ALUMNI temp_alun2 = {.number = 2, .name = "Diogo2", .email = "maria@maria.pt", .birth_date = {2, 2, 2001}, .nif = 987654321, .number_driving_license = "222222", .active_state = 1};
	INSTRUCTOR temp_instr1 = {.name = "Jota", .email = "pedro@pedro.pt", .year_start = 2005, .active_state = 1, .number_citizen_card = 999999999};
	INSTRUCTOR temp_instr2 = {.name = "Jota2", .email = "ana@ana.pt", .year_start = 2008, .active_state = 1, .number_citizen_card = 888888888};
	self->alumni_array[self->alumni_array_insert_index] = temp_alun1;
	self->alumni_array[self->alumni_array_insert_index+1] = temp_alun2;
	self->alumni_array_insert_index += 2;
	self->instructors_array[self->instructors_array_insert_index] = temp_instr1;
	self->instructors_array[self->instructors_array_insert_index+1] = temp_instr2;
	self->instructors_array_insert_index += 2;
}

int32_t main(void){
	/*
	Função primária do programa
	Argumentos:
		Nenhum
	Retorno:
		Nenhum
	*/
    fflush(stdin);
	uint8_t escolha_menu;
	SELF self;
	self.instructors_array_insert_index = 0;
	self.alumni_array_insert_index = 0;
	self.classes_array_insert_index = 0;
	if(DEBUG==1) teste(&self);
	while (1){
		clear_screen();
		cabecalho("MENU PRINCIPAL", CABECALHO_LEN);
		escolha_menu = menu(main_menu, len_main_menu, 1);
		if(escolha_menu==0) break;
		switch(escolha_menu){
			case 1: alumin_mngmt(&self); break;
			case 2: instructor_mngmt(&self); break;
			case 3: class_mngmt(&self); break;
			default: printf("\nFunção ainda não implementada!!\n");
		}
	}
	printf("Saindo...\n");
	return 0;
};
