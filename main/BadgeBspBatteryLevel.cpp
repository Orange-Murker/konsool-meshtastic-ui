#include "BadgeBspBatteryLevel.h"
extern "C" {
#include "bsp/power.h"
}

TFTView_320x240* BadgeBspBatteryLevel::gui = NULL;

void BadgeBspBatteryLevel::battery_task(void* param) {
    while (1) {
        bsp_power_battery_information_t information;
        bsp_power_get_battery_information(&information);

        uint32_t percentage = (uint32_t)information.remaining_percentage;

        BatteryLevel::Status status;

        if (information.power_supply_available && !information.battery_charging) {
            status = BatteryLevel::Status::Plugged;
        }

        else if (information.battery_charging) {
            status = BatteryLevel::Status::Charging;
        } else if (percentage >= 80) {
            status = BatteryLevel::Status::Full;
        } else if (percentage >= 35) {
            status = BatteryLevel::Status::Mid;
        } else if (percentage >= 10) {
            status = BatteryLevel::Status::Low;
        } else if (percentage > 5) {
            status = BatteryLevel::Status::Empty;
        } else {
            status = BatteryLevel::Status::Warn;
        }

        BadgeBspBatteryLevel::gui->updateBatteryLevel(status, percentage);

        vTaskDelay(pdMS_TO_TICKS(5000));
    }
}

BadgeBspBatteryLevel::BadgeBspBatteryLevel(TFTView_320x240* g) {
    BadgeBspBatteryLevel::gui = g;
    xTaskCreatePinnedToCore(battery_task, "battery-task", 4096, NULL, 1, &task, 0);
}
