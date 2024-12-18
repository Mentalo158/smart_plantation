#ifndef FAN_CONTROL_H
#define FAN_CONTROL_H

#include <stdint.h>
#include "driver/ledc.h"

#define LEDC_CHANNEL_FAN LEDC_CHANNEL_3    /*!< LEDC Channel for Fan control */
#define LEDC_FAN_DUTY_RES LEDC_TIMER_8_BIT /*!< Duty resolution for the fan (8-bit) */
#define LEDC_FAN_DUTY_MAX 255              /*!< Maximum duty cycle for fan (255) */
#define LEDC_FAN_FREQUENCY 25000           /*!< PWM frequency for fan (25kHz) */
#define FAN_LEDC_MODE LEDC_HIGH_SPEED_MODE /*!< LEDC mode for fan (high speed) */
#define LEDC_TIMER LEDC_TIMER_0            /*!< LEDC Timer used for fan */
#define LEDC_DUTY_RES LEDC_TIMER_8_BIT     /*!< LEDC duty resolution (8-bit) */

/**
 * @brief Initializes the fan control and tachometer.
 *
 * This function initializes the fan control, sets up PWM for speed control,
 * and optionally configures the tachometer signal input.
 *
 * @param pinFan The GPIO pin connected to the fan's PWM input.
 * @param pinControl The GPIO pin controlling the transistor (optional).
 * @param pinTach The GPIO pin receiving the tachometer signal (optional).
 */
void fan_init(int pinFan, int pinControl, int pinTach);

/**
 * @brief Sets the fan speed.
 *
 * This function sets the PWM duty cycle, which determines the fan speed.
 * The speed can range from 0 (off) to 255 (maximum speed).
 *
 * @param speed The fan speed (0 to 255).
 */
void fan_set_speed(uint8_t speed);

/**
 * @brief Calculates the fan speed in RPM based on tachometer pulses.
 *
 * This function calculates the fan's rotations per minute (RPM) by counting
 * the tachometer pulses. The number of pulses per revolution should be provided.
 *
 * @param pulses_per_revolution The number of pulses generated per fan revolution.
 *
 * @return The fan speed in rotations per minute (RPM).
 */
uint32_t fan_get_speed_rpm(uint32_t pulses_per_revolution);

/**
 * @brief Turns the fan on and sets the speed.
 *
 * This function turns on the fan and sets its speed by controlling the
 * transistor and adjusting the PWM duty cycle.
 *
 * @param pinControl The GPIO pin controlling the transistor.
 * @param speed The desired fan speed (0 to 255).
 */
void fan_on(int pinControl, uint8_t speed);

/**
 * @brief Turns off the fan completely.
 *
 * This function turns off the fan by deactivating the transistor.
 *
 * @param pinControl The GPIO pin controlling the transistor.
 */
void fan_off(int pinControl);

#endif // FAN_CONTROL_H
