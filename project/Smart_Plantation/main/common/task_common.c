#include "task_common.h"
#include "backend/http_server.h"
#include "backend/mdns_server.h"
#include "sensors/temperature_sensor.h"
#include "peripherals/led_rgb_control.h"
#include "peripherals/wifi_plug.h"
#include "peripherals/fan_control.h"
#include "esp_log.h"
#include "backend/sntp_client.h"
#include "common/config_storage.h"
#include "sensors/light_sensor.h"

#include "sensors/adc_read.h"

#define QUEUE_LENGTH 1
#define ITEM_SIZE sizeof(float)

QueueHandle_t moistureDataQueue;
QueueHandle_t dhtDataQueue;
QueueHandle_t lightDataQueue;
QueueHandle_t led_queue;
QueueHandle_t pump_queue;
QueueHandle_t fan_queue;
QueueHandle_t fanSpeedQueue;
QueueHandle_t soil_queue;
QueueHandle_t moisture_enabled_queue;
QueueHandle_t tempThresholdQueue;

void moisture_task(void *pvParameters)
{
    adc_init(MOISTURE_CHANNEL, ADC_WIDTH_BIT_12, MOISTURE_ATTEN);

    while (1)
    {
        float moisturePercentage = adc_read_sensor(MOISTURE_CHANNEL, MOISTURE_ADC_MAX_VALUE);

        if (xQueueOverwrite(moistureDataQueue, &moisturePercentage) != pdPASS)
        {
            ESP_LOGE("Moisture_Sensor", "Failed to overwrite ADC value in queue");
        }
        else
        {
            ESP_LOGI("Moisture_Sensor", "ADC Value: %.2f%%", moisturePercentage);
        }

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void pump_water_based_on_soil_task(void *pvParameters)
{
    config_t config;
    load_config(&config);

    uint8_t soil_threshold = config.moisture_threshold;
    bool moisture_enabled = (config.moisture_enabled != 0);
    float moisturePercentage = 0.0f;

    if (moisture_enabled && xQueuePeek(moistureDataQueue, &moisturePercentage, 0) == pdTRUE)
    {
        if (moisturePercentage <= soil_threshold)
        {
            tasmota_toggle_power(2000);
            vTaskDelay(pdMS_TO_TICKS(10000));
        }
    }

    while (1)
    {

        uint8_t moisture_enabled_flag;
        if (xQueueReceive(moisture_enabled_queue, &moisture_enabled_flag, 0) == pdTRUE)
        {
            moisture_enabled = (moisture_enabled_flag != 0);
        }

        if (!moisture_enabled)
        {
            vTaskDelay(pdMS_TO_TICKS(1000));
            continue;
        }

        uint8_t new_threshold;
        if (xQueueReceive(soil_queue, &new_threshold, 0) == pdTRUE)
        {
            soil_threshold = new_threshold;
        }

        if (xQueuePeek(moistureDataQueue, &moisturePercentage, pdMS_TO_TICKS(100)) == pdTRUE)
        {
            if (moisturePercentage <= soil_threshold)
            {
                tasmota_toggle_power(2000);
                vTaskDelay(pdMS_TO_TICKS(10000));
            }
        }

        vTaskDelay(pdMS_TO_TICKS(3000));
    }
}

void dhtTask(void *pvParameters)
{
    ESP_LOGI("DHT_Sensor", "DHT Sensor Task gestartet...");

    vTaskDelay(pdMS_TO_TICKS(2000));

    dht_data_t dhtData;

    while (1)
    {

        if (read_temperature_sensor(DHT_GPIO_PIN, &dhtData) == ESP_OK)
        {

            if (xQueueOverwrite(dhtDataQueue, &dhtData) != pdPASS)
            {
                ESP_LOGE("DHT_Sensor", "Failed to overwrite DHT data in queue");
            }
        }
        vTaskDelay(pdMS_TO_TICKS(5000));
    }
}

void light_sensor_task(void *pvParameters)
{

    const int max_retries = 5;
    int retry_count = 0;
    esp_err_t init_status = ESP_FAIL;

    while (retry_count < max_retries)
    {
        init_status = bh1750_init(I2C_SCL_PIN, I2C_SDA_PIN);
        if (init_status == ESP_OK)
        {
            ESP_LOGI("LightSensor", "Sensor erfolgreich initialisiert (Versuch %d).", retry_count + 1);
            break;
        }
        else
        {
            ESP_LOGW("LightSensor", "Sensorinitialisierung fehlgeschlagen (Versuch %d von %d).", retry_count + 1, max_retries);
            vTaskDelay(pdMS_TO_TICKS(1000));
        }
        retry_count++;
    }

    if (init_status != ESP_OK)
    {
        ESP_LOGE("LightSensor", "Sensorinitialisierung endgültig fehlgeschlagen nach %d Versuchen.", max_retries);
        vTaskDelete(NULL);
        return;
    }

    while (1)
    {

        LightState state = get_light_state();

        if (xQueueOverwrite(lightDataQueue, &state) != pdPASS)
        {
            ESP_LOGE("LightSensor", "Fehler beim Übertragen der Lichtdaten");
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

        if (xQueueReceive(led_queue, &led_data, portMAX_DELAY))
        {

            rgb_set_color(led_data.red, led_data.green, led_data.blue);
            printf("LED updated: R=%d, G=%d, B=%d\n", led_data.red, led_data.green, led_data.blue);
        }
    }
}

void time_sync_task(void *pvParameter)
{
    const TickType_t xDelay = 1000 / portTICK_PERIOD_MS;
    for (;;)
    {
        sntp_update_time();
        vTaskDelay(xDelay);
    }
}

void pump_control_task(void *pvParameter)
{
    pump_times config_data;
    bool pump_triggered_today = false;

    config_t initial_config;
    load_config(&initial_config);

    for (int i = 0; i < 7; i++)
    {
        config_data.hours[i] = initial_config.hours[i];
        config_data.minutes[i] = initial_config.minutes[i];
    }
    config_data.days = initial_config.days;

    while (1)
    {

        if (xQueueReceive(pump_queue, &config_data, pdMS_TO_TICKS(100)))
        {
            ESP_LOGI("PUMP_TASK", "Neue Konfiguration empfangen");

            pump_triggered_today = false;
        }

        time_t now = get_current_time();
        struct tm timeinfo;
        localtime_r(&now, &timeinfo);

        int current_day = get_current_day_of_week();
        bool should_trigger_today = false;

        if ((config_data.days & (1 << current_day)))
        {
            if (timeinfo.tm_hour == config_data.hours[current_day] && timeinfo.tm_min == config_data.minutes[current_day])
            {
                should_trigger_today = true;
            }
        }

        if (should_trigger_today && !pump_triggered_today)
        {
            ESP_LOGI("PUMP_TASK", "Pumpe aktivieren: Geplante Zeit erreicht.");
            tasmota_toggle_power(2000);

            pump_triggered_today = true;
        }

        if (timeinfo.tm_hour == 0 && timeinfo.tm_min == 0)
        {
            pump_triggered_today = false;
        }

        vTaskDelay(60000 / portTICK_PERIOD_MS);
    }
}

void fan_control_task(void *pvParameters)
{
    config_t initial_config;
    load_config(&initial_config); // Lade initiale Konfiguration

    dht_data_t dhtData;

    bool fanRunning = false;
    bool isFanEnabled = (initial_config.temp_enabled != 0);
    const uint32_t pulses_per_revolution = 2;

    fan_init(FAN_PWM_PIN, FAN_CONTROL_PIN, FAN_TACH_PIN);

    // Temperatur-Schwellenwert aus der Konfiguration
    int tempThreshold = initial_config.temp_threshold;

    while (1)
    {
        uint8_t tempEnabledFlag;

        // Prüfen, ob der Lüfter aktiviert/deaktiviert werden soll
        if (xQueueReceive(fan_queue, &tempEnabledFlag, pdMS_TO_TICKS(100)))
        {
            isFanEnabled = (tempEnabledFlag != 0);
        }

        if (isFanEnabled)
        {
            if (!fanRunning)
            {
                fan_on(FAN_CONTROL_PIN, 0);
                fanRunning = true; // Status aktualisieren
                printf("Lüfter eingeschaltet.\n");
            }

            uint32_t rpm = fan_get_speed_rpm(pulses_per_revolution);

            // RPM-Wert in die Queue schreiben
            if (xQueueOverwrite(fanSpeedQueue, &rpm) != pdPASS)
            {
                ESP_LOGE("Lüfter", "Fehler beim Übertragen der Lüftergeschwindigkeit");
            }

            // Temperaturdaten prüfen
            if (xQueuePeek(dhtDataQueue, &dhtData, pdMS_TO_TICKS(100)) == pdTRUE)
            {
                // Prüfen, ob ein neuer Temperatur-Schwellenwert empfangen wird
                uint8_t newTempThreshold;
                if (xQueueReceive(tempThresholdQueue, &newTempThreshold, pdMS_TO_TICKS(100)) == pdTRUE)
                {
                    tempThreshold = newTempThreshold; // Schwellenwert aktualisieren
                }

                // Berechnung der Lüftergeschwindigkeit basierend auf dem Temperatur-Schwellenwert
                int tempDifference = dhtData.temperature - tempThreshold;
                int fan_speed = tempDifference * 10;

                // Begrenzung der Lüftergeschwindigkeit
                fan_speed = (fan_speed > 255) ? 255 : (fan_speed < 30 ? 30 : fan_speed);

                // Lüftergeschwindigkeit setzen
                fan_set_speed(fan_speed);

                printf("Lüftergeschwindigkeit: %d (Temperatur: %.2f°C, Schwelle: %d°C, RPM: %ld)\n",
                       fan_speed, dhtData.temperature, tempThreshold, rpm);
            }
            else
            {
                printf("Keine Temperaturdaten verfügbar, Lüfter läuft weiter.\n");
            }
        }
        else
        {
            fan_off(FAN_CONTROL_PIN);
            printf("Lüfter deaktiviert durch Benutzer.\n");
        }

        vTaskDelay(pdMS_TO_TICKS(3000));
    }
}

void webServerTask(void *pvParameters)
{

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
    moistureDataQueue = xQueueCreate(QUEUE_LENGTH, sizeof(float));
    if (moistureDataQueue == NULL)
    {
        ESP_LOGE("Task_Common", "ADC Queue creation failed!");
    }

    dhtDataQueue = xQueueCreate(QUEUE_LENGTH, sizeof(dht_data_t));
    if (dhtDataQueue == NULL)
    {
        ESP_LOGE("Task_Common", "DHT Queue creation failed!");
    }

    lightDataQueue = xQueueCreate(QUEUE_LENGTH, sizeof(LightState));
    if (lightDataQueue == NULL)
    {
        ESP_LOGE("Task_Common", "Light Sensor Queue creation failed!");
    }

    led_queue = xQueueCreate(QUEUE_LENGTH, sizeof(led_color_t));
    if (led_queue == NULL)
    {
        ESP_LOGE("Task_Common", "LED Queue creation failed!");
    }

    pump_queue = xQueueCreate(QUEUE_LENGTH, sizeof(pump_times));
    if (pump_queue == NULL)
    {
        ESP_LOGE("Task_Common", "Configuration Queue creation failed!");
    }

    fan_queue = xQueueCreate(QUEUE_LENGTH, sizeof(uint8_t));
    {
        ESP_LOGE("Task_Common", "Fan Queue creation failed!");
    }

    fanSpeedQueue = xQueueCreate(QUEUE_LENGTH, sizeof(uint32_t));
    if (fanSpeedQueue == NULL)
    {
        ESP_LOGE("Task_Common", "Fan Queue Speed creation failed!");
    }

    soil_queue = xQueueCreate(QUEUE_LENGTH, sizeof(uint8_t));
    if (soil_queue == NULL)
    {
        ESP_LOGE("Task_Common", "Soil Queue creation failed!");
    }

    moisture_enabled_queue = xQueueCreate(QUEUE_LENGTH, sizeof(uint8_t));
    if (moisture_enabled_queue == NULL)
    {
        ESP_LOGE("Task_Common", "Moisture Enabled Queue creation failed!");
    }

    tempThresholdQueue = xQueueCreate(QUEUE_LENGTH, sizeof(uint8_t));
    if (tempThresholdQueue == NULL)
    {
        ESP_LOGE("Task_Common", "tempThresholdQueue creation failed!");
    }
}