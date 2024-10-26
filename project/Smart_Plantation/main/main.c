#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "common/task_common.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "backend/wifi-server.h"

/**
 * @brief Hauptanwendung.
 *
 * Diese Funktion wird beim Start der Anwendung aufgerufen.
 * Sie initialisiert den Non-Volatile Storage (NVS) Flash-Speicher,
 * stellt eine WLAN-Verbindung her und erstellt die erforderlichen Tasks
 * für den ADC-Sensor und den Webserver.
 */
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

    // Tasks erstellen
    xTaskCreatePinnedToCore(moisture_task, "Moisture Sensor Task", 2048, NULL, 1, NULL, 1);  // ADC Task auf Core 1
    xTaskCreatePinnedToCore(dhtTask, "DHT Sensor Task", 2048, NULL, 1, NULL, 1);             // DHT Task auf Core 1
    xTaskCreatePinnedToCore(light_sensor_task, "Light Sensor Task", 2048, NULL, 1, NULL, 1); // ADC Task auf Core 1
    xTaskCreatePinnedToCore(led_task, "LED Task", 2048, NULL, 1, NULL, 1);                   // ADC Task auf Core 1
    xTaskCreatePinnedToCore(webServerTask, "Web Server Task", 8192, NULL, 1, NULL, 0); // Webserver auf Core 0
    xTaskCreatePinnedToCore(time_sync_task, "Time Sync Task", 2048, NULL, 1, NULL, 1);
}
