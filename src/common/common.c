#include "common.h"

// Вывести сообщение об ошибке в stderr
void print_error(const char *filename, const char *message) {
    fprintf(stderr, "%s: %s\n", filename, message);
}

// Вывести предупреждение в stderr
void print_warning(const char *filename, const char *message) {
    fprintf(stderr, "Warning: %s: %s\n", filename, message);
}

// Открыть файл для чтения, вернуть NULL при ошибке
FILE *open_file(const char *filename) {
    if (filename == NULL) {
        return stdin;
    }
    return fopen(filename, "r");
}

// Закрыть файл и обработать ошибку
void close_file(FILE *file, const char *filename) {
    (void)filename;  // unused parameter
    if (file != NULL && file != stdin) {
        fclose(file);
    }
}

// Удалить символы перевода строки из конца строки
void trim_newline(char *str) {
    if (str == NULL) return;
    size_t len = strlen(str);
    while (len > 0 && (str[len - 1] == '\n' || str[len - 1] == '\r')) {
        str[--len] = '\0';
    }
}

// Проверить, является ли строка пустой (содержит только \n)
int is_blank_line(const char *str) {
    if (str == NULL) return 1;
    while (*str) {
        if (*str != '\n' && *str != '\r' && *str != ' ' && *str != '\t') {
            return 0;
        }
        str++;
    }
    return 1;
}
