#ifndef TEMPERATURE_SENSOR_H
#define TEMPERATURE_SENSOR_H

#include <stdint.h>
#include "esp_log.h"

// DHT-Sensor Typen
#define DHT_TYPE_DHT11_SENSOR 11
#define DHT_TYPE_DHT22_SENSOR 22

// Beispiel GPIO-Pin für den DHT-Sensor
#define DHT_GPIO_PIN 4

// Struktur zur Speicherung der DHT-Daten
typedef struct
{
    float humidity;    // Luftfeuchtigkeit
    float temperature; // Temperatur
} dht_data_t;

// Funktion zur Messung von Temperatur und Feuchtigkeit
dht_data_t read_temperature_sensor(void);

#endif // TEMPERATURE_SENSOR_H
