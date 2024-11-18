#ifndef CONFIG_STORAGE_H
#define CONFIG_STORAGE_H

#define MAX_TIMES 7 // Maximale Anzahl an Uhrzeiten

typedef struct
{
    uint32_t temp_threshold;
    uint8_t red;
    uint8_t green;
    uint8_t blue;
    uint8_t hours[MAX_TIMES];   // Stundenwerte für bis zu 7 Zeiten
    uint8_t minutes[MAX_TIMES]; // Minutenwerte für bis zu 7 Zeiten
    uint8_t days;               // Bitmaske für Wochentage
} config_t;

void save_config(config_t *config);
void load_config(config_t *config);

#endif // CONFIG_STORAGE_H
