#ifndef ADC_READ_H
#define ADC_READ_H

#include <stdio.h>
#include "esp_err.h"
#include "driver/adc.h"
#include "esp_adc/adc_oneshot.h"

/**
 * @brief Initializes the ADC for a specified channel, bit width, and attenuation.
 *
 * This function sets up the calibration, initializes the ADC unit, and configures the specified ADC channel.
 *
 * @param channel The ADC channel to be initialized.
 * @param bit_width The bit width for ADC resolution (e.g., ADC_WIDTH_BIT_12).
 * @param atten The input voltage attenuation level for the ADC.
 */
void adc_init(adc_channel_t channel, adc_bits_width_t bit_width, adc_atten_t atten);

/**
 * @brief Reads the ADC value of a specified sensor and converts it to a percentage.
 *
 * This function reads the raw ADC value from the sensor and converts it into a percentage
 * based on the specified bit width.
 *
 * @param channel The ADC channel to read from.
 * @param bitWidth The bit width used for ADC calibration.
 * @return The converted ADC value as a percentage (0-100%).
 */
float adc_read_sensor(adc_channel_t channel, float bidWidth);

/**
 * @brief Converts the ADC value to a percentage based on the specified bit width.
 *
 * @param adcValue The raw ADC value read from the sensor.
 * @param bitWidth The bit width used for ADC calibration.
 * @return The percentage representation of the ADC value.
 */
float adcToPercentage(float adcValue, float bitWidth);

/**
 * @brief Cleans up and releases the ADC resources.
 *
 * This function deletes the calibration scheme and cleans up the ADC unit.
 */
void adc_cleanup();

#endif // ADC_READ_H
