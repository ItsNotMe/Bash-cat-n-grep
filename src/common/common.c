#include "common.h"

// Вывести сообщение об ошибке в stderr
void print_error(const char *filename, const char *message) {
  fprintf(stderr, "%s: %s\n", filename, message);
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
  (void)filename;  // неиспользуемый параметр
  if (file != NULL && file != stdin) {
    fclose(file);
  }
}
