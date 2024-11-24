#include "task_common.h"
#include "sensors/adc_read.h"
#include "backend/wifi-server.h"
#include "backend/mdns_server.h"
#include "sensors/temperature_sensor.h"
#include "peripherals/led_rgb_control.h"
#include "peripherals/wifi_plug.h"
#include "peripherals/fan_control.h"
#include "esp_log.h"
#include "backend/sntp_client.h"
#include "common/config_storage.h"
#include "sensors/light_sensor.h"
#include "sensors/moisture_sensor.h"


#define QUEUE_LENGTH 1
#define ITEM_SIZE sizeof(float)

// Globales Handle für die ADC-Daten-Queue und DHT-Daten-Queue
QueueHandle_t moistureDataQueue;
QueueHandle_t dhtDataQueue;
QueueHandle_t lightDataQueue;
QueueHandle_t led_queue;
QueueHandle_t pump_queue;
QueueHandle_t fan_queue;

void moisture_task(void *pvParameters)
{
    // Initialisierung des I2C-Busses für den Feuchtigkeitssensor
    if (init_soil_sensor_i2c(I2C_SCL_PIN_SOIL, I2C_SDA_PIN_SOIL) != ESP_OK)
    {
        ESP_LOGE("Moisture_Sensor", "Fehler bei der I2C-Initialisierung");
        vTaskDelete(NULL); // Task beenden, falls Initialisierung fehlschlägt
    }

    while (1)
    {
        // Feuchtigkeitswert in Prozent lesen
        float moisture_value = readMoistureValueInPercent();

        if (moisture_value != -1.0f) // Überprüfen, ob der Sensorwert gültig ist
        {
            // Feuchtigkeitswert in die Queue schreiben
            if (xQueueOverwrite(moistureDataQueue, &moisture_value) != pdPASS)
            {
                ESP_LOGE("Moisture_Sensor", "Fehler beim Übertragen der Feuchtigkeitsdaten");
            }
            else
            {
                ESP_LOGI("Moisture_Sensor", "Feuchtigkeit: %.2f%%", moisture_value);
            }
        }

        vTaskDelay(pdMS_TO_TICKS(1000)); // 1 Sekunde Verzögerung
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


        // Versuche, den BH1750-Sensor mehrmals zu initialisieren
        const int max_retries = 5; // Maximale Anzahl der Versuche
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
            vTaskDelay(pdMS_TO_TICKS(1000)); // 1 Sekunde warten vor erneutem Versuch
        }
        retry_count++;
    }

    if (init_status != ESP_OK)
    {
        ESP_LOGE("LightSensor", "Sensorinitialisierung endgültig fehlgeschlagen nach %d Versuchen.", max_retries);
        vTaskDelete(NULL); // Task beenden, wenn Sensor nicht initialisiert werden kann
        return;
    }

    while (1)
    {
        // Holen des aktuellen Lichtstatus
        LightState state = get_light_state();

        // Übertragen des Lichtwerts in die Queue
        if (xQueueOverwrite(lightDataQueue, &state) != pdPASS)
        {
            ESP_LOGE("LightSensor", "Fehler beim Übertragen der Lichtdaten");
        }

        // Sensor alle 1000ms abfragen
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

void pump_control_task(void *pvParameter)
{
    pump_times config_data;
    bool pump_triggered_today = false;

    // Initialkonfiguration aus NVS laden
    config_t initial_config;
    load_config(&initial_config);

    // Die Konfiguration auf die entsprechenden Daten setzen (7 Tage, Stunden und Minuten)
    for (int i = 0; i < 7; i++)
    {
        config_data.hours[i] = initial_config.hours[i];     // Stunden für jeden Wochentag
        config_data.minutes[i] = initial_config.minutes[i]; // Minuten für jeden Wochentag
    }
    config_data.days = initial_config.days; // Bitmaske der aktiven Tage

    // TODO: Fehlerbehandlung bei Task-Fehlern einfügen

    while (1)
    {
        // Warten, bis neue Konfigurationsdaten in die Queue geschrieben werden
        if (xQueueReceive(pump_queue, &config_data, pdMS_TO_TICKS(100)))
        {
            ESP_LOGI("PUMP_TASK", "Neue Konfiguration empfangen");

            // Täglichen Trigger zurücksetzen bei neuer Konfiguration
            pump_triggered_today = false;
        }

        // Aktuelle Zeit prüfen
        time_t now = get_current_time();
        struct tm timeinfo;
        localtime_r(&now, &timeinfo);

        int current_day = get_current_day_of_week(); // Aktuellen Wochentag ermitteln
        bool should_trigger_today = false;

        // Prüfen, ob die Pumpe für den aktuellen Tag aktiviert werden soll
        if ((config_data.days & (1 << current_day))) // Prüfen, ob der aktuelle Tag aktiviert ist
        {
            if (timeinfo.tm_hour == config_data.hours[current_day] && timeinfo.tm_min == config_data.minutes[current_day])
            {
                should_trigger_today = true;
            }
        }

        // Wenn die Bedingungen erfüllt sind und die Pumpe noch nicht für den heutigen Tag ausgelöst wurde
        if (should_trigger_today && !pump_triggered_today)
        {
            ESP_LOGI("PUMP_TASK", "Pumpe aktivieren: Geplante Zeit erreicht.");
            tasmota_toggle_power(2000); // Beispielwert für die Dauer

            // Setze den Trigger für den heutigen Tag
            pump_triggered_today = true;
        }

        // Täglichen Auslöser um Mitternacht zurücksetzen
        if (timeinfo.tm_hour == 0 && timeinfo.tm_min == 0)
        {
            pump_triggered_today = false;
        }

        // Warten, bis zur nächsten Minute (um CPU zu schonen)
        vTaskDelay(60000 / portTICK_PERIOD_MS); // Wartezeit für eine Minute
    }
}

void fan_control_task(void *pvParameters)
{
    int fan_pwm_pin = GPIO_NUM_12;    // PWM-Pin für die Lüftergeschwindigkeit
    int fan_tach_pin = GPIO_NUM_18;   // Tachometer-Pin des Lüfters
    int fan_control_pin = GPIO_NUM_0; // Steuer-Pin für den Transistor

    config_t initial_config;
    load_config(&initial_config);

    dht_data_t dhtData;

    // `temp_enabled` aus der Konfiguration interpretieren
    bool isFanEnabled = (initial_config.temp_enabled != 0);

    // Initialisiere den Lüfter mit den Pins
    fan_init(fan_pwm_pin, fan_control_pin, fan_tach_pin);

    while (1)
    {
        // Aktualisiere `isFanEnabled` aus der Queue, wenn vorhanden
        uint8_t tempEnabledFlag;
        if (xQueueReceive(fan_queue, &tempEnabledFlag, pdMS_TO_TICKS(100)))
        {
            isFanEnabled = (tempEnabledFlag != 0);
        }

        if (isFanEnabled)
        {
            fan_on(fan_control_pin, 0);

            if (xQueuePeek(dhtDataQueue, &dhtData, pdMS_TO_TICKS(100)) == pdTRUE)
            {
                if (dhtData.temperature >= initial_config.temp_threshold)
                {
                    int temp_diff = dhtData.temperature - initial_config.temp_threshold;

                    int fan_speed = temp_diff * 10;
                    fan_speed = (fan_speed > 255) ? 255 : (fan_speed < 0 ? 0 : fan_speed);

                    fan_set_speed(fan_speed);

                    printf("Lüftergeschwindigkeit: %d (Temperatur: %.2f°C)\n", fan_speed, dhtData.temperature);
                }
            }
        }
        else
        {
            fan_off(fan_control_pin);
            printf("Lüfter deaktiviert durch Konfiguration.\n");
        }
        vTaskDelay(pdMS_TO_TICKS(3000));
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
    moistureDataQueue = xQueueCreate(QUEUE_LENGTH, sizeof(float)); // Feuchtigkeitswerte als float
    if (moistureDataQueue == NULL)
    {
        ESP_LOGE("Task_Common", "ADC Queue creation failed!");
    }

    dhtDataQueue = xQueueCreate(QUEUE_LENGTH, sizeof(dht_data_t)); // DHT Daten
    if (dhtDataQueue == NULL)
    {
        ESP_LOGE("Task_Common", "DHT Queue creation failed!");
    }

    // Für LightState Struktur
    lightDataQueue = xQueueCreate(QUEUE_LENGTH, sizeof(LightState)); // Queue für Lichtwerte
    if (lightDataQueue == NULL)
    {
        ESP_LOGE("Task_Common", "Light Sensor Queue creation failed!");
    }

    led_queue = xQueueCreate(QUEUE_LENGTH, sizeof(led_color_t)); // Queue für LED Farben
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
}