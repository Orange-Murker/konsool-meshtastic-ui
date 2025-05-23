#include "BadgeBspDisplay.h"
extern "C" {
#include "bsp/display.h"
#include "bsp/input.h"
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
static QueueHandle_t                input_event_queue;

BadgeBspDisplay* BadgeBspDisplay::bsp_display = NULL;

BadgeBspDisplay::BadgeBspDisplay(uint16_t width, uint16_t height) : DisplayDriver(width, height) {
}

BadgeBspDisplay& BadgeBspDisplay::create() {
    if (!bsp_display) {
        esp_err_t res;
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

void BadgeBspDisplay::task_handler(void) {};
