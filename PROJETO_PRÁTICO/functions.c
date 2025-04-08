// Makes so that the said macros/labels are not allowed in the program, raising errors
// Might be useful for debugging
#pragma GCC poison int unsigned short
// Makes so that if "#define __main__" is not somewhere before this program is compiled an error ocurs, stopping compilation
#ifndef __main__
#pragma GCC error "This code is not meant to be compiled directly."
#else
// Makes so that this file is only included once
#pragma once
#endif

// Especificações do cliente
#define CABECALHO_LEN 70
#define MAX_ALUMNI 30
#define MAX_INSTRUCTORS 10
#define MAX_CLASSES 100
#define CLASS_DURATION 3600 // in seconds
#define DEBUG 0
#define CLEAR 0

// Especificações do programador
#define LEN_NAME 70
#define LEN_EMAIL 30
#define LEN_POSTAL_CODE 8
#define LEN_STREET 100
#define LEN_LOCATTION 50
#define LEN_DRIVING_LICENSE 10
#define BUFFER_LEN 256 // Mínimo do maior valor dos lens defenidos em cima

// Defenition of the structs
typedef struct _date{
	uint8_t day;
	uint8_t month;
	uint16_t year;
} DATE;
typedef struct _time{
	uint8_t hour;
	uint8_t minutes;
} TIME;
typedef struct _address{
	char street[LEN_STREET+1];
	uint16_t door_number;
	char postal_code[LEN_POSTAL_CODE+1];
	char location[LEN_LOCATTION+1];
} ADDRESS;
typedef struct _alumni{
	uint8_t number;  // apenas está assim pelo máximo ser de 30 alunos
	char name[LEN_NAME+1];
	ADDRESS address;
	char email[LEN_EMAIL+1];
	DATE birth_date;
	uint64_t number_citizen_card;
	uint64_t nif;
	DATE date_license_completed;
	char number_driving_license[LEN_DRIVING_LICENSE+1];
	uint8_t active_state;
} ALUMNI;
typedef struct _instructor{
	uint64_t number_citizen_card;
	char name[LEN_NAME+1];
	char email[LEN_EMAIL+1];
	uint32_t year_start;
	uint8_t active_state;
} INSTRUCTOR;
typedef struct _class{
	DATE class_date;
	TIME class_time;
	ALUMNI *alumni;
	INSTRUCTOR *instructor;
} CLASS;
typedef struct _self{
	INSTRUCTOR instructors_array[MAX_INSTRUCTORS];
	uint8_t instructors_array_insert_index;
	ALUMNI alumni_array[MAX_ALUMNI];
	uint8_t alumni_array_insert_index;
	CLASS classes_array[MAX_CLASSES];
	uint8_t classes_array_insert_index;
} SELF;

void clear_screen(void){
	/*
	Uses escape characters
	*/
	if(CLEAR) printf("\033[H\033[J");
	return;
}
void flush_stdin(void){
	/*
	Usando apenas o fflus(stdin) estava a dar erros em alguns casos e uma performance inconsistente
	Por isso fizemos a nossa própria implementação
	*/
    char c;
    fflush(stdin);
    while((c = getchar()) != '\n' && c != EOF){};
	return;
}
void reverse_str(char *str, uint16_t length){
	/*
	Tal como o nome indica, inverte uma string.
	Recebe como argumentos:
		char *str   -> Pointer para a str a inverter.
		int  length -> Tamanho da str a inverter
	*/
    uint16_t start = 0;
    uint16_t end = length - 1;
    while (start < end) {
        char temp = str[start];
        str[start] = str[end];
        str[end] = temp;
        end--;
        start++;
    }
}
char* my_itoa(int16_t num, char* str, int16_t base){
	/*
	Our implementation of itoa() as it was not being found by the compiler in stdlib.h
	Arguments:
		int16_t num  -> Integer to convert to string
		char*   str  -> Pointer to buffer where the string is going to be stored
		int16_t base -> Base of the integer to convert, E.x. 10
	Returns:
		char* <- Poiter to where string was stored.
	*/
    int16_t i = 0;
    int16_t isNegative = 0;
    /* Handle 0 explicitly, otherwise empty string is saved for 0 */
    if(num == 0){
        str[i++] = '0';
        str[i] = '\0';
        return str;
    }
    // In standard itoa(), negative numbers are handled only with base 10. Numbers are ptherwise considered unsigned.
    if(num < 0 && base == 10){
        isNegative = 1;
        num = -num;
    }
    while (num != 0){
        uint16_t rem = num % base;
        str[i++] = (rem > 9) ? (rem - 10) + 'a' : rem + '0';
        num = num / base;
    }
    if(isNegative == 1) str[i++] = '-';
    str[i] = '\0';
    reverse_str(str, i);
    return str;
}
void read_n_chars(uint8_t _len_to_read, char* pointer_to_write_to){
	/*
	Implementação de uma função parecida com gets, pois esta estava a dar muitos erros de inconsistência
	Argumentos:
		uint8_t _len_to_read        -> Número de caracteres que serão interpretados pelo programa
		char*   pointer_to_write_to -> Onde os characteres lidos serão wuardados.
	*/
	char command[12] = " %";
	my_itoa(_len_to_read, command+strlen(command), 10);
	strcpy(command+strlen(command), "[^\n]");
	scanf(command, pointer_to_write_to);
	flush_stdin();
}

DATE get_current_date(DATE struct_data){
	/*
	Gets current system date and writes it to a passed struct, as well as returning its pointer
	Arguments:
		DATE struct_data -> pointer to first element of struct
	Return:
		DATE -> pointer to first element of struct
	*/
	time_t _time = time(NULL);
	struct tm time_maneger = *localtime(&_time);
	struct_data.year = time_maneger.tm_year + 1900;
	struct_data.month = time_maneger.tm_mon + 1;
	struct_data.day = time_maneger.tm_mday;
	return struct_data;
}
int64_t str_to_int64_t(char *pointer_to_str){
	/*
	Função que converte um inteiro representado como str para um inteiro
	Argumentos:
		char *pointer_to_str -> Início da str para converter para inteiro
	Retorno:
		int64_t -> inteiro contido na str
	*/
	int64_t _buffer = -1;
	char char_to_evaluate;
	while(1){
		char_to_evaluate = *(pointer_to_str);
		if(char_to_evaluate == 0x00) break;
		if(char_to_evaluate < 0x30) break;
		if(char_to_evaluate > 0x3A) break;
		if(_buffer==-1) _buffer=0;
		_buffer*=10;
		_buffer+=(char_to_evaluate - 0x30);
		pointer_to_str++;
	}
	return _buffer;
}
int8_t is_leap(int64_t year){
	/*
	Função para confirmar se um ano é bisexto.
	Argumentos:
		int64_t year -> Ano a analizar
	Retorno:
		uint8_t  0 -> O ano é bisexto
		uint8_t -1 -> O ano não é bisexto
	*/
	if (year % 400 == 0) return -1;
	else if (year % 100 == 0) return 0;
	else if (year % 4 == 0) return -1;
	return 0;
}
int8_t strIsNum(char *str){
	/*
	Esta função verifica se uma string apenas contém dígitos.
	Argumentos:
		const char *str -> Apontador para o início da string terminada com '\0' a analisar
	Retorno:
		int8_t  0 -> A string passada apenas contém dígitos
		int8_t -1 -> A string passada contém pelo menos um caractere que não é dígito
	*/
    for (uint8_t i = 0; str[i] != '\0'; i++) {
        if (!isdigit(str[i])) {
            return -1;
        }
    }
    return 0;
}

void cabecalho(char msg[], uint8_t len_cabecalho){
	/*
	Procedimento que imprime um cabeçalho
	Argumentos:
		char msg[]            -> String a servir de título.
		uint8_t len_cabecalho -> Tamanho do cabeçalho
	Retorno:
		Nenhum
	*/
	uint8_t _len = strlen(msg);
	for(uint8_t _index = 0; _index<len_cabecalho; _index++){
		printf("-");
	} printf("\n");
	_len = (len_cabecalho - _len)/2;
	for(uint8_t _index = 0; _index<_len; _index++){
		printf(" ");
	} printf("%s", msg);
	for(uint8_t _index = 0; _index<_len; _index++){
		printf(" ");
	} printf("\n");
	for(uint8_t _index = 0; _index<len_cabecalho; _index++){
		printf("-");
	} printf("\n");
	return;
};
int64_t menu(const char menu_options[][CABECALHO_LEN], uint8_t menu_size, uint8_t last_zero){
	/*
	Função para apresentar ao utilizador um menu
	Argumentos:
		const char menu_options[][CABECALHO_LEN] -> Array de strings a usar como opções do menu
		uint8_t menu_size                        -> Número de opções a ler do array passado
		uint8_t last_zero                        -> Torna a uĺtima opção sempre zero, independentemente das outras
	Retorno:
		int64_t -> Escolha do utilizador
	*/
	char buffer[5];
	int64_t _option;
	for(uint8_t _index=0; _index<menu_size; _index++) {
		if(_index+last_zero == menu_size) printf(" 0 - %s\n",  menu_options[_index]);
		else printf("%2d - %s\n", _index + 1,  menu_options[_index]);
	}
    while(1) {
        printf("Introduza a sua opção: ");
    	read_n_chars(3, buffer);
    	_option = str_to_int64_t(buffer);
    	if((_option<=(menu_size-last_zero)) && ((1-last_zero) <= _option)) break;
    	printf("Opção inválida!!\n");
    }
    return _option;
};
