#ifndef S21_COMMON_H
#define S21_COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Вывести сообщение об ошибке в stderr
void print_error(const char *filename, const char *message);

// Открыть файл для чтения, вернуть NULL при ошибке
FILE *open_file(const char *filename);

// Закрыть файл и обработать ошибку
void close_file(FILE *file, const char *filename);

#endif // S21_COMMON_H
