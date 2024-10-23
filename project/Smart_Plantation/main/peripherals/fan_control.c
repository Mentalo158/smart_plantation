#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/ledc.h"
#include "esp_err.h"
#include "fan_control.h"

void fan_init(int pinFan)
{
    ledc_timer_config_t ledc_timer = {
        .speed_mode = LEDC_MODE,
        .timer_num = LEDC_TIMER_1,
        .duty_resolution = LEDC_FAN_DUTY_RES,
        .freq_hz = LEDC_FAN_FREQUENCY,
        .clk_cfg = LEDC_AUTO_CLK};
    ledc_timer_config(&ledc_timer);

    ledc_channel_config_t ledc_channel_fan = {
        .speed_mode = LEDC_MODE,
        .channel = LEDC_CHANNEL_FAN,
        .timer_sel = LEDC_TIMER_1,
        .intr_type = LEDC_INTR_DISABLE,
        .gpio_num = pinFan,
        .duty = 0,
        .hpoint = 0};
    ledc_channel_config(&ledc_channel_fan);
}

void fan_set_speed(uint8_t speed)
{
    ledc_set_duty(LEDC_MODE, LEDC_CHANNEL_FAN, speed);
    ledc_update_duty(LEDC_MODE, LEDC_CHANNEL_FAN);
}
