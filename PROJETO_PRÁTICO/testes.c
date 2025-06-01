#define __main__
#define MAX_PASSWORD_LENGTH 20
#include "functions.c"

int main2(){
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
				if ((ch == 0x08 || ch == 0x7F) &&  != 0){   //  Delete and backspace
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
			if (state == 1 && strlen2(password) < MAX_PASSWORD_LENGTH){
				//printf("%d ", ch);
				if ((ch == 0x08 || ch == 0x7F) &&  != 0){   //  Delete and backspace
					if (strlen2(password) > 0){
						printf("\033[1D \033[1D");
						temp = strlen2(password);
						password[temp-1] = 0x00;
					}
				} else {
					if (ch >= 0x20 || (int32_t) ch < 0x00){
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


int main(){
	uint8_t returned;
	returned = main2();
	printf("Returned: %u\n", returned);
	return 0;
}
