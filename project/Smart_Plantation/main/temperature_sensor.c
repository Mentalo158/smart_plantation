#include "temperature_sensor.h"
#include "dht.h" // Stelle sicher, dass dieser Header für die DHT-Bibliothek vorhanden ist

/**
 * @brief Liest die Temperatur und Feuchtigkeit vom DHT-Sensor.
 *
 * @return dht_data_t Struktur mit den gelesenen Werten.
 */
dht_data_t temperature_sensor(void)
{
    dht_data_t dhtData;

    // Lese die Daten vom DHT-Sensor
    if (dht_read_float_data(DHT_TYPE_DHT11, DHT_GPIO_PIN, &dhtData.humidity, &dhtData.temperature) == ESP_OK)
    {
        ESP_LOGI("DHT Sensor", "Humidity: %.1f%%, Temperature: %.1f°C", dhtData.humidity, dhtData.temperature);
    }
    else
    {
        ESP_LOGE("DHT Sensor", "Fehler beim Lesen der Daten vom DHT-Sensor!");
    }

    return dhtData;
}
