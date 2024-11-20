#include "light_sensor.h"
#include "esp_log.h"
#include "driver/i2c.h"

#define BH1750_ADDR BH1750_I2C_ADDRESS_DEFAULT // Standardadresse des BH1750 Sensors
static const char *TAG = "LightSensor";

static bh1750_handle_t sensor = NULL;

// Maximale Lux-Werte (Sonnenlicht)
#define MAX_LUX_VALUE 50000

// Funktion zur Initialisierung des BH1750 Sensors mit Pins als Parameter
esp_err_t bh1750_init(gpio_num_t scl_pin, gpio_num_t sda_pin)
{
    // I2C-Konfiguration
    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = sda_pin,
        .scl_io_num = scl_pin,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = 100000 // I2C Frequenz
    };

    // I2C initialisieren
    esp_err_t err = i2c_param_config(I2C_MASTER_NUM, &conf);
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "I2C-Konfiguration fehlgeschlagen");
        return err;
    }

    // I2C-Treiber installieren
    err = i2c_driver_install(I2C_MASTER_NUM, I2C_MODE_MASTER, 0, 0, 0);
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "I2C-Treiberinstallation fehlgeschlagen");
        return err;
    }

    // BH1750-Sensor erstellen
    sensor = bh1750_create(I2C_MASTER_NUM, BH1750_ADDR);
    if (sensor == NULL)
    {
        ESP_LOGE(TAG, "BH1750-Sensorerstellung fehlgeschlagen");
        return ESP_FAIL;
    }

    // Messmodus setzen
    err = bh1750_set_measure_mode(sensor, BH1750_CONTINUE_1LX_RES);
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "Fehler beim Setzen des Messmodus");
        return err;
    }

    return ESP_OK;
}

// Funktion zum Abrufen des aktuellen Lichtstatus
LightState get_light_state()
{
    LightState state;
    float lux_value = 0.0;

    // Lux-Wert vom BH1750 Sensor lesen
    esp_err_t err = bh1750_get_data(sensor, &lux_value);
    if (err == ESP_OK)
    {
        state.lux_value = (uint16_t)lux_value;

        // Berechnung der Lichtintensität in Prozent
        if (lux_value > MAX_LUX_VALUE)
        {
            state.light_intensity = 100.0f;
        }
        else
        {
            state.light_intensity = (lux_value / MAX_LUX_VALUE) * 100.0f;
        }

        ESP_LOGI(TAG, "Lux-Wert: %.2f, Lichtintensität: %.2f%%", lux_value, state.light_intensity);
    }
    else
    {
        ESP_LOGE(TAG, "Fehler beim Lesen des BH1750 Sensors!");
        state.lux_value = 0;
        state.light_intensity = 0.0f;
    }

    return state;
}

// Funktion zum Freigeben des BH1750-Sensors
void bh1750_deinit()
{
    if (sensor != NULL)
    {
        bh1750_delete(sensor);
        sensor = NULL;
        i2c_driver_delete(I2C_MASTER_NUM);
        ESP_LOGI(TAG, "BH1750-Sensor deaktiviert");
    }
}
