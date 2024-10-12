#include <stdio.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_adc/adc_oneshot.h"
#include "esp_adc/adc_cali.h"
#include "driver/adc.h"
#include "esp_adc_cal.h"

adc_cali_handle_t adc1_cali_handle;
adc_oneshot_unit_handle_t adc1_handle;

float adcToPercentage(uint32_t adcValue)
{
    float maxBitWidth = 4095.0f;              // Maximale Bitbreite für 12-Bit ADC
    return (adcValue / maxBitWidth) * 100.0f; // Umrechnung in Prozent
}

void adc_init(adc_channel_t channel, adc_bits_width_t bitwidth)
{
    // Kalibrierung
    adc_cali_line_fitting_config_t cali_config = {
        .atten = ADC_ATTEN_DB_11,
        .bitwidth = bitwidth, // Bitbreite wird als Parameter übergeben
    };
    adc_cali_create_scheme_line_fitting(&cali_config, &adc1_cali_handle);

    // Initialisierung des Oneshot ADC
    adc_oneshot_unit_init_cfg_t init_config = {.unit_id = ADC_UNIT_1};
    adc_oneshot_new_unit(&init_config, &adc1_handle);

    // Konfiguration für den übergebenen ADC-Kanal
    adc_oneshot_chan_cfg_t channel_config = {.atten = ADC_ATTEN_DB_11};
    adc_oneshot_config_channel(adc1_handle, channel, &channel_config);
}

float adc_read_sensor(adc_channel_t channel)
{
    uint32_t adcValue;
    adc_oneshot_read(adc1_handle, channel, &adcValue);
    // printf("Sensor %d - ADC Value: %ld, Percentage: %.2f%%\n", channel, adcValue, adcToPercentage(adcValue));
    return adcToPercentage(adcValue);
}

// ADC ausschalten
void adc_cleanup()
{
    adc_cali_delete_scheme_line_fitting(adc1_cali_handle);
    adc_oneshot_del_unit(adc1_handle);
}
