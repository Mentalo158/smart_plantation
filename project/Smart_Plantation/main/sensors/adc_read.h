#ifndef ADC_READ_H
#define ADC_READ_H

#include <stdio.h>
#include "esp_err.h"
#include "driver/adc.h"
#include "esp_adc/adc_oneshot.h"


void adc_init(adc_channel_t channel, adc_bits_width_t bit_width, adc_atten_t atten);


float adc_read_sensor(adc_channel_t channel, float bidWidth);


float adcToPercentage(float adcValue, float bitWidth);


void adc_cleanup();

#endif // ADC_READ_H
