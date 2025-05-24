extern "C" {
#include "bsp/device.h"
#include "bsp/power.h"
#include "bsp_lvgl.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "nvs_flash.h"
}
#include "Meshtastic.h"

// Constants
static char const TAG[] = "main";

extern "C" void app_main(void) {
    // Start the GPIO interrupt service
    gpio_install_isr_service(0);

    // Initialize the Non Volatile Storage service
    esp_err_t res = nvs_flash_init();
    if (res == ESP_ERR_NVS_NO_FREE_PAGES || res == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        res = nvs_flash_init();
    }
    ESP_ERROR_CHECK(res);

    // Initialize the Board Support Package
    ESP_ERROR_CHECK(bsp_device_initialize());

    bsp_power_set_radio_state(BSP_POWER_RADIO_STATE_APPLICATION);

    ESP_LOGW(TAG, "Hello Meshtastic!");

    run_meshtastic();
}
