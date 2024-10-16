#ifndef TASK_COMMON_H
#define TASK_COMMON_H

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#define MOISTURE_ATTEN ADC_ATTEN_DB_2_5 // Beispiel für ADC Attenuation

// Deklaration der Queues für ADC- und DHT-Daten
extern QueueHandle_t adcDataQueue; // Queue für ADC-Daten
extern QueueHandle_t dhtDataQueue; // Queue für DHT-Daten

// Funktion zur Initialisierung der Queues
void init_queue();

// Funktionsprototypen für die Tasks
void adcTask(void *pvParameters);
void dhtTask(void *pvParameters);
void webServerTask(void *pvParameters);

#endif // TASK_COMMON_H
