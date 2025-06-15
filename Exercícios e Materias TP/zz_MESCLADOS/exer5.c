// Soma de um array de forma recursiva
#include <stdio.h>
#include <stdlib.h>

unsigned int bugger_value(unsigned int* array, unsigned short int length){
	if(length == 1)	return array[0];
	unsigned int lol = bugger_value(array, length-1);
	if (array[length-1] > lol) return array[length-1];
	else return lol;
}

unsigned int little_value(unsigned int* array, unsigned short int length){
	if(length == 1)	return array[0];
	unsigned int lol = little_value(array, length-1);
	if (array[length-1] < lol) return array[length-1];
	else return lol;
}

int main(void){
	unsigned int array[]= {12,1,1,68,54,1,23,4,8,68,4,51,351,3,6,463,54,35,1321,321,31,3543,4};
	unsigned short int length = sizeof(array)/sizeof(array[0]);

	unsigned int sum = bugger_value(array, length);
	printf("Bigger term of the array: %u\n", sum);
	sum = little_value(array, length);
	printf("Lower term of the array: %u\n", sum);
	return 0;
}
