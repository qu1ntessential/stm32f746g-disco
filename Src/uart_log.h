#ifndef UART_LOG_H
#define UART_LOG_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdarg.h>

#define UART_COLOR_RESET  "\x1B[0m"  // неокрашенный лог
#define UART_COLOR_RED    "\x1B[31m" // красный цвет лога
#define UART_COLOR_BLUE   "\x1B[34m" // синий цвет лога
#define UART_COLOR_GREEN  "\x1B[32m" // зеленый цвет лога
#define UART_COLOR_YELLOW "\x1B[33m" // желтый цвет лога

typedef enum {
    NONE_LOG = 0,
    ERROR_LOG,
    INFO_LOG,
    DEBUG_LOG
} log_level_t;

#define LOG_LEVEL DEBUG_LOG // Установка текущего уровня логирования

inline void print_log(log_level_t level, const char *format, ...) {
    if (level <= LOG_LEVEL) {
        // Выбираем цвет в зависимости от уровня
        const char *color = (level == ERROR_LOG) ? UART_COLOR_RED :
                            (level == INFO_LOG) ? UART_COLOR_YELLOW :
                            (level == DEBUG_LOG) ? UART_COLOR_GREEN :
                            UART_COLOR_RESET;

        // Печатаем цвет
        printf("%s", color);

        // Используем va_list для обработки переменного числа аргументов
        va_list args;
        va_start(args, format);
        vprintf(format, args);  // Выводим форматированную строку
        va_end(args);

        // Сбрасываем цвет
        printf("%s", UART_COLOR_RESET);
        fflush(stdout);  // Обеспечиваем немедленный вывод
    }
}

#ifdef __cplusplus
}
#endif

#endif //UART_LOG_H
