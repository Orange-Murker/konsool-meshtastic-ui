#include "esp_log.h"
#include "esp_log_level.h"
#include "util/ILog.h"

static char const UI_TAG[] = "DeviceUI";

class EspLogger : ILog {
   public:
    EspLogger() : ILog(this) {
    }

    void log_debug(const char* format, ...) override {
        esp_log_level_t level = ESP_LOG_DEBUG;

        esp_log_write(level, UI_TAG, "D %s ", UI_TAG);

        va_list args;
        va_start(args, format);
        esp_log_writev(level, UI_TAG, format, args);
        va_end(args);

        esp_log_write(level, UI_TAG, "\r\n");
    }

    void log_info(const char* format, ...) override {
        esp_log_level_t level = ESP_LOG_INFO;

        esp_log_write(level, UI_TAG, "I %s ", UI_TAG);

        va_list args;
        va_start(args, format);
        esp_log_writev(level, UI_TAG, format, args);
        va_end(args);

        esp_log_write(level, UI_TAG, "\r\n");
    }

    void log_warn(const char* format, ...) override {
        esp_log_level_t level = ESP_LOG_WARN;

        esp_log_write(level, UI_TAG, "W %s ", UI_TAG);

        va_list args;
        va_start(args, format);
        esp_log_writev(level, UI_TAG, format, args);
        va_end(args);

        esp_log_write(level, UI_TAG, "\r\n");
    }

    void log_error(const char* format, ...) override {
        esp_log_level_t level = ESP_LOG_ERROR;

        esp_log_write(level, UI_TAG, "E %s ", UI_TAG);

        va_list args;
        va_start(args, format);
        esp_log_writev(level, UI_TAG, format, args);
        va_end(args);

        esp_log_write(level, UI_TAG, "\r\n");
    }

    void log_crit(const char* format, ...) override {
        esp_log_level_t level = ESP_LOG_ERROR;

        esp_log_write(level, UI_TAG, "E %s ", UI_TAG);

        va_list args;
        va_start(args, format);
        esp_log_writev(level, UI_TAG, format, args);
        va_end(args);

        esp_log_write(level, UI_TAG, "\r\n");
    }

    void log_trace(const char* format, ...) override {
        esp_log_level_t level = ESP_LOG_VERBOSE;

        esp_log_write(level, UI_TAG, "V %s ", UI_TAG);

        va_list args;
        va_start(args, format);
        esp_log_writev(level, UI_TAG, format, args);
        va_end(args);

        esp_log_write(level, UI_TAG, "\r\n");
    };
};
