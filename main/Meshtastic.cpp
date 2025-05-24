#include "Meshtastic.h"
#include <LittleFS.h>
#include <cstddef>
#include "BadgeBspBatteryLevel.h"
#include "BadgeBspDisplay.h"
#include "EspClient.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/idf_additions.h"
#include "freertos/task.h"
#include "graphics/DeviceGUI.h"
#include "graphics/driver/DisplayDriver.h"
#include "graphics/driver/DisplayDriverConfig.h"
#include "graphics/view/TFT/TFTView_320x240.h"
#include "indev/lv_indev.h"
#include "input/InputDriver.h"
#include "misc/lv_types.h"
#include "portmacro.h"
extern "C" {
#include "bsp/input.h"
#include "bsp_lvgl.h"
}

static char const TAG[] = "MeshtasticMain";

static TFTView_320x240*      gui     = NULL;
static EspClient*            client  = NULL;
static DisplayDriver*        display = NULL;
static BadgeBspBatteryLevel* bat     = NULL;

static TaskHandle_t meshtastic_task_handle = NULL;
static TaskHandle_t input_task_handle      = NULL;

static QueueHandle_t input_event_queue      = NULL;
static QueueHandle_t lvgl_input_event_queue = NULL;
static QueueHandle_t button_indev_queue     = NULL;

static lv_indev_t* button_indev = NULL;

const char* firmware_version = "Tanmatsu Edition";

// Point values for the buttons on the left size of the screen for a 480 pixel tall display
const lv_point_t points_array[] = {{30, 20}, {30, 97}, {30, 174}, {30, 251}, {30, 328}, {30, 405}};

static void meshtastic_task_handler(void* param) {
    while (1) {
        lvgl_lock();
        gui->task_handler();
        lvgl_unlock();
        vTaskDelay(pdMS_TO_TICKS(50));
    }
}

void button_read_cb(lv_indev_t* indev, lv_indev_data_t* data) {
    int button_id = -1;

    UBaseType_t messages_waiting = uxQueueMessagesWaiting(button_indev_queue);

    if (messages_waiting > 1) {
        data->continue_reading = true;
    }

    if (messages_waiting >= 1) {
        if (xQueueReceive(button_indev_queue, &button_id, 0) == pdTRUE) {
        }
    }

    static uint32_t last_btn = 0;
    if (button_id >= 0) {
        last_btn    = button_id;
        data->state = LV_INDEV_STATE_PRESSED;
    } else {
        data->state = LV_INDEV_STATE_RELEASED;
    }

    data->btn_id = last_btn;
}

// Use hardware function buttons to emulate screen presses
static void input_task(void* param) {
    button_indev_queue     = xQueueCreate(32, sizeof(int));

    button_indev = lv_indev_create();
    lv_indev_set_type(button_indev, LV_INDEV_TYPE_BUTTON);
    lv_indev_set_button_points(button_indev, points_array);
    lv_indev_set_read_cb(button_indev, button_read_cb);

    while (1) {
        bsp_input_event_t event;
        if (xQueueReceive(input_event_queue, &event, portMAX_DELAY) == pdTRUE) {
            if (event.type == INPUT_EVENT_TYPE_NAVIGATION && event.args_navigation.key >= BSP_INPUT_NAVIGATION_KEY_F1 &&
                event.args_navigation.key <= BSP_INPUT_NAVIGATION_KEY_F6) {
                int button_num = -1;
                // Only set button numbers for pressed buttons
                if (event.args_navigation.state) {
                    switch (event.args_navigation.key) {
                        case BSP_INPUT_NAVIGATION_KEY_F1:
                            button_num = 0;
                            break;
                        case BSP_INPUT_NAVIGATION_KEY_F2:
                            button_num = 1;
                            break;
                        case BSP_INPUT_NAVIGATION_KEY_F3:
                            button_num = 2;
                            break;
                        case BSP_INPUT_NAVIGATION_KEY_F4:
                            button_num = 3;
                            break;
                        case BSP_INPUT_NAVIGATION_KEY_F5:
                            button_num = 4;
                            break;
                        case BSP_INPUT_NAVIGATION_KEY_F6:
                            button_num = 5;
                            break;
                        default:
                            button_num = -1;
                            break;
                    }
                }
                xQueueSend(button_indev_queue, &button_num, portMAX_DELAY);
            } else {
                xQueueSend(lvgl_input_event_queue, &event, portMAX_DELAY);
            }
        }
    }
}

void run_meshtastic() {
    ESP_ERROR_CHECK(bsp_input_get_queue(&input_event_queue));

    if (!LittleFS.begin(true)) {
        ESP_LOGE(TAG, "LittleFS Mount Failed");
        return;
    }

    client  = new EspClient;

    lvgl_input_event_queue = xQueueCreate(32, sizeof(bsp_input_event_t));
    display = &BadgeBspDisplay::create(lvgl_input_event_queue);
    lvgl_set_navigation_mode(NAVIGATION_USE_ARROW_UP_DOWN);

    DisplayDriverConfig cfg = DisplayDriverConfig();
    cfg.device(DisplayDriverConfig::device_t::CUSTOM_TFT);

    gui = TFTView_320x240::instance(cfg, display);

    InputDriver* input_driver = gui->getInputDriver();
    input_driver->setKeyboard(lvgl_get_indev());

    lvgl_lock();
    gui->init(client);
    lvgl_unlock();

    bat = new BadgeBspBatteryLevel(gui);

    xTaskCreatePinnedToCore(meshtastic_task_handler, "meshtastic-gui", 10240, NULL, 1, &meshtastic_task_handle, 1);

    xTaskCreatePinnedToCore(input_task, "input-task", 2048, NULL, 1, &input_task_handle, 0);
}
