#include "Meshtastic.h"
#include <LittleFS.h>
#include "BadgeBspBatteryLevel.h"
#include "BadgeBspDisplay.h"
#include "EspClient.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "graphics/DeviceGUI.h"
#include "graphics/driver/DisplayDriver.h"
#include "graphics/driver/DisplayDriverConfig.h"
#include "graphics/view/TFT/TFTView_320x240.h"
#include "input/InputDriver.h"
#include "misc/lv_types.h"
extern "C" {
#include "bsp/input.h"
#include "bsp_lvgl.h"
}

static char const TAG[] = "MeshtasticMain";

static TFTView_320x240*      gui     = NULL;
static EspClient*            client  = NULL;
static DisplayDriver*        display = NULL;
static BadgeBspBatteryLevel* bat     = NULL;

static TaskHandle_t  task                   = NULL;

const char* firmware_version = "Tanmatsu Edition";

static void meshtastic_task_handler(void* param) {
    while (1) {
        lvgl_lock();
        gui->task_handler();
        lvgl_unlock();
        vTaskDelay(pdMS_TO_TICKS(50));
    }
}

void run_meshtastic() {
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

    InputDriver* input_driver = gui->getInputDriver();
    input_driver->setKeyboard(lvgl_get_indev());

    lvgl_lock();
    gui->init(client);
    lvgl_unlock();

    bat = new BadgeBspBatteryLevel(gui);

    xTaskCreatePinnedToCore(meshtastic_task_handler, "meshtastic-gui", 10240, NULL, 1, &task, 1);
}
