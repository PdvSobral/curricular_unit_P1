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
#pragma GCC poison int unsigned short

// makes so that if "#define __main__" is not somewhere before this program is compiled, and error occurs, stopping the compiling.
#ifndef __main__
#pragma GCC error "This code is not meant to be compiled directly"
#else
// makes so that this file is only included once
#pragma once
#endif
