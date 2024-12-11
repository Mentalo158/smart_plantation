#include "light_sensor.h"
#include "esp_log.h"
#include "driver/i2c.h"
#include "bh1750.h"
#include "string.h"

#define BH1750_ADDR BH1750_ADDR_LO 
static const char *TAG = "LightSensor";

static i2c_dev_t dev; 


#define MAX_LUX_VALUE 50000


esp_err_t bh1750_init(gpio_num_t scl_pin, gpio_num_t sda_pin)
{
    vTaskDelay(pdMS_TO_TICKS(3000));
    
    ESP_ERROR_CHECK(i2cdev_init());

    memset(&dev, 0, sizeof(i2c_dev_t)); 

    esp_err_t err = bh1750_init_desc(&dev, BH1750_ADDR, 1, sda_pin, scl_pin);
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "Fehler bei der Initialisierung der BH1750-Beschreibung!");
        return err;
    }

    
    err = bh1750_setup(&dev, BH1750_MODE_CONTINUOUS, BH1750_RES_HIGH);
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "Fehler bei der Konfiguration des BH1750 Sensors!");
    }
    else
    {
        ESP_LOGI(TAG, "BH1750 erfolgreich initialisiert.");
    }

    return err;
}


LightState get_light_state()
{
    LightState state;
    uint16_t lux_value = 0;

    esp_err_t err = bh1750_read(&dev, &lux_value);
    if (err == ESP_OK)
    {
        state.lux_value = lux_value;

        
        if (lux_value > MAX_LUX_VALUE)
        {
            state.light_intensity = 100.0f;
        }
        else
        {
            state.light_intensity = ((float)lux_value / MAX_LUX_VALUE) * 100.0f;
        }

        ESP_LOGI(TAG, "Lux-Wert: %d, Lichtintensität: %.2f%%", lux_value, state.light_intensity);
    }
    else
    {
        ESP_LOGE(TAG, "Fehler beim Lesen des BH1750 Sensors!");
        state.lux_value = -1;
        state.light_intensity = 0.0f;
    }

    return state;
}
