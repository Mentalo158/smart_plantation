#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "common/task_common.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "backend/http_server.h"
#include "backend/wifi_connection.h"
#include "driver/gpio.h"


void app_main()
{
    esp_err_t ret = nvs_flash_init();

    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    // Queue init
    init_queue();

    // WLAN connect
    wifi_connection();

    xTaskCreatePinnedToCore(moisture_task, "Moisture Sensor Task", 2048, NULL, 2, NULL, 0);
    xTaskCreatePinnedToCore(dhtTask, "DHT Sensor Task", 2048, NULL, 2, NULL, 0);
    xTaskCreatePinnedToCore(light_sensor_task, "Light Sensor Task", 2048, NULL, 2, NULL, 0);
    xTaskCreatePinnedToCore(led_task, "LED Task", 2048, NULL, 1, NULL, 1);
    xTaskCreatePinnedToCore(time_sync_task, "Time Sync Task", 2048, NULL, 1, NULL, 1);
    xTaskCreatePinnedToCore(pump_control_task, "Pump Control Task", 8192, NULL, 4, NULL, 0);
    xTaskCreatePinnedToCore(fan_control_task, "Fan Control Task", 2048, NULL, 4, NULL, 1);
    xTaskCreatePinnedToCore(pump_water_based_on_soil_task, "Pump Water Based on Soil Task", 4096, NULL, 4, NULL, 0);
    xTaskCreatePinnedToCore(webServerTask, "Web Server Task", 8192, NULL, 3, NULL, 1);

    // TODO doesnt work lol
    //xTaskCreate(dynamicLightTask, "Dynamic Light Task", 4096, NULL, 3, NULL);
}
