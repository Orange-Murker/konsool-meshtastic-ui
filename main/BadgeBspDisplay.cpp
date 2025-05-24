#include "BadgeBspDisplay.h"
#include "freertos/idf_additions.h"
extern "C" {
#include "bsp/display.h"
#include "bsp_lvgl.h"
}
#include "display/lv_display.h"
#include "esp_err.h"
#include "graphics/DeviceGUI.h"
#include "util/ILog.h"

static esp_lcd_panel_handle_t       display_lcd_panel;
static esp_lcd_panel_io_handle_t    display_lcd_panel_io;
static size_t                       display_h_res;
static size_t                       display_v_res;
static lcd_color_rgb_pixel_format_t display_color_format;

BadgeBspDisplay* BadgeBspDisplay::bsp_display = NULL;

BadgeBspDisplay::BadgeBspDisplay(uint16_t width, uint16_t height)
    : DisplayDriver(width, height),
      screen_timeout_ms(defaultScreenTimeout),
      last_brightness(defaultBrightness),
      powersaving(false) {
    setBrightness(defaultBrightness);
}

BadgeBspDisplay& BadgeBspDisplay::create(QueueHandle_t input_event_queue) {
    if (!bsp_display) {
        esp_err_t res;
        // Fetch the handle for using the screen, this works even when
        res = bsp_display_get_panel(&display_lcd_panel);
        ESP_ERROR_CHECK(res);                             // Check that the display handle has been initialized
        bsp_display_get_panel_io(&display_lcd_panel_io);  // Do not check result of panel IO handle: not all types of
                                                          // display expose a panel IO handle
        res = bsp_display_get_parameters(&display_h_res, &display_v_res, &display_color_format);
        ESP_ERROR_CHECK(res);  // Check that the display parameters have been initialized

        // Initialise LVGL
        lvgl_init(display_h_res, display_v_res, display_color_format, display_lcd_panel, display_lcd_panel_io,
                  input_event_queue);

        bsp_display = new BadgeBspDisplay(display_h_res, display_v_res);
    }
    return *bsp_display;
}

void BadgeBspDisplay::init(DeviceGUI* gui) {
    ILOG_DEBUG("BadgeBspDriver::init...");
    // Initialize LVGL
    DisplayDriver::init(gui);

    screenWidth  = display_h_res;
    screenHeight = display_v_res;
    display      = lv_display_get_default();
}

void BadgeBspDisplay::task_handler(void) {
    if ((screen_timeout_ms > 0 && lv_display_get_inactive_time(NULL) > screen_timeout_ms) || powersaving ||
        (DisplayDriver::view->isScreenLocked())) {
        if (!powersaving) {
            // dim display brightness slowly down
            uint8_t brightness;
            bsp_display_get_backlight_brightness(&brightness);
            if (brightness > 1) {
                bsp_display_set_backlight_brightness(brightness - 1);
            } else {
                ILOG_INFO("enter powersave");
                DisplayDriver::view->screenSaving(true);
                powersaving = true;
            }
        }
    }
    if (powersaving) {
        int pin_int = -1;
        if ((pin_int >= 0 && DisplayDriver::view->sleep(pin_int)) ||
            (screen_timeout_ms + 50 > lv_display_get_inactive_time(NULL) && !DisplayDriver::view->isScreenLocked())) {
            // delay(2); // let the CPU finish to restore all register in case of light sleep
            // // woke up by touch or button
            ILOG_INFO("leaving powersave");
            powersaving = false;
            DisplayDriver::view->triggerHeartbeat();
            setBrightness(last_brightness);
            DisplayDriver::view->screenSaving(false);
            lv_display_trigger_activity(NULL);
        } else {
            // we woke up due to e.g. serial traffic (or sleep() simply not implemented)
            // continue with processing loop and enter sleep() again next round
        }
    }
};

uint8_t BadgeBspDisplay::getBrightness() {
    uint8_t percentage;
    bsp_display_get_backlight_brightness(&percentage);
    uint8_t value = (((uint16_t)percentage * 255) / 100);
    return value;
}

void BadgeBspDisplay::setBrightness(uint8_t brightness) {
    uint8_t percentage = ((uint16_t)brightness * 100) / 255;
    bsp_display_set_backlight_brightness(percentage);
}
