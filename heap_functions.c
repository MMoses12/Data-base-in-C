/* Larissa 18/06/2021 Moysis Moysis. */
/* Contains all functions for managing the heap memory arrays. */
#include"heap_functions.h"
#include<stdio.h>
#include<stdlib.h>

// Get the new size of the name table using realloc. 
void get_size (array_t *nameTable, char selection) {
	array_t assistantArr;
	
	//Change the size dedpending the use. 
	if (selection == 'a') {
		assistantArr.size = nameTable->size + STARTING_SIZE;
	}
	else if (selection == 'r') {
		assistantArr.size = nameTable->size - STARTING_SIZE;
	}
	else if (selection == 'c') {
		assistantArr.size = STARTING_SIZE;
	}
	
	assistantArr.name = (char *) realloc(nameTable->name, assistantArr.size * sizeof(char));
	
	if (assistantArr.name != NULL) {
		nameTable->name = assistantArr.name;
		nameTable->size = assistantArr.size;
	}
}

/* Scans the name letter by letter and stores it into
 * a heap memory array. */
void scan_array (array_t *array) {
	char letter;
	int counter = 0;
	
	scanf(" %c", &letter);
	array->name[counter] = letter;
	counter ++;
				
	while (1) {	
		scanf("%c", &letter);
		if (letter == ' ' || letter == '\n') {	
			array->name[counter] = '\0';
			break;
		}
		array->name[counter] = letter;
		counter ++;
		if (array->size == counter) {
			get_size(array, 'a');
		} 
	}
}

/* Initialize the heap memory strings at starting size. */
void name_init (array_t *array) {
	array->name = (char *) calloc(STARTING_SIZE, sizeof(char));
	
	array->size = STARTING_SIZE;
}
