#ifndef LIGHT_SENSOR_H
#define LIGHT_SENSOR_H

#include <stdint.h>
#include <esp_err.h>
#include <driver/gpio.h>

/**
 * @brief Structure to store light sensor state.
 *
 * This structure contains the raw lux value from the sensor and the calculated light intensity as a percentage.
 */
typedef struct
{
    uint16_t lux_value;    /*!< Raw lux value from the sensor */
    float light_intensity; /*!< Light intensity as a percentage of the maximum value */
} LightState;

/**
 * @brief Initializes the BH1750 light sensor.
 *
 * This function initializes the BH1750 sensor by configuring the I2C communication and setting the
 * sensor mode and resolution.
 *
 * @param scl_pin The GPIO pin number for the I2C clock (SCL).
 * @param sda_pin The GPIO pin number for the I2C data (SDA).
 * @return ESP_OK if the initialization was successful, otherwise an error code.
 */
esp_err_t bh1750_init(gpio_num_t scl_pin, gpio_num_t sda_pin);

/**
 * @brief Retrieves the current light sensor state.
 *
 * This function reads the lux value from the BH1750 sensor, calculates the light intensity as a
 * percentage, and returns a `LightState` structure containing the values.
 *
 * @return A `LightState` structure containing the lux value and light intensity as a percentage.
 */
LightState get_light_state();

#endif // LIGHT_SENSOR_H
