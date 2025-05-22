#include "EspClient.h"
#include "comms/IClientBase.h"
#include "esp_log.h"
#include "freertos/idf_additions.h"
#include "freertos/projdefs.h"
#include "graphics/DeviceGUI.h"
#include "graphics/driver/DisplayDriver.h"
#include "graphics/driver/DisplayDriverConfig.h"
#include "graphics/view/TFT/TFTView_320x240.h"
#include "Meshtastic.h"
#include "BadgeBspDisplay.h"

static char const TAG[] = "MeshtasticMain";

static DeviceGUI* gui = NULL;
static EspClient client;

const char *firmware_version = "Tanmatsu Edition";

static void task_handler(void* param) {
    gui->task_handler();
    vTaskDelay(pdMS_TO_TICKS(100));
}

Meshtastic::Meshtastic() {
    DisplayDriver display =  BadgeBspDisplay::create();
    client =  EspClient();
    ESP_LOGI(TAG, "Client %p", &client);

    DisplayDriverConfig cfg = DisplayDriverConfig();
    cfg.device(DisplayDriverConfig::device_t::CUSTOM_TFT);

    gui = TFTView_320x240::instance(cfg, &display);

    gui->init(&client);

    // xTaskCreatePinnedToCore(task_handler, "meshtastic-gui", 10240, NULL, 1, NULL, 1);
}
