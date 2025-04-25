#ifndef LOG_H
#define LOG_H

#include <stdio.h>

/* Proste makra logowania */
#define log_info(...)  do { \
    printf("[INFO] "); \
    printf(__VA_ARGS__); \
} while (0)

#define log_error(...) do { \
    printf("[ERROR] "); \
    printf(__VA_ARGS__); \
} while (0)

#define log_warn(...)  do { \
    printf("[WARN] "); \
    printf(__VA_ARGS__); \
} while (0)

#endif // LOG_H
