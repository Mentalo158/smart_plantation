#ifndef MOISTURE_SENSOR_H
#define MOISTURE_SENSOR_H

#include <stdint.h>
#include <esp_err.h>
#include <driver/gpio.h>

#define I2C_MASTER_NUM 0

// Initialisiert den Adafruit Stemma Soil Sensor über I²C
esp_err_t init_soil_sensor_i2c(gpio_num_t scl_pin, gpio_num_t sda_pin);

// Liest den Feuchtigkeitswert vom Sensor und gibt ihn in Prozent zurück
float readMoistureValueInPercent();

// Liest den Temperaturwert vom Sensor und gibt ihn in Grad Celsius zurück
float readTemperatureValue();

#endif // MOISTURE_SENSOR_H
