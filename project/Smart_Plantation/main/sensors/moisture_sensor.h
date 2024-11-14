#ifndef MOISTURE_SENSOR_H
#define MOISTURE_SENSOR_H

#define I2C_MASTER_NUM 0
#define I2C_SDA_PIN 21
#define I2C_SCL_PIN 22
#define DELAY_TIME_BETWEEN_READINGS_MS 100

float readMoistureValueInPercent();

#endif // MOISTURE_SENSOR_H