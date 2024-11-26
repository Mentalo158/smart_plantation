#include <stdio.h>
#include <string.h>
#include "nvs_flash.h"
#include "nvs.h"
#include "config_storage.h"

#define CONFIG_NAMESPACE "storage" // NVS-Speicherbereich für Konfigurationsdaten
#define CONFIG_KEY "config_data"   // Schlüssel zum Speichern der Konfigurationsdaten

void save_config(config_t *config)
{
    nvs_handle_t nvs_handle;
    esp_err_t err = nvs_open(CONFIG_NAMESPACE, NVS_READWRITE, &nvs_handle);

    if (err != ESP_OK)
    {
        printf("Error (%s) opening NVS handle!\n", esp_err_to_name(err));
        return;
    }

    // Speichern der Konfiguration als Blob (gesamte Struktur auf einmal)
    err = nvs_set_blob(nvs_handle, CONFIG_KEY, config, sizeof(config_t));
    if (err != ESP_OK)
    {
        printf("Error (%s) saving config blob!\n", esp_err_to_name(err));
    }
    else
    {
        // Finales Commit der Daten
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
    esp_err_t err = nvs_open(CONFIG_NAMESPACE, NVS_READWRITE, &nvs_handle);

    if (err != ESP_OK)
    {
        printf("Error (%s) opening NVS handle!\n", esp_err_to_name(err));

        // Setze Standardwerte, wenn keine Konfiguration geladen werden kann
        config->moisture_threshold = 50;
        config->moisture_enabled = 0;
        config->temp_threshold = 25;
        config->temp_enabled = 0;
        config->luminance = 500;
        config->light_intensity = 50;
        config->use_luminance_or_light_intensity = 1;
        config->use_dynamic_lightning = 0;

        for (int i = 0; i < MAX_TIMES; i++)
        {
            config->hours[i] = 0;
            config->minutes[i] = 0;
        }
        config->days = 0;
        config->red = 255;
        config->green = 255;
        config->blue = 255;

        // Speichern der Standardwerte als Blob
        err = nvs_set_blob(nvs_handle, CONFIG_KEY, config, sizeof(config_t));
        if (err != ESP_OK)
        {
            printf("Fehler beim Speichern der Standardkonfiguration in NVS: %s\n", esp_err_to_name(err));
        }
        else
        {
            nvs_commit(nvs_handle);
        }
        nvs_close(nvs_handle);
        return;
    }

    // Größe des Blobs ermitteln
    size_t required_size = sizeof(config_t);
    err = nvs_get_blob(nvs_handle, CONFIG_KEY, config, &required_size);

    if (err == ESP_OK)
    {
        // Konfiguration erfolgreich geladen
        printf("Config geladen:\n");
        printf("Feuchtigkeitsschwelle: %d%%\n", config->moisture_threshold);
        printf("Feuchtigkeit aktiviert: %s\n", config->moisture_enabled ? "true" : "false");
        printf("Temperaturschwelle: %d°C\n", config->temp_threshold);
        printf("Temperatur aktiviert: %s\n", config->temp_enabled ? "true" : "false");
        printf("Luminanz: %ld Lux\n", config->luminance);
        printf("Lichtintensität: %d%%\n", config->light_intensity);
        printf("Luminanz oder Lichtintensität verwenden: %s\n", config->use_luminance_or_light_intensity ? "true" : "false");
        printf("Dynamische Beleuchtung verwenden: %s\n", config->use_dynamic_lightning ? "true" : "false");

        // Ausgabe der Zeiten
        for (int i = 0; i < MAX_TIMES; i++)
        {
            printf("Zeit %d: %02d:%02d\n", i + 1, config->hours[i], config->minutes[i]);
        }

        // Ausgabe der Wochentage (Bitmaske)
        printf("Wochentage: 0x%02X\n", config->days);

        // RGB-Werte der LED
        printf("LED Farbe - Rot: %d, Grün: %d, Blau: %d\n", config->red, config->green, config->blue);
    }
    else
    {
        printf("Keine gespeicherte Config gefunden, Standardwerte verwenden\n");
        // Im Fehlerfall Standardwerte setzen
        config->moisture_threshold = 50;
        config->moisture_enabled = 0;
        config->temp_threshold = 25;
        config->temp_enabled = 0;
        config->luminance = 500;
        config->light_intensity = 50;
        config->use_luminance_or_light_intensity = 1;
        config->use_dynamic_lightning = 0;
        for (int i = 0; i < MAX_TIMES; i++)
        {
            config->hours[i] = 0;
            config->minutes[i] = 0;
        }
        config->days = 0;
        config->red = 255;
        config->green = 255;
        config->blue = 255;
    }

    nvs_close(nvs_handle);
}
