#ifndef MOISTURE_SENSOR_H
#define MOISTURE_SENSOR_H

#define I2C_MASTER_NUM 0

esp_err_t init_soil_sensor_i2c(gpio_num_t scl_pin, gpio_num_t sda_pin);

float readMoistureValueInPercent();

#endif // MOISTURE_SENSOR_H