#ifndef CONFIG_STORAGE_H
#define CONFIG_STORAGE_H

#define MAX_TIMES 7

typedef struct
{
    uint8_t moisture_threshold; // Bodenfeuchtigkeitsschwellenwert als Integer
    uint8_t moisture_enabled;   // 0 = false, 1 = true

    uint8_t temp_threshold; // Temperaturgrenze als Integer
    uint8_t temp_enabled;   // 0 = false, 1 = true

    uint32_t luminance;                       // Lux-Wert (Luminanz)
    uint8_t light_intensity;                  // Lichtintensität (0-100%)
    uint8_t use_luminance_or_light_intensity; // 0 = false, 1 = true
    uint8_t use_dynamic_lightning;            // 0 = false, 1 = true

    uint8_t hours[MAX_TIMES];   // Stundenwerte für bis zu 7 Zeiten
    uint8_t minutes[MAX_TIMES]; // Minutenwerte für bis zu 7 Zeiten
    uint8_t days;               // Bitmaske für Wochentage

    uint8_t red;   // LED R-Wert
    uint8_t green; // LED G-Wert
    uint8_t blue;  // LED B-Wert
} config_t;

void save_config(config_t *config);
void load_config(config_t *config);

#endif // CONFIG_STORAGE_H
