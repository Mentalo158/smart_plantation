#include <stdio.h>
#include <inttypes.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_chip_info.h"
#include "esp_flash.h"
#include "esp_system.h"
#include "wifi-server.h"

void app_main(void)
{
    wifi_connection();
    //xTaskCreatePinnedToCore(&web_server_task, "web_server_task", 4096, NULL, 5, NULL, 1);
}
