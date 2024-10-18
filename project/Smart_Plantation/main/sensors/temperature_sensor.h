#ifndef TEMPERATURE_SENSOR_H
#define TEMPERATURE_SENSOR_H

#include <stdint.h>
#include "esp_log.h"

// DHT-Sensor Typen
#define DHT_TYPE_DHT11_SENSOR 11
#define DHT_TYPE_DHT22_SENSOR 22

typedef struct
{
    float humidity;    ///< Luftfeuchtigkeit in Prozent
    float temperature; ///< Temperatur in Grad Celsius
} dht_data_t;

/**
 * @brief Liest die Temperatur und Feuchtigkeit vom DHT-Sensor.
 *
 * Diese Funktion kommuniziert mit dem angegebenen DHT-Sensor, um
 * die Luftfeuchtigkeit und Temperatur zu erfassen.
 *
 * @param dhtGpio Der GPIO-Pin, an dem der DHT-Sensor angeschlossen ist.
 *
 * @return dht_data_t Struktur mit den gelesenen Werten.
 */
dht_data_t read_temperature_sensor(int dhtGpio);

#endif // TEMPERATURE_SENSOR_H
