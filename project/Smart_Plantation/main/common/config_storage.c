#include <stdio.h>
#include <string.h>
#include "nvs_flash.h"
#include "nvs.h"
#include "config_storage.h"

#define CONFIG_NAMESPACE "storage" // NVS-Speicherbereich für Konfigurationsdaten
#define CONFIG_KEY "config_data"      // Schlüssel zum Speichern der Konfigurationsdaten

void save_config(config_t *config)
{
    nvs_handle_t nvs_handle;
    esp_err_t err = nvs_open(CONFIG_NAMESPACE, NVS_READWRITE, &nvs_handle);

    if (err != ESP_OK)
    {
        printf("Error (%s) opening NVS handle!\n", esp_err_to_name(err));
        return;
    }

    err = nvs_set_blob(nvs_handle, CONFIG_KEY, config, sizeof(config_t));
    if (err != ESP_OK)
    {
        printf("Error (%s) saving config!\n", esp_err_to_name(err));
    }
    else
    {
        err = nvs_commit(nvs_handle);
        if (err != ESP_OK)
        {
            printf("Error (%s) committing config!\n", esp_err_to_name(err));
        }
    }

    nvs_close(nvs_handle);
}

void load_config(config_t *config)
{
    nvs_handle_t nvs_handle;
    esp_err_t err = nvs_open(CONFIG_NAMESPACE, NVS_READONLY, &nvs_handle);

    if (err != ESP_OK)
    {
        printf("Error (%s) opening NVS handle!\n", esp_err_to_name(err));
        // Standardwerte setzen
        config->temp_threshold = 25;
        config->red = 255;
        config->green = 255;
        config->blue = 255;
        config->hour = 0;
        config->minute = 0;
        config->days = 0;
        return;
    }

    size_t required_size = sizeof(config_t);
    err = nvs_get_blob(nvs_handle, CONFIG_KEY, config, &required_size);

    if (err == ESP_OK)
    {
        printf("Config geladen: Temp = %ld, RGB = (%d, %d, %d), Zeit = %02d:%02d, Tage = 0x%02X\n",
               config->temp_threshold, config->red, config->green, config->blue,
               config->hour, config->minute, config->days);
    }
    else
    {
        printf("Keine gespeicherte Config gefunden, Standardwerte verwenden\n");
        config->temp_threshold = 25;
        config->red = 255;
        config->green = 255;
        config->blue = 255;
        config->hour = 0;
        config->minute = 0;
        config->days = 0;
    }

    nvs_close(nvs_handle);
}
