#include"objdb.h"
#include<stdio.h>
#include<errno.h>
#include<string.h>
#include<stdlib.h>

int main (int argc, char *argv[]) {
	char selection, formatStr[20] = {'\0'};
	int opened = 0, fd1 = 0, result, start = 0, fd2 = 0;
	array_t dbName, fileName, objectName, resultArr;
	
	sprintf(formatStr, "%%%ds", STARTING_SIZE - 1);
	do {
		scanf(" %c", &selection);
		
		switch (selection) {
			case 'o':	
				name_init(&dbName);
				
				scan_array(&dbName);
				
				result = open_base(dbName.name, &opened, &fd1, &fd2);
				
				if (result == -1) {
					fprintf(stderr, "\nError opening %s.\n", dbName.name);
				}
				if (result == -2) {
					fprintf(stderr, "\nInvalid db file %s.\n", dbName.name);		
				}
				
				free(dbName.name);
				
				break;
				
			case 'i':		
				if (opened == 0) {
					fprintf(stderr, "\nNo open db file.\n");
					break;
				}	
						
				name_init(&fileName);
				name_init(&objectName);
				
				scan_array(&fileName);
				
				scan_array(&objectName);
				
				result = import_object(fileName.name, objectName.name, fd1);
				
				if (result == -1) {
					fprintf(stderr, "\nFile %s not found.\n", fileName.name);
				}
				if (result == -2) {
					fprintf(stderr, "\nObject %s already in db.\n", objectName.name);
				}
				if (result == -3) {
					perror("");
					free(fileName.name);
					free(objectName.name);
					return 42;
				}
				
				free(fileName.name);
				free(objectName.name);
				
				break;
			
			case 'f':	
				if (opened == 0) {
					fprintf(stderr, "\nNo open db file.\n");
					break;
				}
			
				name_init(&objectName);
				name_init(&resultArr);
				
				scan_array(&objectName);
					
				start = 0;		
				
				printf("\n##\n");
				while (1) {
					result = find_object(objectName.name, fd1, &resultArr, start, 'f');
					if (result == -1) {
						perror("");
						free(objectName.name);
						free(resultArr.name);
						return 42;
					}
					else if (result == -2) {
						break;
					}
					else {
						printf("%s\n", resultArr.name);
					}

					start = 1;
				}
				
				free(objectName.name);
				free(resultArr.name);
				
				break;
				
			case 'e':
				if (opened == 0) {
					fprintf(stderr, "\nNo open db file.\n");
					break;
				}
				
				name_init(&objectName);
				name_init(&fileName);

				scan_array(&objectName);
				scan_array(&fileName);
				
				result = export_object(objectName.name, fileName.name, fd1);
				
				if (result == -2) {
					fprintf(stderr, "\nCannot open file %s.\n", fileName.name);
					free(objectName.name);
					free(fileName.name);
					break;
				}
				if (result == -1) {
					fprintf(stderr, "\nObject %s not in db.\n", objectName.name);
					free(objectName.name);
					free(fileName.name);
					break;
				}
				if (result == -3) {
					perror("");
					free(objectName.name);
					free(fileName.name);
					return 42;
				}
				
				free(objectName.name);
				free(fileName.name);
				
				break;
				
			case 'd':
				if (opened == 0) {
					fprintf(stderr, "\nNo open db file.\n");
					break;
				}
				
				name_init(&objectName);
				
				scan_array(&objectName);
				
				result = delete_object(objectName.name, fd1, fd2);
				if (result == -1) {
					fprintf(stderr, "\nObject %s not in db.\n", objectName.name);
					free(objectName.name);
					break;
				}
				else if (result == -3) {
					perror("");
					free(objectName.name);
					return 42;
				}
				
				free(objectName.name);
				
				break;
				
			case 'c':
				if (opened == 0) {
					fprintf(stderr, "\nNo open db file.\n");
					break;
				}
				
				result = close_base(&fd1, &fd2, &opened);
				if (result == -1) {
					perror("");
					return 42;
				}
								
				break;
				
			case 'q':
				if (opened == 1) {
					result = close_base(&fd1, &fd2, &opened);
				}
				
				if (result == -1) {
					perror("");
					return 42;
				}
				
				break;
			
			default :
				break;
		}	
	} while (selection != 'q');	
	
	return 0;
}
