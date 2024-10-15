#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "task_common.h"
#include "adc_sensor.h"
#include "wifi-server.h"
#include "mdns_server.h"
#include "esp_log.h"
#include "nvs_flash.h"

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
        // Wenn kein Speicherplatz vorhanden ist oder eine neue Version gefunden wurde, lösche den NVS-Speicher
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret); // Überprüfe auf weitere Fehler

    // Queue initialisieren
    init_queue(); // Initialisiert die Queue für die Kommunikation zwischen Tasks

    // WLAN-Verbindung herstellen
    wifi_connection(); // Stellt eine Verbindung zum WLAN her

    // Tasks erstellen
    xTaskCreatePinnedToCore(adcSensorTask, "ADC Sensor Task", 2048, NULL, 1, NULL, 1); // ADC auf Core 1
    xTaskCreatePinnedToCore(webServerTask, "Web Server Task", 8192, NULL, 1, NULL, 0); // Webserver und mDNS auf Core 0
}
