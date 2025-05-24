#pragma once

#include "esp_lcd_types.h"
#include "freertos/FreeRTOS.h"
#include "graphics/driver/DisplayDriver.h"

constexpr uint32_t defaultScreenTimeout = 30 * 1000;
constexpr uint32_t defaultBrightness    = 255;

class BadgeBspDisplay : public DisplayDriver {
   public:
    static BadgeBspDisplay& create(QueueHandle_t input_event_queue);
    void                    init(DeviceGUI* gui) override;
    bool                    hasButton(void) override {
        return true;
    };
    bool hasLight(void) override {
        return true;
    }
    void task_handler(void) override;
    bool isPowersaving(void) override {
        return powersaving;
    }

    uint8_t getBrightness() override;
    void    setBrightness(uint8_t brightness) override;

    uint16_t getScreenTimeout() override {
        return screen_timeout_ms / 1000;
    }
    void setScreenTimeout(uint16_t timeout) override {
        screen_timeout_ms = timeout * 1000;
    };

   private:
    BadgeBspDisplay(uint16_t width, uint16_t height);
    static BadgeBspDisplay* bsp_display;
    uint32_t                screen_timeout_ms;
    uint32_t                last_brightness;
    bool                    powersaving;
};
