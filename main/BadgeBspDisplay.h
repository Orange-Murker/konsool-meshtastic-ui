#pragma once

#include "esp_lcd_types.h"
#include "freertos/FreeRTOS.h"
#include "graphics/driver/DisplayDriver.h"

class BadgeBspDisplay : public DisplayDriver {
   public:
    static BadgeBspDisplay& create();
    void                    init(DeviceGUI* gui) override;
    virtual ~BadgeBspDisplay() {
    }

   private:
    BadgeBspDisplay(uint16_t width, uint16_t height);
    static BadgeBspDisplay* bsp_display;
};
