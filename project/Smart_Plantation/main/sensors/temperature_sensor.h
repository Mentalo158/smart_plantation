#ifndef TEMPERATURE_SENSOR_H
#define TEMPERATURE_SENSOR_H

#include <stdint.h>
#include "esp_log.h"
#include "esp_err.h"

// DHT-Sensor Typen
#define DHT_TYPE_DHT11_SENSOR 11
#define DHT_TYPE_DHT22_SENSOR 22

typedef struct
{
    float humidity;    ///< Luftfeuchtigkeit in Prozent
    float temperature; ///< Temperatur in Grad Celsius
} dht_data_t;

esp_err_t read_temperature_sensor(int dhtGpio, dht_data_t *dhtData);

#endif // TEMPERATURE_SENSOR_H
