#ifndef FAN_CONTROL_H
#define FAN_CONTROL_H

#include <stdint.h>
#include "driver/ledc.h"

// LEDC Konfiguration für den Lüfter
#define LEDC_CHANNEL_FAN LEDC_CHANNEL_3
#define LEDC_FAN_DUTY_RES LEDC_TIMER_8_BIT // 8 Bit Auflösung für den Lüfter
#define LEDC_FAN_DUTY_MAX 255              // Maximaler Duty-Cycle (100%)
#define LEDC_FAN_FREQUENCY 25000           // 25 kHz für den Lüfter

/**
 * @brief Initialisiert die Lüftersteuerung und den Tachometer.
 *
 * @param pinFan Der GPIO-Pin, der mit dem PWM-Eingang des Lüfters verbunden ist.
 * @param pinTach Der GPIO-Pin, der das Tachometer-Signal empfängt.
 */
void fan_init(int pinFan, int pinTach);

/**
 * @brief Setzt die Lüftergeschwindigkeit.
 *
 * @param speed Lüftergeschwindigkeit (0 bis 255).
 */
void fan_set_speed(uint8_t speed);

/**
 * @brief Berechnet die Drehzahl des Lüfters basierend auf den Impulsen des Tachometers.
 *
 * @return Die Drehzahl in Umdrehungen pro Minute (RPM).
 */
uint32_t fan_get_speed_rpm(void);

#endif // FAN_CONTROL_H
