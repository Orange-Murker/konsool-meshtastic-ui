#include "Meshtastic.h"
#include <LittleFS.h>
#include "freertos/task.h"
#include "esp_log.h"
#include "EspClient.h"
#include "BadgeBspDisplay.h"
#include "graphics/DeviceGUI.h"
#include "graphics/driver/DisplayDriver.h"
#include "graphics/driver/DisplayDriverConfig.h"
#include "graphics/view/TFT/TFTView_320x240.h"
extern "C" {
#include "bsp_lvgl.h"
}

static char const TAG[] = "MeshtasticMain";

static TFTView_320x240* gui = NULL;
static EspClient*       client;
static DisplayDriver*   display;

const char* firmware_version = "Tanmatsu Edition";

static void task_handler(void* param) {
    while (1) {
        lvgl_lock();
        gui->task_handler();
        lvgl_unlock();
        vTaskDelay(pdMS_TO_TICKS(50));
    }
}

Meshtastic::Meshtastic() {
    if (!LittleFS.begin(true)) {
        ESP_LOGE(TAG, "LittleFS Mount Failed");
        return;
    }

    display = &BadgeBspDisplay::create();
    client  = new EspClient;
    ESP_LOGI(TAG, "Client %p", &client);

    DisplayDriverConfig cfg = DisplayDriverConfig();
    cfg.device(DisplayDriverConfig::device_t::CUSTOM_TFT);

    gui = TFTView_320x240::instance(cfg, display);

    lvgl_lock();
    gui->init(client);
    lvgl_unlock();

    xTaskCreatePinnedToCore(task_handler, "meshtastic-gui", 10240, NULL, 1, NULL, 1);
}
