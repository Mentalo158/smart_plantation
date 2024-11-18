#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "common/task_common.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "backend/wifi-server.h"

// Funktion zur Überwachung des Stackverbrauchs
void monitor_stack_usage()
{
    UBaseType_t sntp_stack_usage = uxTaskGetStackHighWaterMark(NULL); // Stack-Wasserstand für den aktuellen Task
    ESP_LOGI("Stack Monitoring", "Free stack space for current task: %d bytes", sntp_stack_usage);

    // Hier könnte man spezifische Stack-Messung für die Webserver-Task einfügen
    // Wenn die Task-ID bekannt ist, können wir den Stack von webServerTask separat überprüfen
    TaskHandle_t webserver_task_handle = xTaskGetHandle("Web Server Task"); // Hole Handle für Webserver Task
    if (webserver_task_handle != NULL)
    {
        UBaseType_t webserver_stack_usage = uxTaskGetStackHighWaterMark(webserver_task_handle); // Stack-Wasserstand des Webserver Tasks
        ESP_LOGI("Stack Monitoring", "Free stack space for Web Server Task: %d bytes", webserver_stack_usage);
    }
}

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

    // Tasks erstellen
    xTaskCreatePinnedToCore(moisture_task, "Moisture Sensor Task", 2048, NULL, 1, NULL, 1);     // ADC Task auf Core 1
    xTaskCreatePinnedToCore(dhtTask, "DHT Sensor Task", 2048, NULL, 1, NULL, 1);                // DHT Task auf Core 1
    // xTaskCreatePinnedToCore(light_sensor_task, "Light Sensor Task", 2048, NULL, 1, NULL, 1);    // ADC Task auf Core 1
    xTaskCreatePinnedToCore(led_task, "LED Task", 2048, NULL, 1, NULL, 1);                      // LED Task auf Core 1
    xTaskCreatePinnedToCore(time_sync_task, "Time Sync Task", 4096, NULL, 1, NULL, 1);          // SNTP Task auf Core 1
    xTaskCreatePinnedToCore(pump_control_task, "Pump Control Task", 8192, NULL, 1, NULL, 1);    // SNTP Task auf Core 1
    xTaskCreatePinnedToCore(fan_control_task, "Fan Control Task", 2048, NULL, 1, NULL, 1);      // FAN_CONTROL Task auf Core 1
    xTaskCreatePinnedToCore(webServerTask, "Web Server Task", 8192, NULL, 1, NULL, 0);          // Webserver auf Core 0


    // Überwachung des Stackverbrauchs
    // monitor_stack_usage();
}
