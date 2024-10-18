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


float adcToPercentage(float adcValue, float bitWidth)
{
    return (adcValue / bitWidth) * 100.0f;    // Umrechnung in Prozent
}

void adc_init(adc_channel_t channel, adc_bits_width_t bitwidth, adc_atten_t atten)
{
    // Kalibrierung konfigurieren
    adc_cali_line_fitting_config_t cali_config = {
        .atten = atten, // Dämpfung einstellen
        .bitwidth = bitwidth,     // Bitbreite wird als Parameter übergeben
    };

    // Kalibrierungsschema erstellen
    adc_cali_create_scheme_line_fitting(&cali_config, &adc1_cali_handle);

    // Initialisierung des Oneshot ADC
    adc_oneshot_unit_init_cfg_t init_config = {.unit_id = ADC_UNIT_1};
    adc_oneshot_new_unit(&init_config, &adc1_handle);

    // Konfiguration für den übergebenen ADC-Kanal
    adc_oneshot_chan_cfg_t channel_config = {.atten = atten};
    adc_oneshot_config_channel(adc1_handle, channel, &channel_config);
}

float adc_read_sensor(adc_channel_t channel, float bidWidth)
{
    uint32_t adcValue;
    
    adc_oneshot_read(adc1_handle, channel, &adcValue);
    return adcToPercentage((float)adcValue, bidWidth); // Umrechnung in Prozent
}

void adc_cleanup()
{
    adc_cali_delete_scheme_line_fitting(adc1_cali_handle); // Kalibrierung löschen
    adc_oneshot_del_unit(adc1_handle);                     // ADC-Einheit löschen
}
