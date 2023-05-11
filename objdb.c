/* Larissa 18/06/2021 Moysis Moysis. */
/* This contains the implementations of the 
 * functions for file management. */
#include"objdb.h"
#include<unistd.h>
#include<fcntl.h>
#include<string.h>
#include<errno.h>
#include<sys/stat.h>
#include<stdlib.h>


/* Force read all bytes the user wants. Also has a selection
 * that is used for the proper typecasting. If the use wants
 * to read an int use 'i', if the user wants to read characters
 * use the selection 'c'. Return -1 for fail and the read bytes 
 * for success. */
int read_file (int fd, void *file, int size, char selection) {
	int readNum, readBytes = 0;
	
	do {
		if (selection == 'c') {
			readNum = read(fd, &((char *) file)[readBytes], size - readBytes);
		}
		else if (selection == 'i') {
			readNum = read(fd, ((int *) file), size - readBytes);
		}
		
		if (readNum == 0) {
			break;
		}
		else if (readNum == -1) {
			return -1;
		}
		
		readBytes += readNum;
	} while (readBytes < size);
	
	return (readBytes);
}

/* Force write all bytes the user wants. Also it has a selection
 * that is used for the proper typecasting. If the use wants
 * to write an int use 'i', if the user wants to write characters
 * use the selection 'c'. Return -1 for fail or the written bytes
 * for success. */
int write_object (int fd, void *buf, int size, char selection) {
	int written = 0, writeNum;
	
	do {
		if (selection == 'c') {
			writeNum = write(fd, &((char *) buf)[written], size - written);
		}
		else if (selection == 'i') {
			writeNum = write(fd, ((int *) buf), size - written);
		}
		
		if (writeNum == 0) {
			break;
		}
		if (writeNum == -1) {
			return -1;
		}
		
		written += writeNum;
	} while (written < size);
	
	return (written);
}

/* Close the open database and reset the two fds and the opened flag. 
 * Return 0 for success and -1 for fail.*/
int close_base (int *fd1, int *fd2, int *opened) {
	int result;
	
	result = close(*fd1);
	
	if (result == -1) {
		return -1;
	}
	
	result = close(*fd2);
	if (result == -1) {
		return -1;
	}
	
	*opened = 0;
	*fd1 = 0;
	*fd2 = 0;
	
	return 0;
}

/* Open a file and check if it is database file. If the file
 * does not exist create it. Return -1 for system fail, -2  
 * if the file is not a database file and 0 for success. */
int open_base (const char dbName[], int *opened, int *fd1, int *fd2) {
	char name[NAMESIZE + 1] = {'\0'};
	int readNum = 0, result = 0, helpFd, helpFd2, writeNum;
	struct stat buf;
	
	if (*opened == 1) {
		close_base(fd1, fd2, opened);
		
		result = open_base(dbName, opened, fd1, fd2);
	}
	else if (*opened == 0) {	
		if (access(dbName, F_OK) == 0) {
			helpFd = open(dbName, O_RDWR);
			helpFd2 = open(dbName, O_RDWR);
			
			if (helpFd == -1 || helpFd2 == -1) {
				return -1;
			}
			
			*opened = 1;
			*fd1 = helpFd;
			*fd2 = helpFd2;
			
			result = fstat(*fd1, &buf);
			if (result == -1) {
				return -1;
			}
			if (buf.st_size < NAMESIZE) {
				close_base(fd1, fd2, opened);
				return -2;
			}
			
			readNum = read_file(helpFd, name, NAMESIZE, 'c');
			if (strcmp(name, MAGIC_NUM) != 0 || readNum != NAMESIZE) {
				close_base(fd1, fd2, opened);
				return -2;
			}
			else if (readNum == -1) {
				return -1;
			}	
		}	
		else if (errno == ENOENT) {
			helpFd = open(dbName, O_CREAT | O_RDWR, 0700);
			helpFd2 = open(dbName, O_RDWR); 
			
			if (helpFd == -1 || helpFd2 == -1) {
				return -1;
			}
			
			*opened = 1;
			*fd1 = helpFd;
			*fd2 = helpFd2;
			
			writeNum = write_object(helpFd, MAGIC_NUM, NAMESIZE, 'c');
			if (writeNum == -1) {
				return -1;
			}
		}
		else {
			return -1;
		}
	}

	return 0;
}

/* Import an object in database. Return -3 for system failure, 
 * -1 for error opening, -2 if the object is already in database
 * and 0 for success. */
int import_object (const char fileName[], char objectName[], const int fd) {
	int result, fileDesc, nameSize, readNum, objectSize;
	struct stat buf;
	char transferArr[TRANSFER_SIZE] = {'\0'}, stopChar = '\0'; 
	array_t helpArr;
	
	helpArr.name = (char *) calloc(STARTING_SIZE, sizeof(char));
	if (helpArr.name == NULL) {
		return -3;
	}
	helpArr.size = STARTING_SIZE;
	
	if (access(fileName, F_OK) == 0) {
		fileDesc = open(fileName, O_RDWR);
		if (fileDesc == -1) {
			free(helpArr.name);
			return -1;
		}
	}
	else {
		free(helpArr.name);
		return -1;
	}	
	
	result = find_object(objectName, fd, &helpArr, 0, 'o');
	if (result > 0) {
		free(helpArr.name);
		return -2;
	}
	
	lseek(fd, 0, SEEK_END);
	result = fstat(fileDesc, &buf);
	if (result == -1) {
		free(helpArr.name);
		return -3;
	}
	
	nameSize = strlen(objectName) + 1;
	result = write_object(fd, &nameSize, sizeof(int), 'i');
	if (result == -1) {
		free(helpArr.name);
		return -3;
	}
	
	result = write_object(fd, objectName, strlen(objectName), 'c');
	if (result == -1) {
		free(helpArr.name);
		return -3;
	}
	
	result = write_object(fd, &stopChar, sizeof(char), 'c');
	if (result == -1) {
		free(helpArr.name);
		return -3;
	}
	
	objectSize = buf.st_size;
	result = write_object(fd, &objectSize, sizeof(int), 'i');
	if (result == -1) {
		free(helpArr.name);
		return -3;
	}
	
	do {
		readNum = read_file(fileDesc, transferArr, TRANSFER_SIZE, 'c');	
		if (result == -1) {
			free(helpArr.name);
			return -3;
		}
		else if (result == 0) {
			break;
		}
		
		result = write_object(fd, transferArr, readNum, 'c');
		if (result == -1) {
			free(helpArr.name);
			return -3;
		}

	} while (1);
	
	free(helpArr.name);
	
	return 0;
}

/* Find the start of the object. This function stops right
 * after the object's name. Return -3 for system fail and -2
 * for end of file. */
int search_object (const int fd, array_t *nameArr) {
	int nameSize, result;
	
	result = read_file(fd, &nameSize, sizeof(int), 'i');
	if (result == -1) {
		return -3;
	}
	else if (result == 0) {
		return -2;
	}
		
	if (nameArr->size < nameSize) {
		nameArr->size = nameSize - STARTING_SIZE;
		get_size(nameArr, 'a');
	}	
	
	result = read_file(fd, nameArr->name, nameSize, 'c');
	if (result == -1) {
		return -3;
	}
	else if (result == 0) {
		return -2;
	}
	
	return (nameSize);
}

/* Pass the object. This function stops right after the object's content.
 * Return -3 for system fail and -2 for end of file.*/
int pass_object (const int fd, char name[]) {
	int objSize, result;
	
	result = read_file(fd, &objSize, sizeof(int), 'i');
	if (result == -1) {
		return -3;
	}
	else if (result == 0) {
		return -2;
	}
	lseek(fd, objSize, SEEK_CUR); 
	
	return 0;	
} 

/* Find an object in database. Return -1 for system fail, 
 * -2 if the object is not in the database or the size of
 * the object's name if the object is found. */
int find_object (char objectName[], const int fd, array_t *returnArr, const int start, const char selection) {		
	int nameSize;
	
	if (start == 0) {
		lseek(fd, NAMESIZE, SEEK_SET);
	}
	
	if (strcmp(objectName, "*") != 0 || selection == 'o') {	
		do {
			nameSize = search_object(fd, returnArr);
			
			if (nameSize == -3) {
				return -1;
			}
			else if (nameSize == -2) {
				return -2;
			}
			else if (selection == 'o' && strcmp(returnArr->name, objectName) == 0) {
				return (nameSize);
			}
			else if (selection == 'f' && strstr(returnArr->name, objectName) != NULL) {
				pass_object(fd, returnArr->name);
				return (nameSize);
			}

			
			pass_object(fd, returnArr->name);
		} while (1);
	}
	else {
		nameSize = search_object(fd, returnArr);
		
		pass_object(fd, returnArr->name);
		
		if (nameSize == -2) {
			return -2;
		}
		
		return (nameSize);
	}
}

/* Export an object from database to a new created file. 
 * Return -3 for system fail, -2 if the file is already
 * existing or 0 for success. */
int export_object (char objectName[], const char fileName[], const int fd) {
	int fileDesc, objSize, result, readNum, endComm;
	char helpArr[TRANSFER_SIZE] = {'\0'};
	array_t resultArr;
	
	resultArr.size = STARTING_SIZE;
	resultArr.name = (char *) calloc(STARTING_SIZE, sizeof(char));
	if (resultArr.name == NULL) {
		return -3;
	}
	
	if (access(fileName, F_OK) == 0) {
		return -2;	
	}
	else if (errno == ENOENT) {				
		fileDesc = open(fileName, O_CREAT | O_RDWR, 0700);
		if (fileDesc == -1) {
			free(resultArr.name);
			return -2;
		}
		
		result = find_object(objectName, fd, &resultArr, 0, 'o');
		if (result == -2) {
			free(resultArr.name);
			result = unlink(fileName);
			if (result == -1) {
				return -3;
			}
			
			return -1;
		} 
		else if (result == -3) {
			free(resultArr.name);
			return -3;
		}
		
		result = read_file(fd, &objSize, sizeof(int), 'i');
		if (result == -1) {
			free(resultArr.name);
			return -3;
		}
		
		do {
			if (objSize > TRANSFER_SIZE) {
				endComm = TRANSFER_SIZE;
				objSize -= TRANSFER_SIZE;
			}
			else {
				endComm = objSize;
				objSize = 0;
			}
			
			readNum = read_file(fd, &helpArr, endComm, 'c');
			if (readNum == -1) {
				free(resultArr.name);
				return -3;
			}
			else if (readNum == 0) {
				break;
			}
			
			result = write_object(fileDesc, &helpArr, readNum, 'c');
			if (result == -1) {
				free(resultArr.name);
				return -3;
			}
		} while(1);
	}
	
	free(resultArr.name);
	
	return 0;
}

/* Delete an object from database. Return -3 for system fail,
 * -1 if the object is not found or 0 for success. */
int delete_object (char objectName[], int fd1, int fd2) {
	int result, objSize, nameSize, readNum, fileSize, totalObjSize;
	char helpArr[TRANSFER_SIZE] = {'\0'};
	struct stat buf;
	array_t resultArr;
	
	resultArr.size = STARTING_SIZE;
	resultArr.name = (char *) calloc(STARTING_SIZE, sizeof(char));
	if (resultArr.name == NULL) {
		return -3;
	}
	
	nameSize = find_object(objectName, fd1, &resultArr, 0, 'o');
	if (nameSize == -2) {
		free(resultArr.name);
		return -1;
	}
	else if (nameSize == -1) {
		free(resultArr.name);
		return -3;
	}
	
	find_object(objectName, fd2, &resultArr, 0, 'o');	
	lseek(fd2, - (nameSize + sizeof(int)), SEEK_CUR);
	
	result = read_file(fd1, &objSize, sizeof(int), 'i');
	if (result == -1) {
		free(resultArr.name);
		return -3;
	}
	lseek(fd1, objSize, SEEK_CUR);
	
	do {		
		readNum = read_file(fd1, helpArr, TRANSFER_SIZE, 'c');	
		if (result == -1) {
			free(resultArr.name);
			return -3;
		}
		else if (result == 0) {
			break;
		}
		
		result = write_object(fd2, helpArr, readNum, 'c');
		if (result == -1) {
			free(resultArr.name);
			return -3;
		}
	} while (1);	
	
	result = fstat(fd1, &buf);
	if (result == -1) {
		return -3;
	}
	
	fileSize = buf.st_size;
	totalObjSize = nameSize + 2 * sizeof(int) + objSize;
	ftruncate(fd1, fileSize - totalObjSize);
	
	free(resultArr.name);
	
	return 0;
}
