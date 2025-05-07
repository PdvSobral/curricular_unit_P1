/*
Author: Pedro Sobral
Date: 08-04-2025

Good practices that I like to adhere to.
*/

//modulos que uso sempre
#include <stdio.h>		// printf, scanf
#include <stdlib.h>		// malloc, calloc, alloc
#include <stdint.h>		// uint8_t
#include <string.h>		// strcomp, strlen

// Makes so that the said macros/labels are not allowed in the program, raising errors
// Might be usefull for debugging
#pragma GCC poison int unsigned short strlen size_t

#ifndef function_strlen2
#define function_strlen2
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
#endif

// makes so that if "#define __main__" is not somewhere before this program is compiled, and error occurs, stopping the compiling.
#ifndef __main__
#pragma GCC error "This code is not meant to be compiled directly"
#else
// makes so that this file is only included once
#pragma once
#endif
