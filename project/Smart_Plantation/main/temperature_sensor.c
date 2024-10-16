#include "temperature_sensor.h"

/**
 * @brief Liest die Temperatur und Feuchtigkeit vom DHT-Sensor und gibt die Werte aus.
 */
void temperature_sensor(void *pvParameter)
{
    ESP_LOGI(TAG, "DHT Sensor Lesevorgang startet...");

    // Wartezeit vor dem Start (2 Sekunden)
    vTaskDelay(2000 / portTICK_PERIOD_MS);

    // Endlosschleife zum kontinuierlichen Lesen der Daten
    while (1)
    {
        float humidity = 0;
        float temperature = 0;

        // Lese die Daten vom DHT-Sensor
        if (dht_read_float_data(DHT_SENSOR_TYPE, DHT_GPIO_PIN, &humidity, &temperature) == ESP_OK)
        {
            ESP_LOGI(TAG, "Humidity: %.1f%%", humidity);
            ESP_LOGI(TAG, "Temperature: %.1f°C", temperature);
        }
        else
        {
            ESP_LOGE(TAG, "Fehler beim Lesen der Daten vom DHT-Sensor!");
        }

        // Alle 2 Sekunden erneut lesen
        vTaskDelay(2000 / portTICK_PERIOD_MS);
    }
}
