#include <stdio.h>
#include <stdlib.h>
#include <stdlib.h>
#include <string.h>

#define CABECALHO_LEN 70
#define FLOAT_LENGHT 12
#define MAX_LEN_NAME 70
#define MAX_EMPLOYEES 70
#define DEBUG 0

#define BUFFER_LEN 256 // Mínimo do maior valor dos lens defenidos em cima


typedef struct employee {
	uint8_t number_employee;
	char name[MAX_LEN_NAME+1];
	uint64_t nif;
	float salary;
} EMPLOYEE;
typedef struct _self{
	EMPLOYEE employee_array[MAX_INSTRUCTORS];
	uint8_t employee_array_insert_index;
} SELF;

//predone
void cabecalho(char msg[], uint8_t len_cabecalho){
	/*
	Procedimento que imprime um cabeçalho
	Argumentos:
		char msg[]            -> String a servir de título.
		uint8_t len_cabecalho -> Tamanho do cabeçalho
	Retorno:
		Nenhum
	*/
	unsigned short _len = strlen(msg);
	for(unsigned short _index = 0; _index<len_cabecalho; _index++){
		printf("-");
	} printf("\n");
	_len = (len_cabecalho - _len)/2;
	for(unsigned short _index = 0; _index<_len; _index++){
		printf(" ");
	} printf("%s", msg);
	for(unsigned short _index = 0; _index<_len; _index++){
		printf(" ");
	} printf("\n");
	for(unsigned short _index = 0; _index<len_cabecalho; _index++){
		printf("-");
	} printf("\n");
	return;
};


// Prototypes
int8_t checkNif(char *str);


int32_t main(int32_t argc, char *argv[]){
    // code
    return 0;
}


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


int is_valid_float(const char *str) {
    char *endptr;

    if (str == NULL || *str == '\0') return 0; // String is empty

	if (strlen(buffer) > FLOAT_LENGTH-1) return -4;


    // Replace comma (if found) with point
    char temp[FLOAT_LENGTH];
    strncpy(temp, str, FLOAT_LENGTH);
    for (int i = 0; temp[i] != '\0'; i++) if (temp[i] == ',') { temp[i] = '.'; break;}

    // Use strtof to convert the string to a float
    float value = strtof(temp, &endptr);

    // Check if the entire string was consumed and if the value is valid
    if (*endptr != '\0') return -1; // Invalid characters present
    if (value < 0) return -2; // Negative value
    if (value == 0 && strcmp(temp, "0") != 0) return -3; // Not a valid float representation

    return 1; // Valid float
}


void add_employee(SELF *self){
	/*
	Procedimento para adicionar um novo empregado
	Argumentos:
		SELF *self -> Struct do tipo _self que contém o contexto de chamada.
	Retorno:
		Nenhum
	*/
	cabecalho("REGISTO DE UM NOVO EMPREGADO", CABECALHO_LEN);
	if(self->employee_array_insert_index => MAX_ALUMNI){
		printf("Máximo de alunos atingido!\n");
		return;
	}

	EMPLOYEE _temp;
	char buffer[BUFFER_LEN+1];
	int64_t int64_t_buffer;

	while(1){
		printf("Nome Do Empregado: ");
		read_n_chars(LEN_NAME, buffer);
		break;
	} strcpy(_temp.name, buffer);

	while(1){
		printf("NIF (9 dígitos): ");
		read_n_chars(10, buffer);
		int64_t_buffer = checkNif(buffer);
		uint8_t found = 0;
		if (!int64_t_buffer) {
			int64_t_buffer = str_to_int64_t(buffer);
			do {
				if(self->employee_array[found].nif == ((uint64_t) int64_t_buffer)) found = found + MAX_EMPLOYEES;
				found++;
			} while(found < self->employee_array_insert_index);
			if(found<MAX_EMPLOYEES) break;
			found = found - MAX_EMPLOYEES - 1;
			int64_t_buffer = 0;
		}
		printf("NIF inválido: ");
		switch (int64_t_buffer){
			case  0: printf("NIF coincide com empregado nº%hhu (%s)\n", self->alumni_array[found].number, self->alumni_array[found].name); break;
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

	while (1) {
        printf("\tSalário (float): ");
        read_n_chars(FLOAT_LENGTH + 1, buffer);
        int64_t_buffer = is_valid_float(buffer);
        if (int64_t_buffer == 1) break;
        printf("Salário inválido: ");
        switch (int64_t_buffer) {
            case 0: printf("A string introduzida encontra-se vazia!\n"); break;
            case -1: printf("Erro: O valor introduzido contém caracteres inválidos!\n"); break;
            case -2: printf("Erro: O valor introduzido não pode ser negativo!\n"); break;
            case -3: printf("Erro: O valor introduzido não é uma representação válida de um float!\n"); break;
            case -4: printf("Erro: Valor demasiado grande!"); break;
            default: printf("Erro não reconhecido!\n");
        }
        printf("Deseja reintroduzir o valor (1) ou voltar ao menu (qualquer outro valor)? -> ");
        read_n_chars(2, buffer);
        int64_t_buffer = str_to_int64_t(buffer);
        if (!(int64_t_buffer == 1)) return -1;
    } _temp.salary = (float)  strtof(temp, buffer);

	printf("Deseja mesmo guardar os dados introduzidos?\n\t[1 -> Sim, Outro Valor -> Não] -> ");
	read_n_chars(2, buffer);
	int64_t_buffer = str_to_int64_t(buffer);
	if(!(int64_t_buffer==1)) return;
	_temp.number = self->employee_array_insert_index + 1;
	self->employee_array[self->employee_array_insert_index] = _temp;
	self->employee_array_insert_index++;
	printf("Dados Guardados Com Sucesso!\n");
	return;
}

/*
TODO:ierulghoier7 udsybhgoerilfkjxchbglearukdsjhxb fnlicESKZJ CLM IKDNHXGBVL KQ,Ujkashm flnk,reikdyhafglk,iyhk k
TODO:ierulghoier7 udsybhgoerilfkjxchbglearukdsjhxb fnlicESKZJ CLM IKDNHXGBVL KQ,Ujkashm flnk,reikdyhafglk,iyhk k
TODO:ierulghoier7 udsybhgoerilfkjxchbglearukdsjhxb fnlicESKZJ CLM IKDNHXGBVL KQ,Ujkashm flnk,reikdyhafglk,iyhk k
*/
