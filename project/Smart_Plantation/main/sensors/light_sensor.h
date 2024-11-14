#ifndef LIGHT_SENSOR_H
#define LIGHT_SENSOR_H

#include "esp_err.h"
#include "driver/i2c.h"
#include "BH1750.h"

// Struktur, um den aktuellen Lichtstatus zu speichern
typedef struct
{
    uint16_t lux_value;    // Lux-Wert
    float light_intensity; // Lichtintensität in Prozent
} LightState;

// Funktion zur Initialisierung des BH1750 Sensors
esp_err_t bh1750_init();

// Funktion zum Abrufen des aktuellen Lichtstatus
LightState get_light_state();

#endif // LIGHT_SENSOR_H
