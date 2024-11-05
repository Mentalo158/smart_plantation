#include "task_common.h"
#include "sensors/adc_read.h"
#include "backend/wifi-server.h"
#include "backend/mdns_server.h"
#include "sensors/temperature_sensor.h"
#include "peripherals/led_rgb_control.h"
#include "esp_log.h"
#include "backend/sntp_client.h"
#include "common/config_storage.h"

#define QUEUE_LENGTH 1          
#define ITEM_SIZE sizeof(float) 

// Globales Handle für die ADC-Daten-Queue und DHT-Daten-Queue
QueueHandle_t moistureDataQueue;
QueueHandle_t dhtDataQueue;
QueueHandle_t lightDataQueue;
QueueHandle_t led_queue;

void moisture_task(void *pvParameters)
{
    adc_init(MOISTURE_CHANNEL, ADC_WIDTH_BIT_12, MOISTURE_ATTEN); 

    while (1)
    {
        float moisturePercentage = adc_read_sensor(MOISTURE_CHANNEL, MOISTURE_ADC_MAX_VALUE); 

        if (xQueueOverwrite(moistureDataQueue, &moisturePercentage) != pdPASS)
        {
            ESP_LOGE("ADC_Sensor", "Failed to overwrite ADC value in queue"); 
        }
        else
        {
            ESP_LOGI("ADC_Sensor", "ADC Value: %.2f%%", moisturePercentage); 
        }

        vTaskDelay(pdMS_TO_TICKS(1000)); 
    }
}

void dhtTask(void *pvParameters)
{
    ESP_LOGI("DHT_Sensor", "DHT Sensor Task gestartet...");

    vTaskDelay(pdMS_TO_TICKS(2000));

    while (1)
    {
        // Lese die Daten vom DHT-Sensor
        dht_data_t dhtData = read_temperature_sensor(DHT_GPIO_PIN); 

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
    adc_init(LIGHT_CHANNEL, ADC_WIDTH_BIT_12, LIGHT_ATTEN);

    while (1)
    {
        float lightPercentage = adc_read_sensor(LIGHT_CHANNEL, LIGHT_ADC_MAX_VALUE);

        // Versuche, den ADC-Wert in die Queue zu schreiben
        if (xQueueOverwrite(lightDataQueue, &lightPercentage) != pdPASS)
        {
            ESP_LOGE("Light_Sensor", "Failed to overwrite light sensor value in queue");
        }
        else
        {
            ESP_LOGI("Light_Sensor", "Light Sensor Value: %.2f", lightPercentage);
        }

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void led_task(void *pvParameters)
{
    led_color_t led_data;
    rgb_led_init(LEDC_OUTPUT_R_PIN, LEDC_OUTPUT_G_PIN, LEDC_OUTPUT_B_PIN);

    config_t config;
    load_config(&config);

    rgb_set_color(config.red, config.green, config.blue);

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
}

void time_sync_task(void *pvParameter)
{
    const TickType_t xDelay = 1000 / portTICK_PERIOD_MS; // 60 Sekunden
    for (;;)
    {
        sntp_update_time(); // Aktualisiere die Zeit
        vTaskDelay(xDelay); // Warte eine Minute
    }
}

void webServerTask(void *pvParameters)
{
    // Webserver starten
    httpd_handle_t server = start_webserver();
    if (server == NULL)
    {
        ESP_LOGE("WebServer", "Failed to start web server"); 
        vTaskDelete(NULL);                                   
    }

    
    start_mdns(MDNS_NAME, INSTANCE_NAME, SERVICE_TYPE, PROTOCOL, PORT);

    while (1)
    {
        vTaskDelay(pdMS_TO_TICKS(1000)); 
    }
}

void init_queue()
{
    moistureDataQueue = xQueueCreate(QUEUE_LENGTH, ITEM_SIZE);
    if (moistureDataQueue == NULL)
    {
        ESP_LOGE("Task_Common", "ADC Queue creation failed!");
    }

    dhtDataQueue = xQueueCreate(QUEUE_LENGTH, sizeof(dht_data_t)); 
    if (dhtDataQueue == NULL)
    {
        ESP_LOGE("Task_Common", "DHT Queue creation failed!"); 
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
