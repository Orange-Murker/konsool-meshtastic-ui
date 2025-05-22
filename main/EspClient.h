#pragma once

#include "comms/IClientBase.h"
#include "comms/MeshEnvelope.h"
#include "freertos/FreeRTOS.h"

class EspClient : public IClientBase
{
  public:
    EspClient(void);
    void init(void) override;
    // bool sleep(int16_t pin);
    bool connect(void) override;
    bool disconnect(void) override;
    bool isConnected(void) override;
    bool send(meshtastic_ToRadio &&to) override;
    meshtastic_FromRadio receive(void) override;
    virtual ~EspClient();

  protected:
    static EspClient* instance;
};
