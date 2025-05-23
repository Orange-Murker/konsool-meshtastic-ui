#include "EspClient.h"
#include "comms/MeshEnvelope.h"
#include "driver/uart.h"
#include "esp_err.h"
#include "esp_log.h"
#include "freertos/task.h"
#include "hal/uart_types.h"
#include "meshtastic/mesh.pb.h"

#define BUF_SIZE (1024)
static const uart_port_t uart_num      = UART_NUM_1;
static uint8_t           buf[BUF_SIZE] = {0};

static char const TAG[] = "EspClient";

EspClient* EspClient::instance = nullptr;

EspClient::EspClient(void) {
    ESP_LOGI(TAG, "Created...");
    instance = this;
}

void EspClient::init(void) {
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

    return true;
}

bool EspClient::disconnect(void) {
    ESP_ERROR_CHECK(uart_driver_delete(uart_num));
    return false;
}

bool EspClient::isConnected(void) {
    return uart_is_driver_installed(uart_num);
}

bool EspClient::send(meshtastic_ToRadio&& to) {
    MeshEnvelope                envelope;
    const std::vector<uint8_t>& pb_buf = envelope.encode(to);
    uart_write_bytes(uart_num, &pb_buf[0], pb_buf.size());
    return true;
}

meshtastic_FromRadio EspClient::receive(void) {
    size_t length = 0;
    ESP_ERROR_CHECK(uart_get_buffered_data_len(uart_num, &length));
    if (length > 0) {
        length = uart_read_bytes(uart_num, buf, length, 100);
    }

    size_t payload_len;
    bool   valid = MeshEnvelope::validate(buf, length, payload_len);

    if (valid) {
        MeshEnvelope         envelope(buf, length);
        meshtastic_FromRadio fromRadio = envelope.decode();
        MeshEnvelope::invalidate(buf, length, payload_len);

        if (fromRadio.which_payload_variant != 0) {
            return fromRadio;
        }
    }

    return meshtastic_FromRadio();
}

EspClient::~EspClient() {
    if (isConnected()) {
        disconnect();
    }
};
