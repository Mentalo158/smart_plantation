#include "Adafruit_Stemma_soil_sensor.h"
#include "esp_log.h"
#include "moisture_sensor.h"

#define TAG "Soil_Sensor"

float readMoistureValueInPercent()
{
    int ret = ESP_OK;
    uint16_t moisture_value = 0;

    ret = adafruit_stemma_soil_sensor_read_moisture(I2C_MASTER_NUM, &moisture_value);

    if (ret == ESP_OK)
    {
        // The sensor give a value of 200 for very dry and 2000 for very wet
        float moisture_percentage = ((float)(moisture_value - 200) / (2000 - 200)) * 100.0;

        ESP_LOGI(TAG, "Moisture: %.2f%% (raw value: %u)", moisture_percentage, moisture_value);

        return (uint16_t)moisture_percentage;
    }
    else
    {
        ESP_LOGE(TAG, "Failed to read moisture value.");
        return -1.0f; 
    }
}
