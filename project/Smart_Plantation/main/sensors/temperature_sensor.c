#include "temperature_sensor.h"
#include "dht.h"

esp_err_t read_temperature_sensor(int dhtGpio, dht_data_t *dhtData)
{
    if (dht_read_float_data(DHT_TYPE_AM2301, dhtGpio, &dhtData->humidity, &dhtData->temperature) == ESP_OK)
        return ESP_OK;
    return ESP_FAIL;
}
