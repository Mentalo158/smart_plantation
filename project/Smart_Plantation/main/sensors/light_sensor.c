#include "light_sensor.h"
#include "esp_log.h"
#include "driver/i2c.h"

#define I2C_MASTER_NUM 0 // I2C Bus Nummer (ändern je nach Bedarf)
#define BH1750_ADDR 0x23 // Adresse des BH1750 Sensors (kann auch 0x5C sein)

static const char *TAG = "LightSensor";

// I2C Konfiguration für den Sensor
#define I2C_MASTER_SCL_IO 22      // I2C SCL Pin
#define I2C_MASTER_SDA_IO 21      // I2C SDA Pin
#define I2C_MASTER_FREQ_HZ 100000 // I2C Frequenz

static bh1750_t sensor;

// Maximale Lux-Werte (Sonnenlicht)
#define MAX_LUX_VALUE 50000

// Funktion zur Initialisierung des BH1750 Sensors
esp_err_t bh1750_init()
{
    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = I2C_MASTER_SDA_IO,
        .scl_io_num = I2C_MASTER_SCL_IO,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = I2C_MASTER_FREQ_HZ};

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

    // BH1750-Sensor initialisieren
    err = bh1750_init_sensor(&sensor, I2C_MASTER_NUM, BH1750_ADDR);
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "Sensorinitialisierung fehlgeschlagen");
        return err;
    }

    return ESP_OK;
}

// Funktion zum Abrufen des aktuellen Lichtstatus
LightState get_light_state()
{
    LightState state;
    uint16_t lux_value = 0;

    // Lux-Wert vom BH1750 Sensor lesen
    if (bh1750_read_lux(&sensor, &lux_value) == ESP_OK)
    {
        state.lux_value = lux_value;

        // Berechnung der Lichtintensität in Prozent relativ zu einem maximalen Lux-Wert (50.000 Lux)
        if (lux_value > MAX_LUX_VALUE)
        {
            // Wenn der Lux-Wert höher als der Maximalwert ist, auf 100 % setzen
            state.light_intensity = 100.0f;
        }
        else
        {
            // Berechnung der Intensität in Prozent
            state.light_intensity = (float)lux_value / (float)MAX_LUX_VALUE * 100.0f;
        }

        ESP_LOGI(TAG, "Lux-Wert: %d, Lichtintensität: %.2f%%", lux_value, state.light_intensity);
    }
    else
    {
        ESP_LOGE(TAG, "Fehler beim Lesen des BH1750 Sensors!");
        state.lux_value = -1;
        state.light_intensity = -1.0f;
    }

    return state;
}
