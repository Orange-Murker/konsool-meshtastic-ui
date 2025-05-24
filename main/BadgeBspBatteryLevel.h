#include <stdint.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "graphics/common/BatteryLevel.h"
#include "graphics/view/TFT/TFTView_320x240.h"

class BadgeBspBatteryLevel {
   public:
    BadgeBspBatteryLevel(TFTView_320x240* g);
    ~BadgeBspBatteryLevel(void);

   private:
    struct {
        uint32_t percentage;
        float    voltage;
    } levels[7];
    static void battery_task(void* param);
    TaskHandle_t task;
    static TFTView_320x240* gui;
};
