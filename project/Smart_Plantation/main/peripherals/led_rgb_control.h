#ifndef LED_RGB_CONTROL_H
#define LED_RGB_CONTROL_H

#include <stdint.h>
#include "driver/ledc.h"

// PWM configuration for RGB LED
#define LEDC_TIMER LEDC_TIMER_0        ///< PWM Timer for RGB LED
#define LEDC_MODE LEDC_LOW_SPEED_MODE  ///< PWM Mode for RGB LED
#define LEDC_CHANNEL_R LEDC_CHANNEL_0  ///< Channel for Red LED
#define LEDC_CHANNEL_G LEDC_CHANNEL_1  ///< Channel for Green LED
#define LEDC_CHANNEL_B LEDC_CHANNEL_2  ///< Channel for Blue LED
#define LEDC_DUTY_RES LEDC_TIMER_8_BIT ///< Duty resolution (8-bit)
#define LEDC_DUTY_MAX 255              ///< Maximum duty value (full brightness)
#define LEDC_FREQUENCY 5000            ///< PWM Frequency (5 kHz)

/**
 * @brief Structure to represent RGB color.
 */
typedef struct
{
    uint8_t red;   ///< Red component (0 to 255)
    uint8_t green; ///< Green component (0 to 255)
    uint8_t blue;  ///< Blue component (0 to 255)
} led_color_t;

/**
 * @brief Initializes the RGB LED.
 *
 * This function configures the PWM for each color channel (Red, Green, Blue) using the
 * specified GPIO pins and initializes the timer and channels for PWM control.
 *
 * @param pinRed The GPIO pin for the Red LED channel.
 * @param pinGreen The GPIO pin for the Green LED channel.
 * @param pinBlue The GPIO pin for the Blue LED channel.
 */
void rgb_led_init(int pinRed, int pinGreen, int pinBlue);

/**
 * @brief Sets the RGB LED color.
 *
 * This function sets the duty cycle for each color channel (Red, Green, Blue) to
 * the specified values, adjusting the LED's color accordingly.
 *
 * @param red The duty cycle for the Red channel (0 to 255).
 * @param green The duty cycle for the Green channel (0 to 255).
 * @param blue The duty cycle for the Blue channel (0 to 255).
 */
void rgb_set_color(uint8_t red, uint8_t green, uint8_t blue);

#endif // LED_RGB_CONTROL_H
