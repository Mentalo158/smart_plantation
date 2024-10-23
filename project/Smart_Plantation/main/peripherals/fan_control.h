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
 * @brief Initialisiert die Lüftersteuerung.
 *
 * @param pinFan Der GPIO-Pin, der mit dem PWM-Eingang des Lüfters verbunden ist.
 */
void fan_init(int pinFan);

/**
 * @brief Setzt die Lüftergeschwindigkeit.
 *
 * @param speed Lüftergeschwindigkeit (0 bis 255).
 */
void fan_set_speed(uint8_t speed);

#endif // FAN_CONTROL_H
