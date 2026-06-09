#ifndef S21_CAT_H
#define S21_CAT_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Флаги для cat
typedef struct {
  int flag_b; // --number-nonblank: нумеровать только непустые строки
  int flag_e; // отображать концы строк как $
  int flag_E; // без -v отображать концы строк как $
  int flag_n; // --number: нумеровать все строки
  int flag_s; // --squeeze-blank: сжимать пустые строки
  int flag_t; // отображать табы как ^I
  int flag_T; // --show-tabs: без -v отображать табы как ^I
  int flag_v; // показывать непечатаемые символы
} CatFlags;

// Структура для хранения состояния вывода
typedef struct {
  int line_number;
  int prev_was_blank;
  int output_was_squeezed;
} CatState;

// Тип функции обработчика флага
typedef int (*CatFlagHandler)(int argc, char *argv[], int *i, CatFlags *flags);

// Структура для хранения определения флага
typedef struct {
  const char *short_flag;
  const char *long_flag;
  CatFlagHandler handler;
} CatFlagDef;

// Функции
void parse_flags(int argc, char *argv[], CatFlags *flags);
void process_file(const char *filename, const CatFlags *flags, CatState *state);
void print_line(const char *line, const CatFlags *flags, CatState *state);
void process_input(const CatFlags *flags, CatState *state);
int cat_main(int argc, char *argv[]);

#endif // S21_CAT_H
