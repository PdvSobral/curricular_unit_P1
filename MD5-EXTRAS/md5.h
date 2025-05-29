#ifndef MD5_H
#define MD5_H

#include "md5.c"

MD5Context* md5Init();
void md5Update(MD5Context *ctx, uint8_t *input, size_t input_len);
void md5Finalize(MD5Context *ctx);
void md5Step(uint32_t *buffer, uint32_t *input);

void md5String(char *input, uint8_t result[16]);
void md5File(FILE *file, uint8_t result[16]);

#endif