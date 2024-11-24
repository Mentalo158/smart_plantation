#ifndef LIGHT_SENSOR_H
#define LIGHT_SENSOR_H

#include <stdint.h>
#include <esp_err.h>
#include <driver/gpio.h>

// Struktur zur Darstellung des Lichtstatus
typedef struct
{
    uint16_t lux_value;    // Aktueller Lux-Wert
    float light_intensity; // Lichtintensität in Prozent
} LightState;

// Initialisiert den BH1750-Sensor
// Parameter: SCL-Pin und SDA-Pin des I2C-Busses
esp_err_t bh1750_init(gpio_num_t scl_pin, gpio_num_t sda_pin);

// Liest den aktuellen Lichtstatus (Lux-Wert und Lichtintensität in Prozent)
LightState get_light_state();

#endif // LIGHT_SENSOR_H
