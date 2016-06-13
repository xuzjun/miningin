#ifndef FILE_H
#define FILE_H

#include <stdio.h>

#define EXEC_MODE 0111

int file_can_exec(const char *path);
char *read_file(const char *path, size_t *len);

#endif
