#ifndef TASK_COMMON_H
#define TASK_COMMON_H

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#define MOISTURE_ATTEN ADC_ATTEN_DB_2_5

// Deklaration der Queue für den ADC
extern QueueHandle_t adcDataQueue;

// Funktion zur Initialisierung der Tasks
void sensorTask(void *pvParameters);
void webServerTask(void *pvParameters);
void init_queue();

#endif // TASK_COMMON_H
