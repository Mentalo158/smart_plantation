#include "task_common.h"
#include "adc_sensor.h"
#include "wifi-server.h"
#include "mdns_server.h"
#include "esp_log.h"

#define QUEUE_LENGTH 1          // Größe 1 für immer den neuesten Wert
#define ITEM_SIZE sizeof(float) // Größe eines einzelnen Queue-Elements

// Globales Handle für die ADC-Daten-Queue
QueueHandle_t adcDataQueue;

/**
 * @brief ADC Sensor Task.
 *
 * Diese Funktion liest kontinuierlich ADC-Werte von einem
 * analogen Sensor (ADC_CH_4) und schreibt diese Werte in
 * eine Queue. Es wird jede Sekunde ein neuer Wert gelesen.
 *
 * @param pvParameters Pointer zu den übergebenen Parametern (nicht verwendet).
 */
void sensorTask(void *pvParameters)
{
    adc_init(ADC_CH_4, ADC_WIDTH_BIT_12, MOISTURE_ATTEN); // ADC initialisieren, ADC_CH_4 ist ADC1_CHANNEL_4 (GPIO32)

    while (1)
    {
        float adcValue = adc_read_sensor(ADC_CH_4); // ADC-Wert lesen von ADC_CH_4

        // Versuche, den ADC-Wert in die Queue zu schreiben
        if (xQueueOverwrite(adcDataQueue, &adcValue) != pdPASS)
        {
            ESP_LOGE("ADC_Sensor", "Failed to overwrite ADC value in queue"); // Fehler beim Schreiben in die Queue
        }
        else
        {
            ESP_LOGI("ADC_Sensor", "ADC Value: %.2f%%", adcValue); // Erfolgreiches Schreiben, Ausgabe des Wertes
        }

        vTaskDelay(pdMS_TO_TICKS(1000)); // 1 Sekunde warten
    }
}

/**
 * @brief Webserver und mDNS Task.
 *
 * Diese Funktion startet einen Webserver und einen
 * mDNS-Dienst, um das Gerät im Netzwerk auffindbar zu machen.
 * Der Task bleibt in einer Endlosschleife, um die
 * Funktionalität des Servers aufrechtzuerhalten.
 *
 * @param pvParameters Pointer zu den übergebenen Parametern (nicht verwendet).
 */
void webServerTask(void *pvParameters)
{
    // Webserver starten
    httpd_handle_t server = start_webserver();
    if (server == NULL)
    {
        ESP_LOGE("WebServer", "Failed to start web server"); // Fehler beim Starten des Webservers
        vTaskDelete(NULL);                                   // Task beenden, wenn der Server nicht gestartet werden kann
    }

    // mDNS Dienst starten
    start_mdns(MDNS_NAME, INSTANCE_NAME, SERVICE_TYPE, PROTOCOL, PORT);

    while (1)
    {
        vTaskDelay(pdMS_TO_TICKS(1000)); // 1 Sekunde warten
    }
}

/**
 * @brief Initialisiert die ADC-Daten-Queue.
 *
 * Diese Funktion erstellt eine Queue zur Kommunikation
 * zwischen den Tasks. Die Queue hat eine definierte Länge
 * und Größe für die darin enthaltenen Elemente.
 */
void init_queue()
{
    adcDataQueue = xQueueCreate(QUEUE_LENGTH, ITEM_SIZE); // Erstelle die Queue
    if (adcDataQueue == NULL)
    {
        ESP_LOGE("Task_Common", "Queue creation failed!"); // Fehler beim Erstellen der Queue
    }
}
