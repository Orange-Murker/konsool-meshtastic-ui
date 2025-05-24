#pragma once

#include "esp_lcd_types.h"
#include "freertos/FreeRTOS.h"
#include "graphics/driver/DisplayDriver.h"

class BadgeBspDisplay : public DisplayDriver {
   public:
    static BadgeBspDisplay& create(QueueHandle_t input_event_queue);
    void                    init(DeviceGUI* gui) override;
    bool                    hasButton(void) override {
        return true;
    };
    void task_handler(void) override;

   private:
    BadgeBspDisplay(uint16_t width, uint16_t height);
    static BadgeBspDisplay* bsp_display;
};
