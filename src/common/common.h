#ifndef S21_COMMON_H
#define S21_COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Вывести сообщение об ошибке в stderr
void print_error(const char *filename, const char *message);

// Вывести предупреждение в stderr
void print_warning(const char *filename, const char *message);

// Открыть файл для чтения, вернуть NULL при ошибке
FILE *open_file(const char *filename);

// Закрыть файл и обработать ошибку
void close_file(FILE *file, const char *filename);

// Удалить символы перевода строки из конца строки
void trim_newline(char *str);

// Проверить, является ли строка пустой (содержит только \n)
int is_blank_line(const char *str);

#endif // S21_COMMON_H
