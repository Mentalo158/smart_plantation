#include "task_common.h"
#include "sensors/adc_read.h"
#include "backend/wifi-server.h"
#include "backend/mdns_server.h"
#include "sensors/temperature_sensor.h"
#include "peripherals/led_rgb.h"
#include "esp_log.h"

#define QUEUE_LENGTH 1          
#define ITEM_SIZE sizeof(float) 

// Globales Handle für die ADC-Daten-Queue und DHT-Daten-Queue
QueueHandle_t moistureDataQueue;
QueueHandle_t dhtDataQueue;
QueueHandle_t lightDataQueue;
QueueHandle_t led_queue; // Warteschlange für LED-Daten

/**
 * @brief ADC Sensor Task.
 *
 * Diese Funktion liest kontinuierlich ADC-Werte von einem
 * analogen Sensor (MOISTURE_CHANNEL) und schreibt diese Werte in
 * eine Queue. Es wird jede Sekunde ein neuer Wert gelesen.
 *
 * @param pvParameters Pointer zu den übergebenen Parametern (nicht verwendet).
 */
void moisture_task(void *pvParameters)
{
    adc_init(MOISTURE_CHANNEL, ADC_WIDTH_BIT_12, MOISTURE_ATTEN); // ADC initialisieren, MOISTURE_CHANNEL ist ADC1_CHANNEL_4 (GPIO32)

    while (1)
    {
        float moisturePercentage = adc_read_sensor(MOISTURE_CHANNEL, MOISTURE_ADC_MAX_VALUE); // ADC-Wert lesen von MOISTURE_CHANNEL

        // Versuche, den ADC-Wert in die Queue zu schreiben
        if (xQueueOverwrite(moistureDataQueue, &moisturePercentage) != pdPASS)
        {
            ESP_LOGE("ADC_Sensor", "Failed to overwrite ADC value in queue"); // Fehler beim Schreiben in die Queue
        }
        else
        {
            ESP_LOGI("ADC_Sensor", "ADC Value: %.2f%%", moisturePercentage); // Erfolgreiches Schreiben, Ausgabe des Wertes
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
        dht_data_t dhtData = read_temperature_sensor(DHT_GPIO_PIN); // Ruft die Funktion zum Lesen der Sensorwerte auf

        // Schreibe die Temperatur und Feuchtigkeit in die Queue
        if (xQueueOverwrite(dhtDataQueue, &dhtData) != pdPASS)
        {
            ESP_LOGE("DHT_Sensor", "Failed to overwrite DHT data in queue");
        }

        vTaskDelay(pdMS_TO_TICKS(5000));
    }
}

void light_sensor_task(void *pvParameters)
{
    adc_init(LIGHT_CHANNEL, ADC_WIDTH_BIT_12, LIGHT_ATTEN); // Verwende den richtigen ADC-Kanal für den Lichtsensor

    while (1)
    {
        float lightPercentage = adc_read_sensor(LIGHT_CHANNEL, LIGHT_ADC_MAX_VALUE); // ADC-Wert vom Lichtsensor lesen

        // Versuche, den ADC-Wert in die Queue zu schreiben
        if (xQueueOverwrite(lightDataQueue, &lightPercentage) != pdPASS)
        {
            ESP_LOGE("Light_Sensor", "Failed to overwrite light sensor value in queue");
        }
        else
        {
            ESP_LOGI("Light_Sensor", "Light Sensor Value: %.2f", lightPercentage); // Erfolgreiches Schreiben, Ausgabe des Wertes
        }

        vTaskDelay(pdMS_TO_TICKS(1000)); // 1 Sekunde warten
    }
}

void led_task(void *pvParameters)
{
    led_color_t led_data;
    rgb_led_init(LEDC_OUTPUT_R_PIN, LEDC_OUTPUT_G_PIN, LEDC_OUTPUT_B_PIN);

    while (1)
    {
        // Warte, bis neue RGB-Daten in die Warteschlange eingehen
        if (xQueueReceive(led_queue, &led_data, portMAX_DELAY))
        {
            // LED-Werte setzen
            rgb_set_color(led_data.red, led_data.green, led_data.blue);
            printf("LED updated: R=%d, G=%d, B=%d\n", led_data.red, led_data.green, led_data.blue);
        }
    }
};

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
    moistureDataQueue = xQueueCreate(QUEUE_LENGTH, ITEM_SIZE);
    if (moistureDataQueue == NULL)
    {
        ESP_LOGE("Task_Common", "ADC Queue creation failed!"); // Fehler beim Erstellen der ADC Queue
    }

    // DHT Queue initialisieren
    dhtDataQueue = xQueueCreate(QUEUE_LENGTH, sizeof(dht_data_t)); // Queue für die Struktur dht_data_t
    if (dhtDataQueue == NULL)
    {
        ESP_LOGE("Task_Common", "DHT Queue creation failed!"); // Fehler beim Erstellen der DHT Queue
    }

    lightDataQueue = xQueueCreate(QUEUE_LENGTH, ITEM_SIZE);
    if (lightDataQueue == NULL)
    {
        ESP_LOGE("Task_Common", "Light Sensor Queue creation failed!");
    }

    led_queue = xQueueCreate(QUEUE_LENGTH, sizeof(led_color_t));
    if (led_queue == NULL)
    {
        ESP_LOGE("Task_Common", "LED Queue creation failed!");
    }
}
