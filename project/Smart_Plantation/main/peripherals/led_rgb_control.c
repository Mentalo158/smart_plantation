#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/ledc.h"
#include "esp_err.h"
#include "led_rgb_control.h"

void rgb_led_init(int pinRed, int pinGreen, int pinBlue)
{
    // Timer-Konfiguration
    ledc_timer_config_t ledc_timer = {
        .speed_mode = LEDC_MODE,
        .timer_num = LEDC_TIMER,
        .duty_resolution = LEDC_DUTY_RES, 
        .freq_hz = LEDC_FREQUENCY,        
        .clk_cfg = LEDC_AUTO_CLK          
    };
    ledc_timer_config(&ledc_timer);

    // Rot
    ledc_channel_config_t ledc_channel_r = {
        .speed_mode = LEDC_MODE,
        .channel = LEDC_CHANNEL_R,
        .timer_sel = LEDC_TIMER,
        .intr_type = LEDC_INTR_DISABLE,
        .gpio_num = pinRed, 
        .duty = 0,          
        .hpoint = 0};
    ledc_channel_config(&ledc_channel_r);

    // Grün
    ledc_channel_config_t ledc_channel_g = {
        .speed_mode = LEDC_MODE,
        .channel = LEDC_CHANNEL_G,
        .timer_sel = LEDC_TIMER,
        .intr_type = LEDC_INTR_DISABLE,
        .gpio_num = pinGreen, 
        .duty = 0,
        .hpoint = 0};
    ledc_channel_config(&ledc_channel_g);

    // Blau
    ledc_channel_config_t ledc_channel_b = {
        .speed_mode = LEDC_MODE,
        .channel = LEDC_CHANNEL_B,
        .timer_sel = LEDC_TIMER,
        .intr_type = LEDC_INTR_DISABLE,
        .gpio_num = pinBlue, 
        .duty = 0,
        .hpoint = 0};
    ledc_channel_config(&ledc_channel_b);
}

void rgb_set_color(uint8_t red, uint8_t green, uint8_t blue)
{
    ledc_set_duty(LEDC_MODE, LEDC_CHANNEL_R, red);
    ledc_update_duty(LEDC_MODE, LEDC_CHANNEL_R);

    ledc_set_duty(LEDC_MODE, LEDC_CHANNEL_G, green);
    ledc_update_duty(LEDC_MODE, LEDC_CHANNEL_G);

    ledc_set_duty(LEDC_MODE, LEDC_CHANNEL_B, blue);
    ledc_update_duty(LEDC_MODE, LEDC_CHANNEL_B);
}