#ifndef LIGHT_SENSOR_H
#define LIGHT_SENSOR_H

#include "esp_err.h"
#include "bh1750.h"

#define I2C_MASTER_NUM 0
// Struktur, um den aktuellen Lichtstatus zu speichern
typedef struct
{
    uint16_t lux_value;    // Lux-Wert
    float light_intensity; // Lichtintensität in Prozent
} LightState;

// Funktion zur Initialisierung des BH1750 Sensors
esp_err_t bh1750_init(gpio_num_t scl_pin, gpio_num_t sda_pin);

// Funktion zum Abrufen des aktuellen Lichtstatus
LightState get_light_state();

// Funktion zur Freigabe des BH1750 Sensors
void bh1750_deinit();

#endif // LIGHT_SENSOR_H

