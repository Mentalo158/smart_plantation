#ifndef TASK_COMMON_H
#define TASK_COMMON_H

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#define MOISTURE_ATTEN ADC_ATTEN_DB_2_5 // Dämpfung für den Bodenfeuchtesensor bis 1,9V
#define LIGHT_ATTEN ADC_ATTEN_DB_12     // Dämpfung für den Lichtsensor bis 3,9V

#define MOISTURE_CHANNEL ADC1_CHANNEL_4 // GPIO 32
#define LIGHT_CHANNEL ADC1_CHANNEL_5 // GPIO 33

#define LEDC_OUTPUT_R_PIN 27 // GPIO für die rote LED
#define LEDC_OUTPUT_G_PIN 26 // GPIO für die grüne LED
#define LEDC_OUTPUT_B_PIN 25 // GPIO für die blaue LED

#define DHT_GPIO_PIN 4

#define MOISTURE_ADC_MAX_VALUE 4095.0f
#define LIGHT_ADC_MAX_VALUE 4095.0f

// Deklaration der Queues für ADC- und DHT-Daten
extern QueueHandle_t moistureDataQueue; // Queue für ADC-Daten
extern QueueHandle_t dhtDataQueue;      // Queue für DHT-Daten
extern QueueHandle_t lightDataQueue;

// Funktion zur Initialisierung der Queues
void init_queue();

// Funktionsprototypen für die Tasks
void moisture_task(void *pvParameters);
void dhtTask(void *pvParameters);
void light_sensor_task(void *pvParameters);
void led_task(void *pvParameters);
void webServerTask(void *pvParameters);

#endif // TASK_COMMON_H
