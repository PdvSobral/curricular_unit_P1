#define __main__
#define MAX_PASSWORD_LENGTH 30
const char* USER_DATABASE = "./assets/sys_shadow.csv";

#include "functions.c"
#include "typedefs.c"
#include "database_helper.c"
#include "md5.c"

uint8_t regist(){
	clear_screen();
	cabecalho("REGISTER ACCOUNT", CABECALHO_LEN);
	reset_line(CABECALHO_LEN);
	printf("│ [*] Account ID:");
	printf("\033[%uC│\n", CABECALHO_LEN - 18);
	printf("│ [ ] Password:");
	printf("\033[%uC│\n", CABECALHO_LEN - 16);
	printf("│ [ ] Retype Password:");
	printf("\033[%uC│\n", CABECALHO_LEN - 23);
	for (uint8_t i = 0; i < CABECALHO_LEN; i++) printf("─");
	printf("\n");
	reset_line(CABECALHO_LEN);
	printf("│ [ ] Login   [ ] Cancel");
	printf("\033[%uC│\n", CABECALHO_LEN - 25);
	printf("└");
	for(uint8_t _index = 0; _index<CABECALHO_LEN-2; _index++){
		printf("─");
	} printf("┘");
	printf("\033[5A\033[999D\033[18C");
	fflush(stdout);
	uint8_t state=0; //0 - account is  | 1 - password id   | login 'button'
	uint8_t temp;
	char ch;
	char account_id[6]; account_id[0] = 0x00;
	char password[MAX_PASSWORD_LENGTH + 1]; password[0] = 0x00;
	char password2[MAX_PASSWORD_LENGTH + 1]; password2[0] = 0x00;
	char md5_hash[33];
	while (1){
		// chars especiais tipo setas
		if (((ch = getch()) == 27) && ((ch = getch()) == 91)) {
			if (((ch = getch()) == 65 || ch == 68 )&& state != 0){  // up arrow pressed
				// printf("↑");
				switch(state){
					case 1: printf("\033[999D\033[3C \033[1A\033[5D\033[3C*\033[%dC", 14 + strlen2(account_id)); break;
					case 2: printf("\033[999D\033[3C \033[1A\033[5D\033[3C*\033[%dC", 12 + strlen2(password)); break;
					case 3: printf(" \033[1D\033[2A*\033[%dC", 19 + strlen2(password2)); break;
					case 4: printf(" \033[13D*\033[1D"); break;
				}
				state--;
			}
			else if ((ch == 66 || ch == 67) && state != 4){		 // down arrow pressed
				// printf("↓");
				switch(state){
					case 0: printf("\033[999D\033[3C \033[1B\033[5D\033[3C*\033[%dC", 12 + strlen2(password)); break;
					case 1: printf("\033[999D\033[3C \033[1B\033[5D\033[3C*\033[%dC", 19 + strlen2(password2)); break;
					case 2: printf("\033[999D\033[3C \033[2B\033[5D\033[3C*\033[1D"); break;
					case 3: printf(" \033[11C*\033[1D"); break;
				}
				state++;
			}
		}
		else {
			if (state == 0 && strlen2(account_id) <= 5){
				// printf("%d", ch);
				if ((ch == 0x08 || ch == 0x7F)){   //  Delete and backspace
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
				if ((ch == 0x08 || ch == 0x7F)){   //  Delete and backspace
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
			if (state == 2 && strlen2(password2) <= MAX_PASSWORD_LENGTH){
				//printf("%d ", ch);
				if ((ch == 0x08 || ch == 0x7F)){   //  Delete and backspace
					if (strlen2(password2) > 0){
						printf("\033[1D \033[1D");
						temp = strlen2(password2);
						password2[temp-1] = 0x00;
					}
				} else {
					if ((ch >= 0x20 || (int32_t) ch < 0x00) && strlen2(password2) < MAX_PASSWORD_LENGTH){
						putchar('*');
						temp = strlen2(password2);
						password2[temp] = ch;
						password2[temp+1] = 0x00;
					}
				}
			}
			if (state == 3){
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
								if (strcmp(password, password2) != 0){
									printf("\n\n");
									reset_line(CABECALHO_LEN);
									print_between_format("Passwords do not match!", "\033[31m", CABECALHO_LEN, 1);
									print_bottom(CABECALHO_LEN, 1);
									printf("\033[4A\033[3C");
								} else {
									ACCOUNT* my_user = get_user_by_id(USER_DATABASE, account_id);
									if (my_user == NULL){
										free(my_user);
										printf("\n\n");
										reset_line(CABECALHO_LEN);
										print_between_format("User does not exist!", "\033[32m", CABECALHO_LEN, 1);
										print_bottom(CABECALHO_LEN, 1);
										printf("\033[4A\033[3C");
									} else {
										printf("\n\n");
										reset_line(CABECALHO_LEN);
										print_between_format("User already exists!", "\033[31m", CABECALHO_LEN, 1);
										print_bottom(CABECALHO_LEN, 1);
										printf("\033[4A\033[3C");
									}
								}
							}
						}
					}
				}
			}
			if (state == 4){
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

/*
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
*/

int main(){
	uint8_t returned;
	returned = regist(0);
	printf("Returned: %u\n", returned);
	return 0;
}
