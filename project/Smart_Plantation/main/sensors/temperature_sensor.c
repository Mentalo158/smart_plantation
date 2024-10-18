#include "temperature_sensor.h"
#include "dht.h"

dht_data_t read_temperature_sensor(int dhtGpio)
{
    dht_data_t dhtData;

    if (dht_read_float_data(DHT_TYPE_AM2301, dhtGpio, &dhtData.humidity, &dhtData.temperature) == ESP_OK)
    {
        ESP_LOGI("DHT Sensor", "Humidity: %.1f%%, Temperature: %.1f°C", dhtData.humidity, dhtData.temperature);
    }
    else
    {
        ESP_LOGE("DHT Sensor", "Fehler beim Lesen der Daten vom DHT-Sensor!");
    }

    return dhtData;
}
