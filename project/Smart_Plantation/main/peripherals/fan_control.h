#ifndef FAN_CONTROL_H
#define FAN_CONTROL_H

#include <stdint.h>
#include "driver/ledc.h"

// LEDC Konfiguration für den Lüfter
#define LEDC_CHANNEL_FAN LEDC_CHANNEL_3    // Kanal für den Lüfter (beispielsweise Kanal 3)
#define LEDC_FAN_DUTY_RES LEDC_TIMER_8_BIT // 8 Bit Auflösung für den Lüfter
#define LEDC_FAN_DUTY_MAX 255              // Maximaler Duty-Cycle (100%)
#define LEDC_FAN_FREQUENCY 25000           // 25 kHz PWM-Frequenz für den Lüfter
#define LEDC_MODE LEDC_HIGH_SPEED_MODE     // High-Speed Mode verwenden
#define LEDC_TIMER LEDC_TIMER_0            // Verwende Timer 0
#define LEDC_DUTY_RES LEDC_TIMER_8_BIT     // Duty-Resolution auf 8-Bit setzen
#define LEDC_FREQUENCY 25000               // Frequenz von 25 kHz

/**
 * @brief Initialisiert die Lüftersteuerung und den Tachometer.
 *
 * @param pinFan Der GPIO-Pin, der mit dem PWM-Eingang des Lüfters verbunden ist.
 * @param pinControl Der GPIO-Pin, der den Transistor steuert (optional).
 * @param pinTach Der GPIO-Pin, der das Tachometer-Signal empfängt (optional).
 */
void fan_init(int pinFan, int pinControl, int pinTach);

/**
 * @brief Setzt die Lüftergeschwindigkeit.
 *
 * @param speed Lüftergeschwindigkeit (0 bis 255).
 */
void fan_set_speed(uint8_t speed);

/**
 * @brief Berechnet die Drehzahl des Lüfters basierend auf den Impulsen des Tachometers.
 *
 * @param pulses_per_revolution Die Anzahl der Impulse, die pro Umdrehung erzeugt werden.
 *
 * @return Die Drehzahl in Umdrehungen pro Minute (RPM).
 */
uint32_t fan_get_speed_rpm(uint32_t pulses_per_revolution);

/**
 * @brief Schaltet den Lüfter ein und setzt die Geschwindigkeit.
 *
 * @param pinControl Der GPIO-Pin, der den Transistor steuert.
 * @param speed Die gewünschte Lüftergeschwindigkeit (0 bis 255).
 */
void fan_on(int pinControl, uint8_t speed);

/**
 * @brief Schaltet den Lüfter vollständig aus.
 *
 * @param pinControl Der GPIO-Pin, der den Transistor steuert.
 */
void fan_off(int pinControl);

#endif // FAN_CONTROL_H
