#include "Adafruit_Stemma_soil_sensor.h"
#include "esp_log.h"
#include "moisture_sensor.h"

#define TAG "Soil_Sensor"

// Einmalige I²C-Initialisierung
esp_err_t init_soil_sensor_i2c(gpio_num_t scl_pin, gpio_num_t sda_pin)
{
    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = sda_pin,
        .scl_io_num = scl_pin,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = 100000 // I2C Frequenz
    };

    // I²C konfigurieren
    esp_err_t ret = i2c_param_config(I2C_MASTER_NUM, &conf);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "I2C-Konfiguration fehlgeschlagen.");
        return ret;
    }

    // I²C-Treiber installieren
    ret = i2c_driver_install(I2C_MASTER_NUM, I2C_MODE_MASTER, 0, 0, 0);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "I2C-Treiberinstallation fehlgeschlagen.");
        return ret;
    }

    ESP_LOGI(TAG, "I²C-Schnittstelle erfolgreich initialisiert.");
    return ESP_OK;
}

float readMoistureValueInPercent()
{
    uint16_t moisture_value = 0;

    // Feuchtigkeitswert vom Sensor lesen
    esp_err_t ret = adafruit_stemma_soil_sensor_read_moisture(I2C_MASTER_NUM, &moisture_value);

    if (ret == ESP_OK)
    {
        // Der Sensor gibt Werte zwischen 200 (trocken) und 2000 (nass)
        float moisture_percentage = ((float)(moisture_value - 200) / (2000 - 200)) * 100.0;

        ESP_LOGI(TAG, "Feuchtigkeitswert: %.2f%% (Rohwert: %u)", moisture_percentage, moisture_value);

        return moisture_percentage; // Feuchtigkeitswert in Prozent zurückgeben
    }
    else
    {
        ESP_LOGE(TAG, "Fehler beim Lesen des Feuchtigkeitswerts.");
        return -1.0f; // Fehlerwert zurückgeben
    }
}
