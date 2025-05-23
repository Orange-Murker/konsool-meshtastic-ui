#pragma once

#include "comms/SerialClient.h"

class EspClient : public SerialClient {
   public:
    EspClient(void);
    void init(void) override;
    // bool sleep(int16_t pin);
    bool connect(void) override;
    bool disconnect(void) override;
    bool isConnected(void) override;
    virtual ~EspClient();

   protected:
    // low-level send method to write the buffer to serial
    bool send(const uint8_t* buf, size_t len) override;

    // low-level receive method, periodically being called via thread
    size_t receive(uint8_t* buf, size_t space_left) override;
};
