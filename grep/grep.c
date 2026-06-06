#include "grep.h"

#define MAX_PATTERNS 100
#define MAX_PATTERN_LEN 1024

// Основная функция grep
int grep_main(int argc, char *argv[]) {
  GrepFlags flags;
  char patterns[MAX_PATTERNS][MAX_PATTERN_LEN];
  int pattern_count = 0;
  
  // Инициализация флагов
  memset(&flags, 0, sizeof(GrepFlags));
  memset(patterns, 0, sizeof(patterns));

  // Парсинг флагов и паттернов
  parse_flags(argc, argv, &flags, patterns, &pattern_count);

  // Если шаблон не указан, выводим ошибку
  if (pattern_count == 0) {
    fprintf(stderr, "grep: missing pattern\n");
    return 1;
  }

  // Компиляция всех регулярных выражений
  regex_t regexes[MAX_PATTERNS];
  int cflags = REG_EXTENDED;
  if (flags.flag_i) {
    cflags |= REG_ICASE;
  }

  for (int i = 0; i < pattern_count; i++) {
    int errcode = regcomp(&regexes[i], patterns[i], cflags);
    if (errcode != 0) {
      char errbuf[256];
      regerror(errcode, &regexes[i], errbuf, sizeof(errbuf));
      fprintf(stderr, "grep: %s\n", errbuf);
      free_resources(regexes, pattern_count);
      return 1;
    }
  }

  // Определение файлов (всё, что не флаг и не паттерн)
  int file_found = 0;
  int skipped_patterns = 0;  // Количество пропущенных паттернов
  for (int i = 1; i < argc; i++) {
    // Пропускаем флаги и их значения
    if (argv[i][0] == '-' && argv[i][1] != '\0') {
      if (strcmp(argv[i], "-e") == 0 || strcmp(argv[i], "-f") == 0) {
        i++;  // Пропускаем значение
        continue;
      }
      // Для комбинированных флагов с e или f
      char *p = &argv[i][1];
      while (*p) {
        if (*p == 'e' || *p == 'f') {
          i++;  // Пропускаем значение
          break;
        }
        p++;
      }
      continue;
    }
    
    // Пропускаем первые нефлаг аргументы как паттерны
    if (skipped_patterns < pattern_count) {
      skipped_patterns++;
      continue;
    }
    
    // Это файл
    process_file(argv[i], regexes, pattern_count, &flags);
    file_found = 1;
  }

  // Если файлов не указано, читаем из stdin
  if (!file_found) {
    process_file(NULL, regexes, pattern_count, &flags);
  }

  // Освобождение ресурсов
  free_resources(regexes, pattern_count);

  return 0;
}

// Парсинг флагов и паттернов
int parse_flags(int argc, char *argv[], GrepFlags *flags, char patterns[][MAX_PATTERN_LEN], int *pattern_count) {
  int pattern_from_flag = 0;  // Был ли паттерн через -e
  
  // Сначала собираем все флаги
  for (int i = 1; i < argc; i++) {
    // Проверяем, является ли это флагом
    if (argv[i][0] == '-' && argv[i][1] != '\0') {
      // Обработка комбинированных флагов
      char *p = &argv[i][1];
      while (*p) {
        switch (*p) {
          case 'e':
            flags->flag_e = 1;
            break;
          case 'i':
            flags->flag_i = 1;
            break;
          case 'v':
            flags->flag_v = 1;
            break;
          case 'c':
            flags->flag_c = 1;
            break;
          case 'l':
            flags->flag_l = 1;
            break;
          case 'n':
            flags->flag_n = 1;
            break;
          case 'h':
            flags->flag_h = 1;
            break;
          case 's':
            flags->flag_s = 1;
            break;
          case 'f':
            flags->flag_f = 1;
            break;
          case 'o':
            flags->flag_o = 1;
            break;
        }
        p++;
      }
    }
  }

  // Теперь собираем паттерны
  for (int i = 1; i < argc; i++) {
    // Пропускаем флаги и их значения
    if (argv[i][0] == '-' && argv[i][1] != '\0') {
      if (strcmp(argv[i], "-e") == 0) {
        if (i + 1 < argc && *pattern_count < MAX_PATTERNS) {
          i++;
          strncpy(patterns[*pattern_count], argv[i], MAX_PATTERN_LEN - 1);
          (*pattern_count)++;
          pattern_from_flag = 1;
        }
      } else if (strcmp(argv[i], "-f") == 0) {
        if (i + 1 < argc) {
          i++;
          FILE *f = fopen(argv[i], "r");
          if (f) {
            char line[MAX_PATTERN_LEN];
            while (fgets(line, sizeof(line), f) && *pattern_count < MAX_PATTERNS) {
              size_t len = strlen(line);
              if (len > 0 && line[len - 1] == '\n') {
                line[len - 1] = '\0';
              }
              if (strlen(line) > 0) {
                strncpy(patterns[*pattern_count], line, MAX_PATTERN_LEN - 1);
                (*pattern_count)++;
              }
            }
            fclose(f);
          } else if (!flags->flag_s) {
            fprintf(stderr, "grep: %s: No such file or directory\n", argv[i]);
          }
        }
      }
      continue;
    }
    
    // Не флаг - это либо паттерн (если не задан через -e), либо файл
    if (!pattern_from_flag) {
      // Паттерн как аргумент
      if (*pattern_count < MAX_PATTERNS) {
        strncpy(patterns[*pattern_count], argv[i], MAX_PATTERN_LEN - 1);
        (*pattern_count)++;
      }
    }
    // Если был паттерн через -e, то это файл
  }
  
  return 0;
}

// Обработка одного файла
void process_file(const char *filename, regex_t *regexes, int pattern_count, GrepFlags *flags) {
  FILE *file = stdin;
  char display_name[4096] = {0};

  if (filename) {
    file = fopen(filename, "r");
    if (file == NULL) {
      if (!flags->flag_s) {
        fprintf(stderr, "grep: %s: No such file or directory\n", filename);
      }
      return;
    }
    strcpy(display_name, filename);
  } else {
    strcpy(display_name, "");
  }

  // Если нужно вывести только имена файлов
  if (flags->flag_l) {
    find_matches(file, display_name, regexes, pattern_count, flags);
  } else {
    // Сброс позиции файла
    if (filename) {
      rewind(file);
    }
    find_matches(file, display_name, regexes, pattern_count, flags);
  }

  if (filename) {
    fclose(file);
  }
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

// Поиск и вывод совпадений
void find_matches(FILE *file, const char *filename, regex_t *regexes, int pattern_count, GrepFlags *flags) {
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
        // Выводим имя файла только один раз
        printf("%s\n", filename);
        break;
      } else if (flags->flag_o) {
        // Выводим только совпадения
        char *start = line;
        regmatch_t pmatch[1];
        int matched = 1;

        while (matched) {
          matched = 0;
          int min_start = -1;
          int min_match_len = 0;

          // Ищем первое совпадение среди всех паттернов
          for (int p = 0; p < pattern_count; p++) {
            if (regexec(&regexes[p], start, 1, pmatch, 0) == 0) {
              int match_len = pmatch[0].rm_eo - pmatch[0].rm_so;
              if (match_len > 0) {
                if (min_start < 0 || pmatch[0].rm_so < min_start) {
                  min_start = pmatch[0].rm_so;
                  min_match_len = match_len;
                  matched = 1;
                } else if (pmatch[0].rm_so == min_start && match_len > min_match_len) {
                  min_match_len = match_len;
                }
              }
            }
          }

          if (matched) {
            // Выводим имя файла, если не -h и есть имя файла
            if (!flags->flag_h && strlen(filename) > 0) {
              printf("%s:", filename);
            }
            // Выводим номер строки, если -n
            if (flags->flag_n) {
              printf("%d:", line_number);
            }
            // Выводим само совпадение
            fwrite(start + min_start, 1, min_match_len, stdout);
            printf("\n");
            // Переходим к следующему символу после совпадения
            start += min_start + min_match_len;
            if (*start == '\0') break;
          }
        }
      } else {
        // Выводим всю строку
        // Выводим имя файла, если не -h и есть имя файла
        if (!flags->flag_h && strlen(filename) > 0) {
          printf("%s:", filename);
        }
        // Выводим номер строки, если -n
        if (flags->flag_n) {
          printf("%d:", line_number);
        }
        printf("%s", line);
      }
    }
  }

  // Выводим количество совпадений, если -c
  if (flags->flag_c && match_count >= 0) {
    if (!flags->flag_h && strlen(filename) > 0) {
      printf("%s:", filename);
    }
    printf("%d\n", match_count);
  }

  free(line);
}

// Освобождение ресурсов
void free_resources(regex_t *regexes, int pattern_count) {
  for (int i = 0; i < pattern_count; i++) {
    regfree(&regexes[i]);
  }
}
