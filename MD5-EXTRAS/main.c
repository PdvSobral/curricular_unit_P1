#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "md5.h"

void print_hash_in_hex(uint8_t hash_start[16]){
    for(uint8_t i = 0; i < 16; ++i) printf("%02x", hash_start[i]);
    printf("\n");
}

void save_hash_in_hex(uint8_t hash_start[16], char str_to_save_hash_to[33]){
    for(uint8_t i = 0; i < 16; ++i){
        sprintf(&str_to_save_hash_to[i * 2], "%02x", hash_start[i]);
    }
    str_to_save_hash_to[32] = 0x00;
}

void save_hash_in_hex_manual(uint8_t hash_start[16], char str_to_save_hash_to[33]) {
    const char hex_chars[] = "0123456789abcdef";
    for (uint8_t i = 0; i < 16; ++i) {
        uint8_t byte = hash_start[i];
        // Convert the first 4 bits of the byte
        str_to_save_hash_to[i * 2] = hex_chars[byte>>4];
        // Convert the last 4 bits of the byte
        str_to_save_hash_to[i * 2 + 1] = hex_chars[byte & 0x0F];
    }
    str_to_save_hash_to[32] = '\0';
}

int main(){
	char* to_hash = "password";
	char str_hash[33];  // 32 + 0x00
    uint8_t result[16];
	md5String(to_hash, result);
	save_hash_in_hex_manual(result, str_hash);
	printf("Hash: %s\n", str_hash);
    /*
    md5File(stdin, result);
    print_hash_in_hex(result);
    */
    return 0;
}
