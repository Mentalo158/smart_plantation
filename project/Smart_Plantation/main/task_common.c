#include "task_common.h"
#include "adc_sensor.h"
#include "wifi-server.h"
#include "mdns_server.h"
#include "esp_log.h"
#include "temperature_sensor.h" // Header für den DHT-Sensor Task

#define QUEUE_LENGTH 1          // Größe 1 für immer den neuesten Wert
#define ITEM_SIZE sizeof(float) // Größe eines einzelnen Queue-Elements

// Globales Handle für die ADC-Daten-Queue und DHT-Daten-Queue
QueueHandle_t adcDataQueue;
QueueHandle_t dhtDataQueue; 

/**
 * @brief ADC Sensor Task.
 *
 * Diese Funktion liest kontinuierlich ADC-Werte von einem
 * analogen Sensor (ADC_CH_4) und schreibt diese Werte in
 * eine Queue. Es wird jede Sekunde ein neuer Wert gelesen.
 *
 * @param pvParameters Pointer zu den übergebenen Parametern (nicht verwendet).
 */
void adcTask(void *pvParameters)
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
 * @brief DHT Sensor Task.
 *
 * Diese Funktion liest kontinuierlich Temperatur- und Feuchtigkeitswerte
 * von einem DHT-Sensor und schreibt diese Werte in eine Queue.
 *
 * @param pvParameters Pointer zu den übergebenen Parametern (nicht verwendet).
 */
void dhtTask(void *pvParameters)
{
    ESP_LOGI("DHT_Sensor", "DHT Sensor Task gestartet...");

    // Wartezeit vor dem Start (2 Sekunden)
    vTaskDelay(pdMS_TO_TICKS(2000));

    while (1)
    {
        // Lese die Daten vom DHT-Sensor
        dht_data_t dhtData = read_temperature_sensor(); // Ruft die Funktion zum Lesen der Sensorwerte auf

        // Schreibe die Temperatur und Feuchtigkeit in die Queue
        if (xQueueOverwrite(dhtDataQueue, &dhtData) != pdPASS)
        {
            ESP_LOGE("DHT_Sensor", "Failed to overwrite DHT data in queue");
        }

        vTaskDelay(pdMS_TO_TICKS(5000));
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
 * @brief Initialisiert die Queues.
 *
 * Diese Funktion erstellt die Queues zur Kommunikation
 * zwischen den Tasks. Es gibt eine Queue für die ADC-Daten
 * und eine separate für die DHT-Daten.
 */
void init_queue()
{
    // ADC Queue initialisieren
    adcDataQueue = xQueueCreate(QUEUE_LENGTH, ITEM_SIZE);
    if (adcDataQueue == NULL)
    {
        ESP_LOGE("Task_Common", "ADC Queue creation failed!"); // Fehler beim Erstellen der ADC Queue
    }

    // DHT Queue initialisieren
    dhtDataQueue = xQueueCreate(QUEUE_LENGTH, sizeof(dht_data_t)); // Queue für die Struktur dht_data_t
    if (dhtDataQueue == NULL)
    {
        ESP_LOGE("Task_Common", "DHT Queue creation failed!"); // Fehler beim Erstellen der DHT Queue
    }
}
