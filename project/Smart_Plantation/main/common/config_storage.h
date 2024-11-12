#ifndef CONFIG_STORAGE_H
#define CONFIG_STORAGE_H

typedef struct
{
    uint32_t temp_threshold;
    uint8_t red;
    uint8_t green;
    uint8_t blue;
    uint8_t hour;   // Stundenwert für Zeit
    uint8_t minute; // Minutenwert für Zeit
    uint8_t days;   // Bitmaske für Wochentage
} config_t;

void config_storage_init();
void save_config(config_t *config);
void load_config(config_t *config);


#endif // CONFIG_STORAGE_H
