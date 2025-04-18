#ifndef LOG_HANDLER_H
#define LOG_HANDLER_H

#include <stdarg.h>

// Funkcije za logove
void log_init(void);
void log_message(const char* message);
void log_message_format(const char* format, ...);

#endif // LOG_HANDLER_H