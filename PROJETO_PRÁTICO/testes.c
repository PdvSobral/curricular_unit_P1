#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <ctype.h>
#include <stdint.h>
#include <termios.h>    // -> Disabl_ctrl_d
#include <signal.h>		// -> To remap CTRL+C
#include <string.h>		// strcmp, strlens
#include <unistd.h>		// sleep, STDIN_FILENO

#define CABECALHO_LEN 50
#define MAX_PASSWORD_LENGTH 30

uint32_t strlen2(const char *str) {
    uint32_t count = 0;
    uint8_t *s = (uint8_t *)str;
    while (*s != 0x00) {
        if ((*s & 0x80) == 0) {
            // 1-byte character (ASCII)
            count++;
            s++;
        } else if ((*s & 0xE0) == 0xC0) {
            // 2-byte character
            count++;
            s += 2;
        } else if ((*s & 0xF0) == 0xE0) {
            // 3-byte character
            count++;
            s += 3;
        } else if ((*s & 0xF8) == 0xF0) {
            // 4-byte character
            count++;
            s += 4;
        } else {
            // Invalid UTF-8 byte sequence
            break;
        }
    }
    return count;
}

void clear_screen(){
	/*
		Clears the screen and sets the cursor at the upper right corner
	*/
	printf("\033[2J\033[H");
}
void set_cursor(){
	/*
		Save the cursor position to return to later
	*/
	printf("\033[s");
	fflush(stdout);
	return;
}
void reset_cursor(){
	/*
		Restore the cursor position to the previous saved state
	*/
	printf("\033[u");
	fflush(stdout);
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
	my_itoa(_len_to_read, command+strlen2(command), 10);
	strcpy(command+strlen2(command), "[^\n]");
	scanf(command, pointer_to_write_to);
	flush_stdin();
}
void cabecalho(const char msg[], uint8_t len_cabecalho){
	/*
	Procedimento que imprime um cabeçalho
	Argumentos:
		char msg[]            -> String a servir de título.
		uint8_t len_cabecalho -> Tamanho do cabeçalho
	Retorno:
		Nenhum
	*/
	uint8_t _len = strlen2(msg);
	printf("┌");
	for(uint8_t _index = 0; _index<len_cabecalho-2; _index++){
		printf("─");
	} printf("┐\n│");
	_len = (len_cabecalho - _len)/2;
	for(uint8_t _index = 0; _index<_len-1; _index++){
		printf(" ");
	} printf("%s", msg);
	for(uint8_t _index = 0; _index<_len-1; _index++){
		printf(" ");
	} printf("│\n");
	printf("└");
	for(uint8_t _index = 0; _index<len_cabecalho-2; _index++){
		printf("─");
	} printf("┘\n");
	return;
};
void reset_line(){
	set_cursor();
	printf("\033[Am\033[0G");  // up one line and beggining
	printf("├\033[%uC┤\n", CABECALHO_LEN-2);  // replace the line and return to where it was
	reset_cursor();
	return;
};
int64_t str_to_int64_t_flag(char *pointer_to_str, uint8_t *is_numeric){
	/*
	Função que converte um inteiro representado como str para um inteiro
	Argumentos:
		char *pointer_to_str -> Início da str para converter para inteiro
		*uint8_t is_numeric  -> A variável para onde aponta será colocada com 1 se sim, 0 se não
	Retorno:
		int64_t -> inteiro contido na str
	*/
	int64_t _buffer = -1;
	char char_to_evaluate;
	while(1){
		char_to_evaluate = *(pointer_to_str);
		if(char_to_evaluate == 0x00) {*is_numeric=1; break;};
		if(char_to_evaluate < 0x30) {*is_numeric=0; break;};
		if(char_to_evaluate > 0x39) {*is_numeric=0; break;};
		if(_buffer==-1) _buffer=0;
		_buffer*=10;
		_buffer+=(char_to_evaluate - 0x30);
		pointer_to_str++;
	}
	return _buffer;
}


int64_t menu(const char tittle[], uint8_t len_cabecalho, const char menu_options[][CABECALHO_LEN], uint8_t menu_size, uint8_t last_zero){
	/*
	Função para apresentar ao utilizador um menu
	Argumentos:
		char tittle[]							 -> Tittle for the menu
		const char menu_options[][CABECALHO_LEN] -> Array de strings a usar como opções do menu
		uint8_t menu_size                        -> Número de opções a ler do array passado
		uint8_t last_zero                        -> Torna a uĺtima opção sempre zero, independentemente das outras
	Retorno:
		int64_t -> Escolha do utilizador
	*/
	uint8_t wrong=0;
	int64_t _option;
	uint8_t numeric_flag = 0;
	while(1){
		cabecalho(tittle, len_cabecalho);
		reset_line();
		char buffer[5];
		for(uint8_t _index=0; _index<menu_size; _index++) {
			if(_index+last_zero == menu_size){
				printf("│  0 - %s", menu_options[_index]);
			} else {
				printf("│ %2d - %s", _index + 1,  menu_options[_index]);
			}
			printf("\033[%uC│\n", CABECALHO_LEN - 8 - (uint8_t) strlen2(menu_options[_index]));
		};
		printf("├");
		for(uint8_t _index = 0; _index<len_cabecalho-2; _index++){
			printf("─");
		} printf("┤\n");
		printf("\n└");
		for(uint8_t _index = 0; _index<len_cabecalho-2; _index++){
				printf("─");
			} printf("┘");
		printf("\033[1A\033[%dD", CABECALHO_LEN);
		printf("│ Introduza a sua opção: ");
		for (uint8_t i = 0; i < CABECALHO_LEN - 26; ++i) printf(" ");
		printf("│");

		if(wrong==1){
			printf("\n├\033[%uC┤", CABECALHO_LEN-2);
			printf("\n│ \033[31mInvalid Option!! Please enter a valid option.\033[m");
			printf("\033[%uC│\n└", CABECALHO_LEN - 48);
			for(uint8_t _index = 0; _index<len_cabecalho-2; _index++){
				printf("─");
			} printf("┘");
			printf("\033[3A\033[%dD\033[25C", CABECALHO_LEN);
		} else {
			printf("\033[%dD\033[25C", CABECALHO_LEN);
		}
		read_n_chars(3, buffer);
		_option = str_to_int64_t_flag(buffer, &numeric_flag);
		if((_option<=(menu_size-last_zero)) && ((1-last_zero) <= _option) && numeric_flag) break;
		wrong = 1;
		clear_screen();
	}
	printf("\n");
	return _option;
};


char getch() {
    char ch;
    struct termios oldt, newt;
    tcgetattr(STDIN_FILENO, &oldt); 			// Get the current terminal settings
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);			// Disable canonical mode and echo
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);	// Set new terminal settings
    read(STDIN_FILENO, &ch, 1);					// Read a single character from stdin
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);	// Restore the old terminal settings
    return ch;
}
void get_password(char *password_str, uint16_t max_length) {
	fflush(stdout);
    char ch;
    uint16_t current_length = 0;
    while(1) {
        ch = getch();
        if (ch == 0x0A) break;
        else if ((ch == 0x08 || ch == 0x7F) && current_length > 0) { // Backspace and Delete
            current_length--;
            printf("\b \b"); // remove the last char and move back
        }
        else if (current_length < max_length) {
            password_str[current_length++] = ch;
            printf("*"); // Print asterisks for each character
        }
        fflush(stdout);
    }
    password_str[current_length] = 0x00; // Null-terminate the string
    printf("\n"); // Move to the next line after password input
    return;
}


int main(){
	clear_screen();
	cabecalho("LOGIN MENU", CABECALHO_LEN);
	reset_line();
	printf("│ [*] Account ID  :");
	printf("\033[%uC│\n", CABECALHO_LEN - 20);
	printf("│ [ ] Password ID :");
	printf("\033[%uC│\n", CABECALHO_LEN - 20);
	for (uint8_t i = 0; i < CABECALHO_LEN; i++) printf("─");
	printf("\n");
	reset_line();
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
	while (1){
		// chars especiais tipo setas
		if ((ch = getch()) == 27) {
			if ((ch = getch()) == 91) {
				if ((ch = getch()) == 65 && state != 0){  // up arrow pressed
					// printf("↑");
					switch(state){
						case 1: printf("\033[999D\033[3C \033[1A\033[5D\033[3C*\033[%dC", 16 + strlen2(account_id)); break;
						case 2: printf(" \033[1D\033[2A*\033[%dC", 16 + strlen2(password)); break;
						case 3: printf(" \033[13D*\033[1D"); break;
					}
					state--;
				}
				else if (ch == 66 && state != 3){		 // down arrow pressed
					// printf("↓");
					switch(state){
						case 0: printf("\033[999D\033[3C \033[1B\033[5D\033[3C*\033[%dC", 16 + strlen2(password)); break;
						case 1: printf("\033[999D\033[3C \033[2B\033[5D\033[3C*\033[1D"); break;
						case 2: printf(" \033[11C*\033[1D"); break;
					}
					state++;
				}
			}
		}
		else {
			if (state == 0 && strlen2(account_id) < 5){
				// printf("%d", ch);
				if ((ch == 0x08 || ch == 0x7F) && strlen2(account_id) != 0){   //  Delete and backspace
					printf("\033[1D \033[1D");
					account_id[strlen2(account_id)-1] = 0x00;
				} else {
					if (ch >= 0x30 && ch <= 0x39){
						putchar(ch);
						temp = strlen2(account_id);
						account_id[temp] = ch;
						account_id[temp+1] = 0x00;
					}
				}
			}
			if (state == 1 && strlen2(password) < MAX_PASSWORD_LENGTH){
				//printf("%d ", ch);
				if ((ch == 0x08 || ch == 0x7F) && strlen2(password) != 0){   //  Delete and backspace
					printf("\033[1D \033[1D");
					password[strlen2(password)-1] = 0x00;
				} else {
					if (ch >= 0x30 && ch <= 0x39){
						putchar(ch);
						temp = strlen2(account_id);
						account_id[temp] = ch;
						account_id[temp+1] = 0x00;
					}
				}
				if ((int32_t) ch == -61){
					if ((int32_t) (ch = getch()) == -95) {  // á
						printf("á");
						strcpy(password+strlen2(password), "á");
					}
					else if ((int32_t) ch == -89) { // ç
						printf("ç");
						strcpy(password+strlen2(password), "ç");
					}
					else if ((int32_t) ch == -93) { // ã
						printf("ã");
						strcpy(password+strlen2(password), "ã");
					}
					else if ((int32_t) ch == -94) { // â
						printf("â");
						strcpy(password+strlen2(password), "â");
					}
					else if ((int32_t) ch == -87) { // é
						printf("é");
						strcpy(password+strlen2(password), "é");
					}
					else if ((int32_t) ch == -86) { // ê
						printf("ê");
						strcpy(password+strlen2(password), "ê");
					}
					else if ((int32_t) ch == -83) { // í
						printf("í");
						strcpy(password+strlen2(password), "í");
					}
					else if ((int32_t) ch == -77) { // ó
						printf("ó");
						strcpy(password+strlen2(password), "ó");
					}
					else if ((int32_t) ch == -75) { // õ
						printf("õ");
						strcpy(password+strlen2(password), "õ");
					}
					else if ((int32_t) ch == -76) { // ô
						printf("ô");
						strcpy(password+strlen2(password), "ô");
					}
					else if ((int32_t) ch == -70) { // ú
						printf("ú");
					}
					else if ((int32_t) ch == -91) { // Á
						printf("Á");
					}
					else if ((int32_t) ch == -89) { // Ç
						printf("Ç");
					}
					else if ((int32_t) ch == -93) { // Ã
						printf("Ã");
					}
					else if ((int32_t) ch == -94) { // Â
						printf("Â");
					}
					else if ((int32_t) ch == -87) { // É
						printf("É");
					}
					else if ((int32_t) ch == -86) { // Ê
						printf("Ê");
					}
					else if ((int32_t) ch == -83) { // Í
						printf("Í");
					}
					else if ((int32_t) ch == -77) { // Ó
						printf("Ó");
					}
					else if ((int32_t) ch == -75) { // Õ
						printf("Õ");
					}
					else if ((int32_t) ch == -76) { // Ô
						printf("Ô");
					}
					else if ((int32_t) ch == -70) { // Ú
						printf("Ú");
					}

				}
			}
			if (state == 2){
				if (ch == 0x0A){
					if (strlen2(account_id) == 0){
						printf("\n\nNothing in the ID fiel!");
						printf("\033[2A\033[30D\033[3C");
					} else {
						if (strlen2(account_id) < 5){
							printf("\n\nInvalid account ID!");
							printf("\033[2A\033[30D\033[3C");
						} else {
							if (strlen2(password) == 0){
								printf("\n\nNothing in the password field!");
								printf("\033[2A\033[30D\033[3C");
							} else {
								break;
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
	printf("\n\n\n\n\nID: %s\n", account_id);
	return 0;
}