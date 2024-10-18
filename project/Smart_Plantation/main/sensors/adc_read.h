#ifndef ADC_READ_H
#define ADC_READ_H

#include <stdio.h>
#include "esp_err.h"
#include "driver/adc.h"
#include "esp_adc/adc_oneshot.h"

/**
 * @brief Funktion zur Kalibrierung und Initialisierung des ADC.
 *
 * Diese Funktion konfiguriert die Kalibrierung und initialisiert
 * den Oneshot ADC für den angegebenen Kanal, die Bitbreite und
 * die Dämpfung.
 *
 * @param channel Der ADC-Kanal, der initialisiert werden soll.
 * @param bit_width Die Bitbreite für die ADC-Werte.
 * @param atten Die Dämpfungseinstellung für den ADC.
 */
void adc_init(adc_channel_t channel, adc_bits_width_t bit_width, adc_atten_t atten);

/**
 * @brief Funktion zum Lesen des ADC-Werts und Umwandlung in Prozent.
 *
 * Diese Funktion liest den ADC-Wert vom angegebenen Kanal und
 * wandelt ihn in einen Prozentsatz um.
 *
 * @param channel Der ADC-Kanal, von dem der Wert gelesen werden soll.
 * @param bidWidth Die maximale ADC-Wertbreite für die Umrechnung.
 *
 * @return Der ADC-Wert als Prozentsatz.
 */
float adc_read_sensor(adc_channel_t channel, float bidWidth);

/**
 * @brief Funktion zur Umrechnung des ADC-Werts in Prozent.
 *
 * Diese Funktion konvertiert den gegebenen ADC-Wert in einen
 * Prozentsatz basierend auf der übergebenen Bitbreite.
 *
 * @param adcValue Der ADC-Wert, der umgerechnet werden soll.
 * @param bitWidth Die Bitbreite, die für die Umrechnung verwendet wird.
 *
 * @return Der umgerechnete Wert in Prozent.
 */
float adcToPercentage(float adcValue, float bitWidth);

/**
 * @brief Funktion zur Bereinigung des ADC (optional).
 *
 * Diese Funktion löscht die Kalibrierung und die ADC-Einheit,
 * um Ressourcen freizugeben.
 */
void adc_cleanup();

#endif // ADC_READ_H
