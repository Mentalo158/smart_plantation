#ifndef TEMPERATURE_SENSOR_H
#define TEMPERATURE_SENSOR_H

#include <stdint.h>
#include "esp_log.h"
#include "esp_err.h"

/**
 * @brief Defines for supported DHT sensor types.
 */
#define DHT_TYPE_DHT11_SENSOR 11 /*!< DHT11 sensor type */
#define DHT_TYPE_DHT22_SENSOR 22 /*!< DHT22 sensor type */

/**
 * @brief Structure to store temperature and humidity data.
 *
 * This structure holds the humidity and temperature readings from the DHT sensor.
 */
typedef struct
{
    float humidity;    /*!< Humidity reading from the sensor, in percentage */
    float temperature; /*!< Temperature reading from the sensor, in Celsius */
} dht_data_t;

/**
 * @brief Reads the temperature and humidity data from a DHT sensor.
 *
 * This function reads the temperature and humidity data from a specified DHT sensor connected
 * to the given GPIO pin. The data is stored in the provided `dht_data_t` structure.
 *
 * @param dhtGpio GPIO pin number where the DHT sensor is connected.
 * @param dhtData Pointer to a `dht_data_t` structure where the sensor readings will be stored.
 * @return ESP_OK if the data is successfully read, otherwise ESP_FAIL.
 */
esp_err_t read_temperature_sensor(int dhtGpio, dht_data_t *dhtData);

#endif // TEMPERATURE_SENSOR_H
