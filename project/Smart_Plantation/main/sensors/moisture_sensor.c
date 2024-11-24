#include "Adafruit_Stemma_soil_sensor.h"
#include "esp_log.h"
#include "moisture_sensor.h"

#define TAG "Soil_Sensor"

// Einmalige I²C-Initialisierung
esp_err_t init_soil_sensor_i2c(gpio_num_t scl_pin, gpio_num_t sda_pin)
{
    // I2C-Initialisierung
    esp_err_t ret = adafruit_stemma_soil_sensor_init(I2C_MASTER_NUM, sda_pin, scl_pin);
    if (ret == ESP_OK)
    {
        ESP_LOGI(TAG, "Adafruit Stemma Soil Sensor erfolgreich initialisiert.");
    }
    else
    {
        ESP_LOGE(TAG, "Fehler bei der Initialisierung des Adafruit Stemma Soil Sensors.");
    }

    return ret;
}

// Feuchtigkeitswert in Prozent lesen
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

// Temperaturwert lesen
float readTemperatureValue()
{
    float temperature_value = 0.0;

    // Temperaturwert vom Sensor lesen
    esp_err_t ret = adafruit_stemma_soil_sensor_read_temperature(I2C_MASTER_NUM, &temperature_value);

    if (ret == ESP_OK)
    {
        ESP_LOGI(TAG, "Temperaturwert: %.2f°C", temperature_value);
        return temperature_value;
    }
    else
    {
        ESP_LOGE(TAG, "Fehler beim Lesen des Temperaturwerts.");
        return -1.0f; // Fehlerwert zurückgeben
    }
}
