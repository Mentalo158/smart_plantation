#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/ledc.h"
#include "esp_err.h"
#include "fan_control.h"
#include "driver/gpio.h"

static volatile uint32_t pulse_count = 0; // Zähler für die Impulse des Tach-Pins

// Interrupt-Service-Routine (ISR) für den Tachometer
static void IRAM_ATTR tach_interrupt_handler(void *arg)
{
    pulse_count++; // Zähle die Impulse
}

// Initialisierung des Lüfters und des Tach-Pins
void fan_init(int pinFan, int tachPin)
{
    // Lüfter-Initialisierung (PWM)
    ledc_timer_config_t ledc_timer = {
        .speed_mode = LEDC_LOW_SPEED_MODE, // Verwende High-Speed Mode
        .timer_num = LEDC_TIMER_1,
        .duty_resolution = LEDC_FAN_DUTY_RES,
        .freq_hz = LEDC_FAN_FREQUENCY,
        .clk_cfg = LEDC_AUTO_CLK};
    esp_err_t ret = ledc_timer_config(&ledc_timer);
    if (ret != ESP_OK)
    {
        printf("Error in ledc_timer_config: %d\n", ret);
    }

    ledc_channel_config_t ledc_channel_fan = {
        .speed_mode = LEDC_LOW_SPEED_MODE, // Verwende High-Speed Mode
        .channel = LEDC_CHANNEL_FAN,
        .timer_sel = LEDC_TIMER_1,
        .intr_type = LEDC_INTR_DISABLE,
        .gpio_num = pinFan,
        .duty = 0, // Anfangsduty = 0
        .hpoint = 0};
    ret = ledc_channel_config(&ledc_channel_fan);
    if (ret != ESP_OK)
    {
        printf("Error in ledc_channel_config: %d\n", ret);
    }

    // Tachometer-Pin initialisieren
    gpio_set_direction(tachPin, GPIO_MODE_INPUT);
    gpio_set_intr_type(tachPin, GPIO_INTR_POSEDGE);              // Erzeuge Interrupt bei steigender Flanke
    gpio_install_isr_service(ESP_INTR_FLAG_LEVEL1);              // Installiere Interrupt-Service
    gpio_isr_handler_add(tachPin, tach_interrupt_handler, NULL); // Füge ISR hinzu
}

// Setzt die Lüftergeschwindigkeit
void fan_set_speed(uint8_t speed)
{
    printf("Setze Lüftergeschwindigkeit auf: %d\n", speed);                       // Debug-Ausgabe für den PWM-Wert
    esp_err_t ret = ledc_set_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_FAN, speed); // Verwende High-Speed Mode
    if (ret != ESP_OK)
    {
        printf("Error in ledc_set_duty: %d\n", ret);
    }

    ret = ledc_update_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_FAN); // Duty Cycle aktualisieren
    if (ret != ESP_OK)
    {
        printf("Error in ledc_update_duty: %d\n", ret);
    }
}

// Funktion zum Messen der Drehzahl des Lüfters basierend auf den Impulsen des Tachometers
uint32_t fan_get_speed_rpm(void)
{
    static uint32_t last_pulse_count = 0;
    uint32_t rpm = 0;

    // Berechne die Anzahl der Impulse in einer Sekunde
    uint32_t pulse_delta = pulse_count - last_pulse_count;
    last_pulse_count = pulse_count;

    if (pulse_delta > 0)
    {
        // Berechne die RPM (Umdrehungen pro Minute)
        rpm = (pulse_delta * 60); // RPM = Impulse * 60
    }

    return rpm;
}
