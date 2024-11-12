#ifndef LED_RGB_CONTROL_H
#define LED_RGB_CONTROL_H

#include <stdint.h>
#include "driver/ledc.h"

// LEDC Konfiguration
#define LEDC_TIMER LEDC_TIMER_0
#define LEDC_MODE LEDC_LOW_SPEED_MODE
#define LEDC_CHANNEL_R LEDC_CHANNEL_0
#define LEDC_CHANNEL_G LEDC_CHANNEL_1
#define LEDC_CHANNEL_B LEDC_CHANNEL_2
#define LEDC_DUTY_RES LEDC_TIMER_8_BIT 
#define LEDC_DUTY_MAX 255              
#define LEDC_FREQUENCY 5000            

typedef struct
{
    uint8_t red;
    uint8_t green;
    uint8_t blue;
} led_color_t;

/**
 * @brief Initialisiert die RGB-LED.
 *
 * Diese Funktion konfiguriert den LEDC-Timer und die Kanäle für die RGB-LED.
 */
void rgb_led_init(int pinRed, int pinGreen, int pinBlue);

/**
 * @brief Setzt die Farbe der RGB-LED.
 *
 * @param red Wert zwischen 0 und 255 für die rote Farbe.
 * @param green Wert zwischen 0 und 255 für die grüne Farbe.
 * @param blue Wert zwischen 0 und 255 für die blaue Farbe.
 */
void rgb_set_color(uint8_t red, uint8_t green, uint8_t blue);

#endif // LED_RGH_H
