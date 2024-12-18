#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/ledc.h"
#include "esp_err.h"
#include "driver/gpio.h"
#include "fan_control.h"

static volatile uint32_t pulse_count = 0; 


static void IRAM_ATTR tach_interrupt_handler(void *arg)
{
    pulse_count++; 
}


void fan_init(int pinFan, int pinControl, int pinTach)
{
    
    ledc_timer_config_t ledc_timer = {
        .speed_mode = FAN_LEDC_MODE,          
        .timer_num = LEDC_TIMER,              
        .duty_resolution = LEDC_FAN_DUTY_RES, 
        .freq_hz = LEDC_FAN_FREQUENCY,        
        .clk_cfg = LEDC_AUTO_CLK};            
    esp_err_t ret = ledc_timer_config(&ledc_timer);
    if (ret != ESP_OK)
    {
        printf("Error in ledc_timer_config: %d\n", ret);
    }

    
    ledc_channel_config_t ledc_channel_fan = {
        .speed_mode = FAN_LEDC_MODE,    
        .channel = LEDC_CHANNEL_FAN,    
        .timer_sel = LEDC_TIMER,        
        .intr_type = LEDC_INTR_DISABLE, 
        .gpio_num = pinFan,             
        .duty = 0,                      
        .hpoint = 0};
    ret = ledc_channel_config(&ledc_channel_fan);
    if (ret != ESP_OK)
    {
        printf("Error in ledc_channel_config: %d\n", ret);
    }

    
    if (pinControl >= 0)
    {
        gpio_set_direction(pinControl, GPIO_MODE_OUTPUT);
        gpio_set_level(pinControl, 0); 
    }

    
    if (pinTach >= 0)
    {
        gpio_set_direction(pinTach, GPIO_MODE_INPUT);
        gpio_set_intr_type(pinTach, GPIO_INTR_POSEDGE);              
        gpio_install_isr_service(ESP_INTR_FLAG_LEVEL1);              
        gpio_isr_handler_add(pinTach, tach_interrupt_handler, NULL); 
    }
}


void fan_on(int pinControl, uint8_t speed)
{
    if (pinControl >= 0)
    {
        gpio_set_level(pinControl, 1); 
    }

    
    ledc_set_duty(FAN_LEDC_MODE, LEDC_CHANNEL_FAN, speed);
    ledc_update_duty(FAN_LEDC_MODE, LEDC_CHANNEL_FAN);
}


void fan_off(int pinControl)
{
    if (pinControl >= 0)
    {
        gpio_set_level(pinControl, 0); 
    }

    
    ledc_set_duty(FAN_LEDC_MODE, LEDC_CHANNEL_FAN, 0);
    ledc_update_duty(FAN_LEDC_MODE, LEDC_CHANNEL_FAN);
}


void fan_set_speed(uint8_t speed)
{
    ledc_set_duty(FAN_LEDC_MODE, LEDC_CHANNEL_FAN, speed);
    ledc_update_duty(FAN_LEDC_MODE, LEDC_CHANNEL_FAN);
}


uint32_t fan_get_speed_rpm(uint32_t pulses_per_revolution)
{
    static uint32_t last_pulse_count = 0;
    uint32_t rpm = 0;

    
    uint32_t pulse_delta = pulse_count - last_pulse_count;
    last_pulse_count = pulse_count;

    if (pulse_delta > 0)
    {
        
        rpm = (pulse_delta * 60) / pulses_per_revolution;
    }

    return rpm;
}
