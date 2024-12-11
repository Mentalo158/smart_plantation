#ifndef LIGHT_SENSOR_H
#define LIGHT_SENSOR_H

#include <stdint.h>
#include <esp_err.h>
#include <driver/gpio.h>


typedef struct
{
    uint16_t lux_value;    
    float light_intensity; 
} LightState;

esp_err_t bh1750_init(gpio_num_t scl_pin, gpio_num_t sda_pin);

LightState get_light_state();

#endif 
