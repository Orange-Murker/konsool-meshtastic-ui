#include "Meshtastic.h"
#include "BadgeBspDisplay.h"
#include "EspClient.h"
#include "comms/IClientBase.h"
#include "esp_log.h"
#include "freertos/idf_additions.h"
#include "freertos/projdefs.h"
#include "graphics/DeviceGUI.h"
#include "graphics/driver/DisplayDriver.h"
#include "graphics/driver/DisplayDriverConfig.h"
#include "graphics/view/TFT/TFTView_320x240.h"
#include "meshtastic/device_ui.pb.h"
extern "C" {
#include "bsp_lvgl.h"
}

static char const TAG[] = "MeshtasticMain";

static TFTView_320x240* gui = NULL;
static EspClient        client;
static DisplayDriver*   display;

const char* firmware_version = "Tanmatsu Edition";

static void task_handler(void* param) {
    while (1) {
        lvgl_lock();
        gui->task_handler();
        lvgl_unlock();
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

Meshtastic::Meshtastic() {
    display = &BadgeBspDisplay::create();
    client  = EspClient();
    ESP_LOGI(TAG, "Client %p", &client);

    DisplayDriverConfig cfg = DisplayDriverConfig();
    cfg.device(DisplayDriverConfig::device_t::CUSTOM_TFT);

    gui = TFTView_320x240::instance(cfg, display);

    lvgl_lock();
    gui->init(&client);
    lvgl_unlock();

    const meshtastic_DeviceUIConfig device_ui_config = {
        .version            = 0,
        .screen_brightness  = 255,
        .screen_timeout     = 30,
        .screen_lock        = false,
        .settings_lock      = false,
        .pin_code           = 0,
        .theme              = meshtastic_Theme_DARK,
        .alert_enabled      = false,
        .banner_enabled     = false,
        .ring_tone_id       = 0,
        .language           = meshtastic_Language_ENGLISH,
        .has_node_filter    = false,
        .has_node_highlight = false,
        .has_map_data       = false,
    };

    gui->setupUIConfig(device_ui_config);

    xTaskCreatePinnedToCore(task_handler, "meshtastic-gui", 10240, NULL, 1, NULL, 1);
}
