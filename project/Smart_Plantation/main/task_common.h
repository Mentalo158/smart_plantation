#ifndef TASK_COMMON_H
#define TASK_COMMON_H

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#define MOISTURE_ATTEN ADC_ATTEN_DB_2_5 // Dämpfung für den Bodenfeuchtesensor bis 1,9V
#define LIGHT_ATTEN ADC_ATTEN_DB_12     // Dämpfung für den Lichtsensor bis 3,9V

#define ADC_CH_4 ADC1_CHANNEL_4 // GPIO 32
#define ADC_CH_5 ADC1_CHANNEL_5 // GPIO 33

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
void webServerTask(void *pvParameters);

#endif // TASK_COMMON_H
