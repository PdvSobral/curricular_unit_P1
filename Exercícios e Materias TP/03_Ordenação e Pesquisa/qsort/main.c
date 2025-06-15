#include <stdio.h>
#include <stdlib.h>
#include <stdlib.h>

int CYCLE=0;
int MAX_CYCLE=50;

void print_int_array(int *array, unsigned int len_array){
	printf("[");
	for(unsigned int index = 0; index < len_array-1; index++) printf("%d, ", array[index]);
	printf("%u]", array[len_array-1]);
}

int* joinArrays(int* arr1, unsigned int size1, int* arr2, unsigned int size2, int* arr3, unsigned int size3, int* result) {
    for(unsigned int i = 0; i < size1; i++) result[i] = arr1[i];
    for(unsigned int i = 0; i < size2; i++) result[size1 + i] = arr2[i];
    for(unsigned int i = 0; i < size3; i++) result[size1 + size2 + i] = arr3[i];
    return result;
}

int* resetArray(int *to_reset, int *contents, unsigned int len_array){
	for(unsigned int i=0; i<len_array; i++) to_reset[i] = contents[i];
	return to_reset;
}

int* quicksort(int *array, unsigned int len_array){
	if(++CYCLE>=MAX_CYCLE){printf("Max Cycles Reached, exiting..."); exit(0);};
	if(len_array<=1) return array;
	int pivot = array[len_array/2];
	int middle[len_array], left[len_array], right[len_array];
	int middle_pos=0, left_pos=0, right_pos=0;
	for(unsigned int c=0; c<len_array; c++){
		if(array[c] == pivot) middle[middle_pos++]=array[c];
		if(array[c] > pivot) right[right_pos++]=array[c];
		if(array[c] < pivot) left[left_pos++]=array[c];
	};
	return joinArrays(
		quicksort(left, left_pos), left_pos,
		middle, middle_pos,
		quicksort(right, right_pos), right_pos,
		array
	);
}

int* bublesort(int *array, unsigned int len_array){
	int temp = 0;
	for(unsigned int index=0; index < len_array-1; index++){
		for(unsigned int index2=index+1; index2 < len_array; index2++){
			CYCLE++;
			if(array[index] > array[index2]){
				temp = array[index2];
				array[index2] = array[index];
				array[index] = temp;
			}
		}
	}
	return array;
}

int32_t main(void){
	printf("------------------------------------------------------------\n");
	printf("|                     ARRAY TO SORT                        |\n");
	printf("------------------------------------------------------------\n");
    int to_order[] = {3, 6, 8, 10, 1, 2, 1, 6, 8, 5, 11, 5, 4};
	unsigned int len_array = 13;
	printf("| Array: ");
	print_int_array(to_order, len_array);
	printf("\t   |\n|\t\t\t\t\t\t\t   |");
    int secondary[len_array+1];
    printf("\n| Correct Sorting:\t\t\t\t\t   |");
    printf("\n| Array: [1, 1, 2, 3, 4, 5, 5, 6, 6, 8, 8, 10, 11]\t   |\n");
	printf("------------------------------------------------------------\n");

	printf("\n");
    printf("------------------------------------------------------------\n");
	printf("|                        QUICK SORT                        |\n");
	printf("------------------------------------------------------------\n");
	resetArray(secondary, to_order, len_array);
    CYCLE = 0;
	printf("Checking telemetry reset...\nCycles: %u\nArray to order: ", CYCLE);
	print_int_array(secondary, len_array);
	printf("\n----------------------- Starting... ------------------------\n");
    quicksort(secondary, len_array);
    printf("Took %d cycles\nFinal array: ", CYCLE);
    print_int_array(secondary, len_array);
	printf("\n------------------------- END ------------------------------\n");

    printf("\n");
    printf("------------------------------------------------------------\n");
	printf("|                        BUBLE SORT                        |\n");
	printf("------------------------------------------------------------\n");
	resetArray(secondary, to_order, len_array);
    CYCLE = 0;
	printf("Checking telemetry reset...\nCycles: %u\nArray to order: ", CYCLE);
	print_int_array(secondary, len_array);
	printf("\n----------------------- Starting... ------------------------\n");
    bublesort(secondary, len_array);
    printf("Took %d cycles\nFinal array: ", CYCLE);
    print_int_array(secondary, len_array);
	printf("\n------------------------- END ------------------------------\n");

    return 0;
}
