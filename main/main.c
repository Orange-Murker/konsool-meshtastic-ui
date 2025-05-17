#include <stdio.h>
#include "bsp/device.h"
#include "bsp/display.h"
#include "bsp/input.h"
#include "bsp_lvgl.h"
#include "core/lv_obj.h"
#include "display/lv_display.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "font/lv_font.h"
#include "hal/lcd_types.h"
#include "layouts/flex/lv_flex.h"
#include "nvs_flash.h"
#include "widgets/label/lv_label.h"
#include "widgets/textarea/lv_textarea.h"

// Constants
static char const TAG[] = "main";

// Global variables
static esp_lcd_panel_handle_t       display_lcd_panel    = NULL;
static esp_lcd_panel_io_handle_t    display_lcd_panel_io = NULL;
static size_t                       display_h_res        = 0;
static size_t                       display_v_res        = 0;
static lcd_color_rgb_pixel_format_t display_color_format;
static QueueHandle_t                input_event_queue = NULL;

void app_main(void) {
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

    // Fetch the handle for using the screen, this works even when
    res = bsp_display_get_panel(&display_lcd_panel);
    ESP_ERROR_CHECK(res);                             // Check that the display handle has been initialized
    bsp_display_get_panel_io(&display_lcd_panel_io);  // Do not check result of panel IO handle: not all types of
                                                      // display expose a panel IO handle
    res = bsp_display_get_parameters(&display_h_res, &display_v_res, &display_color_format);
    ESP_ERROR_CHECK(res);  // Check that the display parameters have been initialized

    ESP_ERROR_CHECK(bsp_input_get_queue(&input_event_queue));

    // Initialise LVGL
    lvgl_init(display_h_res, display_v_res, display_color_format, display_lcd_panel, display_lcd_panel_io,
              input_event_queue);

    ESP_LOGW(TAG, "Hello world!");

    lvgl_lock();

    lv_obj_t* screen = lv_screen_active();
    lv_obj_set_flex_flow(screen, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(screen, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

    lv_obj_t* label = lv_label_create(screen);
    lv_obj_set_style_text_font(label, &lv_font_montserrat_42, LV_STATE_DEFAULT);
    lv_label_set_text(label, "Hello World!");

    lv_obj_t* textarea = lv_textarea_create(screen);
    lv_textarea_set_placeholder_text(textarea, "Type Here!");

    lvgl_unlock();

    // You can add your main loop here
}
