#include <stdio.h>
extern "C" {
#include "bsp/device.h"
#include "bsp/power.h"
#include "bsp_lvgl.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "hal/lcd_types.h"
#include "nvs_flash.h"
}
#include "Meshtastic.h"

// Constants
static char const TAG[] = "main";

// Global variables
static esp_lcd_panel_handle_t       display_lcd_panel    = NULL;
static esp_lcd_panel_io_handle_t    display_lcd_panel_io = NULL;
static size_t                       display_h_res        = 0;
static size_t                       display_v_res        = 0;
static lcd_color_rgb_pixel_format_t display_color_format;
static QueueHandle_t                input_event_queue = NULL;

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

    Meshtastic();
}
