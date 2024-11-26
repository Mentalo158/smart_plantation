#ifndef TASK_COMMON_H
#define TASK_COMMON_H

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

// I2C Pin-Konfiguration für die Feuchtigkeits- und Lichtsensoren
#define I2C_SCL_PIN 22 // Gemeinsamer SCL Pin für beide Sensoren
#define I2C_SDA_PIN 21 // Gemeinsamer SDA Pin für beide Sensoren
#define MOISTURE_ATTEN ADC_ATTEN_DB_2_5 // Dämpfung für den Bodenfeuchtesensor bis 1,9V
#define MOISTURE_CHANNEL ADC1_CHANNEL_4 // GPIO 32

#define DHT_GPIO_PIN 4 // GPIO Pin für den DHT Sensor

#define LEDC_OUTPUT_R_PIN 27 // GPIO für die rote LED
#define LEDC_OUTPUT_G_PIN 26 // GPIO für die grüne LED
#define LEDC_OUTPUT_B_PIN 25 // GPIO für die blaue LED

#define MOISTURE_ADC_MAX_VALUE 4095.0f

// Deklaration der Queues für die Sensor-Daten
extern QueueHandle_t moistureDataQueue; // Queue für Feuchtigkeitsdaten
extern QueueHandle_t lightDataQueue;    // Queue für Lichtdaten
extern QueueHandle_t led_queue;         // Queue für LED-Daten
extern QueueHandle_t pump_queue;
extern QueueHandle_t fan_queue;

/**
 * @brief Initialisiert die Queues.
 */
void init_queue();

/**
 * @brief Feuchtigkeitssensor Task.
 */
void moisture_task(void *pvParameters);

/**
 * @brief Lichtsensor Task.
 *
 * Diese Funktion liest kontinuierlich Lichtwerte vom BH1750-Sensor
 * und schreibt diese Werte in eine Queue.
 */
void light_sensor_task(void *pvParameters);

/**
 * @brief DHT Sensor Task.
 */
void dhtTask(void *pvParameters);

/**
 * @brief LED Task.
 */
void led_task(void *pvParameters);

void pump_control_task(void *pvParameter);

void fan_control_task(void *pvParameters);

void time_sync_task(void *pvParameter);

void webServerTask(void *pvParameters);

#endif // TASK_COMMON_H
