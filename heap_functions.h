#ifndef _HEAP_FUNCTIONS_H_
#define _HEAP_FUNCTIONS_H_

#define STARTING_SIZE 4

/* Struct that contains the needed information for the 
 * heap memory strings that represent the file path.*/
typedef struct d_array {
	int size;
	char *name;
} array_t;

void get_size (array_t *nameTable, char selection);

void scan_array (array_t *array);

void name_init (array_t *array);

#endif
