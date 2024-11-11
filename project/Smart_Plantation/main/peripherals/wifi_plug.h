#ifndef WIFI_PLUG_H
#define WIFI_PLUG_H

#include <esp_err.h>

// Steckdosen IP-Adresse und Port definieren
#define TASMOTA_IP "192.168.2.218"
#define TASMOTA_PORT 80

typedef struct
{
    uint8_t hour;
    uint8_t minute;
    uint8_t days;
} config_data_t;

// Funktion zum Ein- und Ausschalten der Steckdose für eine bestimmte Dauer
esp_err_t tasmota_toggle_power(uint32_t duration_ms);

#endif // WIFI_PLUG_H
