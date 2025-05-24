#include "graphics/DeviceGUI.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

class Meshtastic {
   public:
    Meshtastic();

   private:
    TaskHandle_t task;
};
