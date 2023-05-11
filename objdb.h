#ifndef _OBJDB_H_
#define _OBJDB_H_

#include"heap_functions.h"

#define MAGIC_NUM "DEADC"
#define NAMESIZE 5
#define TRANSFER_SIZE 512

int open_base (const char dbName[], int *opened, int *fd1, int *fd2);

int import_object (const char fileName[], char objectName[], const int fd);

int find_object (char objectName[], const int fd, array_t *returnArr, const int start, const char selection);

int export_object (char objectName[], const char fileName[], const int fd);

int delete_object (char objectName[], const int fd1, const int fd2);

int close_base (int *fd1, int *fd2, int *opened);

#endif
