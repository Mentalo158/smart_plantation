#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/ledc.h"
#include "esp_err.h"
#include "driver/gpio.h"
#include "fan_control.h"

static volatile uint32_t pulse_count = 0; // Zähler für die Impulse des Tach-Pins

// Interrupt-Service-Routine (ISR) für den Tachometer
static void IRAM_ATTR tach_interrupt_handler(void *arg)
{
    pulse_count++; // Zähle die Impulse
}

// Initialisierung des Lüfters und Steuer-Pins (wie bei einem LED-Strip)
void fan_init(int pinFan, int pinControl, int pinTach)
{
    // Timer-Konfiguration (wie bei einem LED-Strip)
    ledc_timer_config_t ledc_timer = {
        .speed_mode = LEDC_MODE,              // High-Speed Mode verwenden
        .timer_num = LEDC_TIMER,              // Timer 0 verwenden
        .duty_resolution = LEDC_FAN_DUTY_RES, // 8-Bit Auflösung
        .freq_hz = LEDC_FAN_FREQUENCY,        // 25 kHz Frequenz
        .clk_cfg = LEDC_AUTO_CLK};            // Auto-Taktquelle verwenden
    esp_err_t ret = ledc_timer_config(&ledc_timer);
    if (ret != ESP_OK)
    {
        printf("Error in ledc_timer_config: %d\n", ret);
    }

    // PWM-Kanal für den Lüfter konfigurieren
    ledc_channel_config_t ledc_channel_fan = {
        .speed_mode = LEDC_MODE,        // High-Speed Mode verwenden
        .channel = LEDC_CHANNEL_FAN,    // Kanal 3 verwenden
        .timer_sel = LEDC_TIMER,        // Timer 0 verwenden
        .intr_type = LEDC_INTR_DISABLE, // Kein Interrupt
        .gpio_num = pinFan,             // Pin für den PWM-Ausgang
        .duty = 0,                      // Anfangsduty = 0
        .hpoint = 0};
    ret = ledc_channel_config(&ledc_channel_fan);
    if (ret != ESP_OK)
    {
        printf("Error in ledc_channel_config: %d\n", ret);
    }

    // GPIO für Transistor-Steuerung initialisieren
    if (pinControl >= 0)
    {
        gpio_set_direction(pinControl, GPIO_MODE_OUTPUT);
        gpio_set_level(pinControl, 0); // Standardmäßig aus
    }

    // Tachometer-Pin initialisieren
    if (pinTach >= 0)
    {
        gpio_set_direction(pinTach, GPIO_MODE_INPUT);
        gpio_set_intr_type(pinTach, GPIO_INTR_POSEDGE);              // Interrupt bei steigender Flanke
        gpio_install_isr_service(ESP_INTR_FLAG_LEVEL1);              // Interrupt-Service installieren
        gpio_isr_handler_add(pinTach, tach_interrupt_handler, NULL); // ISR registrieren
    }
}

// Lüfter einschalten (ähnlich wie Farben setzen im LED-Strip)
void fan_on(int pinControl, uint8_t speed)
{
    if (pinControl >= 0)
    {
        gpio_set_level(pinControl, 1); // Transistor für Stromversorgung einschalten
    }

    // Duty für PWM setzen
    ledc_set_duty(LEDC_MODE, LEDC_CHANNEL_FAN, speed);
    ledc_update_duty(LEDC_MODE, LEDC_CHANNEL_FAN);
}

// Lüfter ausschalten (ähnlich wie Farben auf 0 setzen im LED-Strip)
void fan_off(int pinControl)
{
    if (pinControl >= 0)
    {
        gpio_set_level(pinControl, 0); // Transistor für Stromversorgung ausschalten
    }

    // Duty für PWM auf 0 setzen
    ledc_set_duty(LEDC_MODE, LEDC_CHANNEL_FAN, 0);
    ledc_update_duty(LEDC_MODE, LEDC_CHANNEL_FAN);
}

// Funktion zum Einstellen der Geschwindigkeit (wie bei Farbwerten im LED-Strip)
void fan_set_speed(uint8_t speed)
{
    ledc_set_duty(LEDC_MODE, LEDC_CHANNEL_FAN, speed);
    ledc_update_duty(LEDC_MODE, LEDC_CHANNEL_FAN);
}

// Funktion zur RPM-Berechnung
uint32_t fan_get_speed_rpm(uint32_t pulses_per_revolution)
{
    static uint32_t last_pulse_count = 0;
    uint32_t rpm = 0;

    // Differenz der Impulse seit der letzten Messung
    uint32_t pulse_delta = pulse_count - last_pulse_count;
    last_pulse_count = pulse_count;

    if (pulse_delta > 0)
    {
        // RPM = Impulse pro Sekunde * 60 / Impulse pro Umdrehung
        rpm = (pulse_delta * 60) / pulses_per_revolution;
    }

    return rpm;
}
