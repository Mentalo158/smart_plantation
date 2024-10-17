#include <stdio.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_adc/adc_oneshot.h"
#include "esp_adc/adc_cali.h"
#include "driver/adc.h"
#include "esp_adc_cal.h"

// Handle für die ADC-Kalibrierung
adc_cali_handle_t adc1_cali_handle;
// Handle für den Oneshot-ADC
adc_oneshot_unit_handle_t adc1_handle;

/**
 * @brief Wandelt einen ADC-Wert in einen Prozentwert um.
 *
 * Diese Funktion rechnet den Rohwert des ADC in einen Prozentsatz um.
 *
 * @param adcValue Der ADC-Rohwert, der umgerechnet werden soll.
 * @return float Der umgerechnete Wert in Prozent.
 */
float adcToPercentage(uint32_t adcValue)
{
    float maxBitWidth = 4095.0f;              // Maximale Bitbreite für 12-Bit ADC
    return (adcValue / maxBitWidth) * 100.0f; // Umrechnung in Prozent
}

/**
 * @brief Initialisiert den ADC.
 *
 * Diese Funktion initialisiert den ADC mit der angegebenen Kanal- und Bitbreite
 * und konfiguriert die Kalibrierung.
 *
 * @param channel Der ADC-Kanal, der initialisiert werden soll.
 * @param bitwidth Die Bitbreite für den ADC (z. B. ADC_WIDTH_BIT_12).
 */
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

/**
 * @brief Liest den Wert des angegebenen ADC-Kanals.
 *
 * Diese Funktion liest den aktuellen ADC-Wert und gibt ihn als Prozentsatz zurück.
 *
 * @param channel Der ADC-Kanal, von dem gelesen werden soll.
 * @return float Der aktuelle ADC-Wert in Prozent.
 */
float adc_read_sensor(adc_channel_t channel)
{
    uint32_t adcValue;
    // ADC-Wert lesen
    adc_oneshot_read(adc1_handle, channel, &adcValue);
    return adcToPercentage((float)adcValue); // Umrechnung in Prozent
}

/**
 * @brief Bereinigt die ADC-Ressourcen.
 *
 * Diese Funktion löscht die Kalibrierungsschemata und gibt die Ressourcen
 * des ADC frei.
 */
void adc_cleanup()
{
    adc_cali_delete_scheme_line_fitting(adc1_cali_handle); // Kalibrierung löschen
    adc_oneshot_del_unit(adc1_handle);                     // ADC-Einheit löschen
}
