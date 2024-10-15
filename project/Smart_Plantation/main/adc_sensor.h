#ifndef ADC_SENSOR_H
#define ADC_SENSOR_H

#include <stdio.h>
#include "esp_err.h"
#include "driver/adc.h"
#include "esp_adc/adc_oneshot.h"

// ADC-Kanal-Definition
#define ADC_CH_4 ADC1_CHANNEL_4 // GPIO 32 auf dem ESP

// Funktion zur Kalibrierung und Initialisierung des ADC
void adc_init(adc_channel_t channel, adc_bits_width_t bit_width);

// Funktion zum Lesen des ADC-Werts und Umwandlung in Prozent
float adc_read_sensor(adc_channel_t channel);

// Funktion zur Umrechnung des ADC-Werts in Prozent
float adcToPercentage(uint32_t adcValue);

// Funktion zur Bereinigung des ADC (optional)
void adc_cleanup();

#endif // ADC_SENSOR_H
