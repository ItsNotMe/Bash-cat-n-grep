#include "grep.h"
#include "common.h"

#define MAX_PATTERNS 100
#define MAX_PATTERN_LEN 1024
#define MAX_LINE_LEN 4096

// Предварительное объявление функций
int matches_pattern(const char *line, regex_t *regexes, int pattern_count, GrepFlags *flags);
void free_resources(regex_t *regexes, int pattern_count);
void print_line(const char *line, const char *filename, int line_number, GrepFlags *flags, int filename_needed);
void find_matches(FILE *file, const char *filename, regex_t *regexes, int pattern_count, GrepFlags *flags, int filename_needed, int is_single_file);
void process_file(const char *filename, regex_t *regexes, int pattern_count, GrepFlags *flags, int filename_needed, int total_file_count);

// Освобождение ресурсов
void free_resources(regex_t *regexes, int pattern_count) {
  for (int i = 0; i < pattern_count; i++) {
    regfree(&regexes[i]);
  }
}

// Вывод строки с учетом флагов
void print_line(const char *line, const char *filename, int line_number, GrepFlags *flags, int filename_needed) {
  // Выводим имя файла, если не -h и есть имя файла и нужно имя файла
  if (!flags->flag_h && filename_needed && filename && strlen(filename) > 0) {
    printf("%s:", filename);
  }

  // Выводим номер строки, если -n
  if (flags->flag_n) {
    printf("%d:", line_number);
  }

  printf("%s", line);
}

// Проверка, соответствует ли строка какому-либо паттерну
int matches_pattern(const char *line, regex_t *regexes, int pattern_count, GrepFlags *flags) {
  (void)flags;  // unused parameter
  for (int i = 0; i < pattern_count; i++) {
    if (regexec(&regexes[i], line, 0, NULL, 0) == 0) {
      return 1;
    }
  }
  return 0;
}

// Основная функция поиска совпадений
void find_matches(FILE *file, const char *filename, regex_t *regexes, int pattern_count, GrepFlags *flags, int filename_needed, int is_single_file) {
  char *line = NULL;
  size_t line_size = 0;
  ssize_t line_len;
  int line_number = 0;
  int match_count = 0;

  while ((line_len = getline(&line, &line_size, file)) != -1) {
    line_number++;

    // Ищем совпадение
    int match = matches_pattern(line, regexes, pattern_count, flags);

    // Инвертирование результата
    if (flags->flag_v) {
      match = !match;
    }

    // Если есть совпадение
    if (match) {
      if (flags->flag_c) {
        match_count++;
      } else if (flags->flag_l) {
        printf("%s\n", filename);
        break;
      } else {
        print_line(line, filename, line_number, flags, filename_needed);
      }
    }
  }

  // Выводим количество совпадений, если -c
  if (flags->flag_c) {
    // Имя файла не выводится при -c если это единственный файл, и не указаны -h и -n
    if (!flags->flag_h && !is_single_file && filename && strlen(filename) > 0) {
      printf("%s:", filename);
    }
    printf("%d\n", match_count);
  }

  free(line);
}

// Обработка одного файла
void process_file(const char *filename, regex_t *regexes, int pattern_count, GrepFlags *flags, int filename_needed, int total_file_count) {
  FILE *file = open_file(filename);
  char display_name[4096] = {0};

  if (filename == NULL) {
    strcpy(display_name, "");
  } else {
    if (file == NULL) {
      if (!flags->flag_s) {
        print_error(filename, "No such file or directory");
      }
      return;
    }
    strcpy(display_name, filename);
  }

  // Определяем, это единственный файл или нет
  int is_single_file = (total_file_count == 1);

  // Если нужно вывести только имена файлов
  if (flags->flag_l) {
    find_matches(file, display_name, regexes, pattern_count, flags, filename_needed, is_single_file);
  } else {
    // Сброс позиции файла
    if (filename) {
      rewind(file);
    }
    find_matches(file, display_name, regexes, pattern_count, flags, filename_needed, is_single_file);
  }

  if (filename && file) {
    close_file(file, filename);
  }
}

// Читаем паттерны из файла
int read_patterns_from_file(const char *filename, char patterns[][MAX_PATTERN_LEN], int *pattern_count) {
  FILE *file = fopen(filename, "r");
  if (!file) {
    return -1;
  }

  char line[MAX_PATTERN_LEN];
  while (fgets(line, sizeof(line), file) != NULL) {
    // Удаляем символ конца строки
    size_t len = strlen(line);
    while (len > 0 && (line[len-1] == '\n' || line[len-1] == '\r')) {
      line[--len] = '\0';
    }

    if (len > 0 && *pattern_count < MAX_PATTERNS) {
      strncpy(patterns[*pattern_count], line, MAX_PATTERN_LEN - 1);
      (*pattern_count)++;
    }
  }

  fclose(file);
  return 0;
}

// Основная функция grep
int grep_main(int argc, char *argv[]) {
  GrepFlags flags;
  char patterns[MAX_PATTERNS][MAX_PATTERN_LEN];
  int pattern_count = 0;

  // Инициализация флагов
  memset(&flags, 0, sizeof(GrepFlags));
  memset(patterns, 0, sizeof(patterns));

  // Сначала обрабатываем все флаги
  int i = 1;
  int has_explicit_pattern = 0;  // Whether -e or -f was used to specify pattern

  while (i < argc) {
    if (strcmp(argv[i], "--") == 0) {
      // Конец флагов
      i++;
      break;
    }

    if (argv[i][0] == '-' && argv[i][1] != '\0') {
      // Обработка флагов
      if (strcmp(argv[i], "-e") == 0) {
        if (i + 1 < argc && argv[i + 1][0] != '-') {
          i++;
          if (pattern_count < MAX_PATTERNS) {
            strncpy(patterns[pattern_count], argv[i], MAX_PATTERN_LEN - 1);
            pattern_count++;
            has_explicit_pattern = 1;
          }
        }
        i++;
        continue;
      } else if (strcmp(argv[i], "-f") == 0) {
        if (i + 1 < argc && argv[i + 1][0] != '-') {
          i++;
          if (read_patterns_from_file(argv[i], patterns, &pattern_count) != 0) {
            print_error(argv[i], "Cannot read file");
            return 1;
          }
          has_explicit_pattern = 1;
        }
        i++;
        continue;
      } else if (strcmp(argv[i], "-h") == 0) {
        flags.flag_h = 1;
        i++;
        continue;
      } else if (strcmp(argv[i], "-i") == 0) {
        flags.flag_i = 1;
        i++;
        continue;
      } else if (strcmp(argv[i], "-v") == 0) {
        flags.flag_v = 1;
        i++;
        continue;
      } else if (strcmp(argv[i], "-c") == 0) {
        flags.flag_c = 1;
        i++;
        continue;
      } else if (strcmp(argv[i], "-l") == 0) {
        flags.flag_l = 1;
        i++;
        continue;
      } else if (strcmp(argv[i], "-n") == 0) {
        flags.flag_n = 1;
        i++;
        continue;
      } else if (strcmp(argv[i], "-s") == 0) {
        flags.flag_s = 1;
        i++;
        continue;
      } else if (strcmp(argv[i], "-o") == 0) {
        flags.flag_o = 1;
        i++;
        continue;
      }

      // Обработка комбинированных флагов (-iv, -nv и т.д.)
      char *p = &argv[i][1];
      while (*p) {
        switch (*p) {
          case 'e':
            // -e не может быть частью комбинированного флага
            print_error("grep", "-e cannot be combined with other flags");
            return 1;
          case 'f':
            // -f не может быть частью комбинированного флага
            print_error("grep", "-f cannot be combined with other flags");
            return 1;
          case 'h':
            flags.flag_h = 1;
            break;
          case 'i':
            flags.flag_i = 1;
            break;
          case 'v':
            flags.flag_v = 1;
            break;
          case 'c':
            flags.flag_c = 1;
            break;
          case 'l':
            flags.flag_l = 1;
            break;
          case 'n':
            flags.flag_n = 1;
            break;
          case 's':
            flags.flag_s = 1;
            break;
          case 'o':
            flags.flag_o = 1;
            break;
        }
        p++;
      }
      i++;
      continue;
    }

    // Это не флаг - останавливаемся
    break;
  }

  // После флагов идут паттерны и файлы
  // Если явно не указано pattern через -e или -f, используем первый аргумент как pattern
  if (i < argc && !has_explicit_pattern) {
    // Добавляем первый аргумент как паттерн
    if (pattern_count < MAX_PATTERNS) {
      strncpy(patterns[pattern_count], argv[i], MAX_PATTERN_LEN - 1);
      pattern_count++;
    }
    i++;
  }

  // Если шаблон не указан, выводим ошибку
  if (pattern_count == 0) {
    print_error("grep", "missing pattern");
    return 1;
  }

  // Компиляция всех регулярных выражений
  regex_t regexes[MAX_PATTERNS];
  int cflags = REG_EXTENDED;
  if (flags.flag_i) {
    cflags |= REG_ICASE;
  }

  for (int j = 0; j < pattern_count; j++) {
    int errcode = regcomp(&regexes[j], patterns[j], cflags);
    if (errcode != 0) {
      char errbuf[256];
      regerror(errcode, &regexes[j], errbuf, sizeof(errbuf));
      fprintf(stderr, "grep: %s\n", errbuf);
      free_resources(regexes, pattern_count);
      return 1;
    }
  }

  // Остальные аргументы - файлы
  int file_found = 0;
  int file_count = 0;
  for (int j = i; j < argc; j++) {
    file_count++;
  }
  for (int j = i; j < argc; j++) {
    process_file(argv[j], regexes, pattern_count, &flags, file_count > 1, file_count);
    file_found = 1;
  }

  // Если файлов не указано, читаем из stdin
  if (!file_found) {
    process_file(NULL, regexes, pattern_count, &flags, 0, 1);
  }

  // Освобождение ресурсов
  free_resources(regexes, pattern_count);

  return 0;
}
