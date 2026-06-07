#ifndef S21_GREP_H
#define S21_GREP_H

#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <regex.h>

#define MAX_PATTERNS 100
#define MAX_PATTERN_LEN 1024

// Структура для хранения пары флаг-значение
typedef struct {
  char flag;
  char value[MAX_PATTERN_LEN];
} FlagValue;

// Флаги для grep
typedef struct {
  int flag_e;  // шаблон поиска
  int flag_i;  // игнорировать регистр
  int flag_v;  // инвертировать поиск
  int flag_c;  // только количество совпадений
  int flag_l;  // только имена файлов
  int flag_n;  // номера строк
  int flag_h;  // без имен файлов
  int flag_s;  // подавить ошибки
  int flag_f;  // шаблоны из файла
  int flag_o;  // только совпадения
} GrepFlags;

// Основная функция grep
int grep_main(int argc, char *argv[]);

// Парсинг флагов и паттернов
int parse_flags(int argc, char *argv[], GrepFlags *flags, char patterns[][MAX_PATTERN_LEN], int *pattern_count);

// Обработка одного файла
void process_file(const char *filename, regex_t *regexes, int pattern_count, GrepFlags *flags, int filename_needed, int total_file_count);

// Поиск и вывод совпадений
void find_matches(FILE *file, const char *filename, regex_t *regexes, int pattern_count, GrepFlags *flags, int filename_needed, int is_single_file);

// Вывод строки
void print_line(const char *line, const char *filename, int line_number, GrepFlags *flags, int filename_needed);

// Освобождение ресурсов
void free_resources(regex_t *regexes, int pattern_count);

#endif  // S21_GREP_H
