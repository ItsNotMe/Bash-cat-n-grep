#include "cat.h"

#include "common.h"

// Обработчики флагов

// Обработка флага -b (--number-nonblank)
int handle_b_flag(int argc, char *argv[], int *i, CatFlags *flags) {
    (void)argc;
    (void)argv;
    (void)i;
    flags->flag_b = 1;
    return 0;
}

// Обработка флага -e
int handle_e_flag(int argc, char *argv[], int *i, CatFlags *flags) {
    (void)argc;
    (void)argv;
    (void)i;
    flags->flag_e = 1;
    flags->flag_v = 1;
    return 0;
}

// Обработка флага -E
int handle_E_flag(int argc, char *argv[], int *i, CatFlags *flags) {
    (void)argc;
    (void)argv;
    (void)i;
    flags->flag_E = 1;
    return 0;
}

// Обработка флага -n (--number)
int handle_n_flag(int argc, char *argv[], int *i, CatFlags *flags) {
    (void)argc;
    (void)argv;
    (void)i;
    flags->flag_n = 1;
    return 0;
}

// Обработка флага -s (--squeeze-blank)
int handle_s_flag(int argc, char *argv[], int *i, CatFlags *flags) {
    (void)argc;
    (void)argv;
    (void)i;
    flags->flag_s = 1;
    return 0;
}

// Обработка флага -t
int handle_t_flag(int argc, char *argv[], int *i, CatFlags *flags) {
    (void)argc;
    (void)argv;
    (void)i;
    flags->flag_t = 1;
    flags->flag_v = 1;
    return 0;
}

// Обработка флага -T
int handle_T_flag(int argc, char *argv[], int *i, CatFlags *flags) {
    (void)argc;
    (void)argv;
    (void)i;
    flags->flag_T = 1;
    return 0;
}

// Обработка флага -v
int handle_v_flag(int argc, char *argv[], int *i, CatFlags *flags) {
    (void)argc;
    (void)argv;
    (void)i;
    flags->flag_v = 1;
    return 0;
}

// Обработка длинных флагов
int handle_long_flag(int argc, char *argv[], int *i, CatFlags *flags) {
    (void)argc;
    (void)argv;
    (void)i;

    if (strcmp(argv[*i], "--number-nonblank") == 0) {
        flags->flag_b = 1;
    } else if (strcmp(argv[*i], "--number") == 0) {
        flags->flag_n = 1;
    } else if (strcmp(argv[*i], "--squeeze-blank") == 0) {
        flags->flag_s = 1;
    }
    return 0;
}

// Обработка флага --
int handle_end_flags(int argc, char *argv[], int *i, CatFlags *flags) {
    (void)argc;
    (void)argv;
    (void)flags;

    // Конец флагов
    (*i)++;
    return 1; // signal to break
}

// Таблица определений флагов
static CatFlagDef cat_flag_defs[] = {
    {"-b", NULL, handle_b_flag},
    {"-e", NULL, handle_e_flag},
    {"-E", NULL, handle_E_flag},
    {"-n", NULL, handle_n_flag},
    {"-s", NULL, handle_s_flag},
    {"-t", NULL, handle_t_flag},
    {"-T", NULL, handle_T_flag},
    {"-v", NULL, handle_v_flag},
    {"--", NULL, handle_end_flags},
};

// Обработка одного флага по таблице
static int process_flag(int argc, char *argv[], char *arg, int *i, CatFlags *flags) {
    for (size_t j = 0; j < sizeof(cat_flag_defs) / sizeof(cat_flag_defs[0]); j++) {
        if (strcmp(arg, cat_flag_defs[j].short_flag) == 0) {
            return cat_flag_defs[j].handler(argc, argv, i, flags);
        }
    }
    return 0;
}

// Обработка комбинированных флагов
static int process_combined_flags(char *arg, CatFlags *flags) {
    char *p = &arg[1];
    while (*p) {
        switch (*p) {
            case 'b':
                flags->flag_b = 1;
                break;
            case 'e':
                flags->flag_e = 1;
                flags->flag_v = 1;
                break;
            case 'E':
                flags->flag_E = 1;
                break;
            case 'n':
                flags->flag_n = 1;
                break;
            case 's':
                flags->flag_s = 1;
                break;
            case 't':
                flags->flag_t = 1;
                flags->flag_v = 1;
                break;
            case 'T':
                flags->flag_T = 1;
                break;
            case 'v':
                flags->flag_v = 1;
                break;
        }
        p++;
    }
    return 0;
}

// Парсинг флагов командной строки
void parse_flags(int argc, char *argv[], CatFlags *flags) {
    // Инициализация флагов нулями
    memset(flags, 0, sizeof(CatFlags));

    for (int i = 1; i < argc; i++) {
        // Проверка на длинные флаги
        if (argv[i][0] == '-' && argv[i][1] == '-') {
            if (process_flag(argc, argv, argv[i], &i, flags) != 0) {
                break;
            }
            continue;
        }

        // Проверка на одиночный флаг (например, -e)
        if (argv[i][0] == '-' && argv[i][1] != '\0' && strlen(argv[i]) == 2) {
            if (process_flag(argc, argv, argv[i], &i, flags) != 0) {
                break;
            }
            continue;
        }

        // Проверка на комбинированный флаг (например, -ev)
        if (argv[i][0] == '-' && argv[i][1] != '\0' && strlen(argv[i]) > 2) {
            process_combined_flags(argv[i], flags);
            continue;
        }

        // Если это не флаг, останавливаемся
        break;
    }
}

// Обработка одного файла
void process_file(const char *filename, const CatFlags *flags,
                  CatState *state) {
    FILE *file = open_file(filename);
    if (file == NULL) {
        print_error(filename, "No such file or directory");
        return;
    }

    char buffer[4096];
    while (fgets(buffer, sizeof(buffer), file) != NULL) {
        print_line(buffer, flags, state);
    }

    fclose(file);
}

// Обработка stdin
void process_input(const CatFlags *flags, CatState *state) {
    char buffer[4096];
    while (fgets(buffer, sizeof(buffer), stdin) != NULL) {
        print_line(buffer, flags, state);
    }
}

// Вывод строки с применением флагов
void print_line(const char *line, const CatFlags *flags, CatState *state) {
    int len = strlen(line);
    int is_blank = (len == 1 && line[0] == '\n');

    // Сжатие пустых строк (-s)
    if (flags->flag_s && is_blank) {
        if (state->prev_was_blank) {
            state->output_was_squeezed = 1;
            return;
        }
        state->prev_was_blank = 1;
    } else {
        state->prev_was_blank = 0;
        state->output_was_squeezed = 0;
    }

    // Нумерация строк (-n или -b)
    if (flags->flag_n || (flags->flag_b && !is_blank)) {
        state->line_number++;
        printf("%6d\t", state->line_number);
    }

    // Вывод содержимого строки с обработкой специальных символов
    for (int i = 0; i < len; i++) {
        unsigned char c = (unsigned char)line[i];

        // Обработка конца строки ($)
        if (c == '\n') {
            if (flags->flag_e || flags->flag_E) {
                printf("$");
            }
            printf("\n");
            continue;
        }

        // Обработка возврата каретки (^M)
        if (c == '\r') {
            if (flags->flag_v) {
                printf("^");
                printf("M");
            } else {
                printf("\r");
            }
            continue;
        }

        // Обработка табуляции (^I)
        if (c == '\t') {
            if (flags->flag_t || flags->flag_T || flags->flag_v) {
                printf("^");
                printf("I");
            } else {
                printf("\t");
            }
            continue;
        }

        // Обработка других непечатаемых символов (^X)
        if (flags->flag_v && (c < 32 || c == 127)) {
            if (c == 127) {
                printf("^");
                printf("?");
            } else {
                printf("^");
                printf("%c", c + 64);
            }
        } else {
            printf("%c", c);
        }
    }
}

// Основная функция cat
int cat_main(int argc, char *argv[]) {
    CatFlags flags;
    CatState state = {0, 0, 0};

    parse_flags(argc, argv, &flags);

    // Поиск первого аргумента, который не является флагом
    int file_found = 0;
    for (int i = 1; i < argc; i++) {
        // Проверяем, является ли аргумент флагом или stdin
        int is_flag = 0;
        if (argv[i][0] == '-') {
            // Проверяем, это просто "-", который означает stdin
            if (strlen(argv[i]) == 1) {
                // "-" означает stdin
                process_input(&flags, &state);
                file_found = 1;
                is_flag = 1; // Продолжаем, но не обрабатываем как файл
            } else {
                // Если это комбинированный флаг или длинный флаг (--flag), он не может
                // быть файлом
                if (strlen(argv[i]) > 1 && argv[i][1] != '-') {
                    is_flag = 1;
                } else if (argv[i][1] == '-') {
                    is_flag = 1; // Длинный флаг (--flag)
                }
            }
        }

        if (!is_flag && strcmp(argv[i], "--") != 0) {
            process_file(argv[i], &flags, &state);
            file_found = 1;
        }
    }

    // Если файлов не указано, читаем из stdin
    if (!file_found) {
        process_input(&flags, &state);
    }

    return 0;
}

// Точка входа в программу
int main(int argc, char *argv[]) {
    return cat_main(argc, argv);
}
