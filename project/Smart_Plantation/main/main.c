#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "common/task_common.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "backend/http_server.h"
#include "backend/wifi_connection.h"
#include "driver/gpio.h"

// Hauptanwendung
void app_main()
{
    // Initialisiere den NVS-Flash-Speicher
    esp_err_t ret = nvs_flash_init();

    // Überprüfe auf Fehler beim Initialisieren des NVS
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    // Queue initialisieren
    init_queue();

    // WLAN-Verbindung herstellen
    wifi_connection();

    xTaskCreate(moisture_task, "Moisture Sensor Task", 2048, NULL, 3, NULL);
    xTaskCreate(dhtTask, "DHT Sensor Task", 2048, NULL, 3, NULL);
    xTaskCreate(light_sensor_task, "Light Sensor Task", 4096, NULL, 3, NULL);
    xTaskCreate(led_task, "LED Task", 2048, NULL, 1, NULL);
    xTaskCreate(time_sync_task, "Time Sync Task", 4096, NULL, 1, NULL);
    xTaskCreate(pump_control_task, "Pump Control Task", 8192, NULL, 4, NULL);
    xTaskCreate(fan_control_task, "Fan Control Task", 2048, NULL, 4, NULL);
    xTaskCreate(pump_water_based_on_soil_task, "Pump Control Task", 4096, NULL, 4, NULL);
    xTaskCreate(webServerTask, "Web Server Task", 8192, NULL, 3, NULL);
}
