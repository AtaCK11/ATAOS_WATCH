#include "logger/logger.h"
#include <stdarg.h>

// Pre-calculate level prefixes for efficiency
static const char* levelPrefixes[] = {
    "E", // Error
    "W", // Warning
    "I", // Info
    "D", // Debug
    "V"  // Verbose
};

void logger::log(int level, const char* tag, const char* file, int line, const char* func, const char* format, ...) {
    if (level <= LOG_LEVEL) {
        TickType_t tickCount = xTaskGetTickCount();
        unsigned long timestamp = tickCount * portTICK_PERIOD_MS;

        const char* levelPrefix = (level >= 0 && level < sizeof(levelPrefixes) / sizeof(levelPrefixes[0])) ? levelPrefixes[level] : levelPrefixes[0];

        va_list args;
        va_start(args, format);

        char buffer[256];
        int len = snprintf(buffer, sizeof(buffer), "[%5lu][%1s][%s:%d] %s(): [%s] ", timestamp, levelPrefix, file, line, func, tag);

        if (len < sizeof(buffer) && len > 0) {
            vsnprintf(buffer + len, sizeof(buffer) - len, format, args);
            Serial.print(buffer);
            Serial.println();
        } else {
            // Truncated (or error) - output the buffer as-is.
            buffer[sizeof(buffer)-1] = '\0';
            Serial.print(buffer);
            Serial.println("...");
        }

        va_end(args);
    }
}
