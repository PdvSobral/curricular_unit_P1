#define __main__
#include "functions.c"
/*
int main2(){
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
*/

uint8_t confirmation_with_cabecalho(char* cabecalho_msg, char* main_msg, uint8_t len_cabecalho){
	cabecalho(cabecalho_msg, len_cabecalho);
	reset_line(len_cabecalho);
	printf("│ %s", main_msg);
	printf("\033[%uC│\n", len_cabecalho - 3 - strlen2(main_msg));
	for (uint8_t i = 0; i < len_cabecalho; i++) printf("─");
	printf("\n");
	reset_line(len_cabecalho);
	printf("│ [*] Yes     [ ] No");
	printf("\033[%uC│\n", len_cabecalho - 21);
	printf("└");
	for(uint8_t _index = 0; _index<len_cabecalho-2; _index++){
		printf("─");
	} printf("┘");
	printf("\033[1A\033[999D\033[3C");
	fflush(stdout);
	uint8_t state=0; //0 - account is  | 1 - password id   | login 'button'
	char ch;
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
		else {
			if (ch == 0x0A){
				printf("\n\n");
				return state;
			}
		}
		fflush(stdout);
	}
}

int main(){
	uint8_t returned;
	returned = confirmation_with_cabecalho("TEMP", "Do you wish to retype the value?", CABECALHO_LEN);
	printf("Returned: %u\n", returned);
	return 0;
}
