#ifndef TEMPERATURE_SENSOR_H
#define TEMPERATURE_SENSOR_H

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include <dht.h>

// Tag für das Logging
static const char *TAG = "DHT_READER";

// Definiere den DHT-Sensortyp und den GPIO-Pin
#define DHT_SENSOR_TYPE DHT_TYPE_DHT11 // Sensor Typ: DHT11 oder DHT22
#define DHT_GPIO_PIN GPIO_NUM_4        // GPIO-Pin für den DHT-Sensor

/**
 * @brief Liest die Temperatur und Feuchtigkeit vom DHT-Sensor und gibt die Werte aus.
 *
 * Diese Funktion wird als Task verwendet, um kontinuierlich Daten vom DHT-Sensor zu lesen
 * und auf der Konsole anzuzeigen. Die Task wiederholt den Vorgang in regelmäßigen Abständen.
 *
 * @param pvParameter Ein optionaler Parameter für die FreeRTOS-Task-Funktion (nicht verwendet).
 */
void temperature_sensor(void *pvParameter);

#endif // TEMPERATURE_SENSOR_H
