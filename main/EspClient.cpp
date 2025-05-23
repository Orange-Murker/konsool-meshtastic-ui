#include "EspClient.h"
#include "comms/SerialClient.h"
#include "driver/uart.h"
#include "esp_err.h"
#include "esp_log.h"
#include "freertos/task.h"
#include "hal/uart_types.h"

#define BUF_SIZE (1024)
static const uart_port_t uart_num      = UART_NUM_1;
static uint8_t           buf[BUF_SIZE] = {0};

static char const TAG[] = "EspClient";

EspClient::EspClient(void) {
}

void EspClient::init(void) {
    SerialClient::init();
}

bool EspClient::connect(void) {
    ESP_LOGI(TAG, "Connecting...");
    uart_config_t uart_config = {
        .baud_rate           = 38400,
        .data_bits           = UART_DATA_8_BITS,
        .parity              = UART_PARITY_DISABLE,
        .stop_bits           = UART_STOP_BITS_1,
        .flow_ctrl           = UART_HW_FLOWCTRL_DISABLE,
        .rx_flow_ctrl_thresh = 122,
    };
    ESP_ERROR_CHECK(uart_param_config(uart_num, &uart_config));
    ESP_ERROR_CHECK(uart_set_pin(UART_NUM_1, 16, 18, -1, -1));

    int intr_alloc_flags = 0;
    ESP_ERROR_CHECK(uart_driver_install(uart_num, BUF_SIZE * 2, BUF_SIZE * 2, 0, NULL, intr_alloc_flags));

    connected = true;
    return true;
}

bool EspClient::disconnect(void) {
    ESP_ERROR_CHECK(uart_driver_delete(uart_num));
    connected = false;
    return false;
}

bool EspClient::isConnected(void) {
    return uart_is_driver_installed(uart_num);
}

bool EspClient::send(const uint8_t* buf, size_t len) {
    uart_write_bytes(uart_num, buf, len);
    return true;
}

size_t EspClient::receive(uint8_t* buf, size_t space_left) {
    size_t length = 0;
    ESP_ERROR_CHECK(uart_get_buffered_data_len(uart_num, &length));
    if (length > space_left) {
        length = space_left;
    }
    if (length != 0) {
        length = uart_read_bytes(uart_num, buf, length, 1000);
    }

    return length;
}

EspClient::~EspClient() {
    if (isConnected()) {
        disconnect();
    }
};
