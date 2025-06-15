// Soma de um array de forma recursiva
#include <stdio.h>
#include <stdlib.h>

unsigned int soma_recursion(unsigned int* array, unsigned short int length){
	if(length == 1)	return array[0];
	return array[length-1] + soma_recursion(array, length-1);
}

int main(void){
	unsigned int array[]= {12,1,1,68,54,1,23,4,8,68,4,51,351,3,6,463,54,35,1321,321,31,3543,4};
	unsigned short int length = sizeof(array)/sizeof(array[0]);

	unsigned int sum = soma_recursion(array, length);
	printf("Sum of all terms of the array: %u\n", sum);
	return 0;
}
