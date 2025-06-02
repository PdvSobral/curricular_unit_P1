#define __main__

#include "functions.c"
#include "typedefs.c"
#include "database_helper.c"
#include "md5.c"

#define MAX_PASSWORD_LENGTH 30
const char* USER_DATABASE = "./assets/sys_shadow.csv";
static ACCOUNT CURRENT_LOGIN = {0, 999999999, 2, ""};

uint8_t change_password(){
	char password[MAX_PASSWORD_LENGTH], buffer[MAX_PASSWORD_LENGTH];
	char md5_hash[33];
	while (1) {
		printf("New Password: ");
		get_password(buffer, MAX_PASSWORD_LENGTH);
		strcpy(password, buffer);
		if (strlen2(buffer) > 0 && strlen2(buffer) <= MAX_PASSWORD_LENGTH){
			printf("Confirm Password: ");
			get_password(buffer, MAX_PASSWORD_LENGTH);
			if(strcmp(password, buffer) == 0) break;
			printf("Password is not the same!\n");
			pause_();
		} else {
			printf("Password too long or empty.\n");
			return 1;
		}
	}
	printf("Passwords equal, proceding with overwrite...\n");
	hash_md5(buffer, md5_hash);
	overwrite_password(USER_DATABASE, CURRENT_LOGIN.name_offset, md5_hash);
	printf("Automaticly reloggin in...");  // maoir mentira de sempre, mas pronto
	strcpy(CURRENT_LOGIN.password, md5_hash);
	printf("Update sucessfull!\n");
	return 0;
}

int main(){
	uint8_t returned;
	CURRENT_LOGIN.name_offset=41;
	CURRENT_LOGIN.uid=33641;
	CURRENT_LOGIN.type=0;
	returned = change_password();
	printf("Returned: %u\n", returned);
	print_account_data(&CURRENT_LOGIN);
	return 0;
}
