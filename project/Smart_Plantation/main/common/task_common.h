#ifndef TASK_COMMON_H
#define TASK_COMMON_H

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#define MOISTURE_ATTEN ADC_ATTEN_DB_2_5 // Dämpfung für den Bodenfeuchtesensor bis 1,9V
#define LIGHT_ATTEN ADC_ATTEN_DB_12     // Dämpfung für den Lichtsensor bis 3,9V

#define MOISTURE_CHANNEL ADC1_CHANNEL_4 // GPIO 32
#define LIGHT_CHANNEL ADC1_CHANNEL_5    // GPIO 33

#define LEDC_OUTPUT_R_PIN 27 // GPIO für die rote LED
#define LEDC_OUTPUT_G_PIN 26 // GPIO für die grüne LED
#define LEDC_OUTPUT_B_PIN 25 // GPIO für die blaue LED

#define DHT_GPIO_PIN 4

#define MOISTURE_ADC_MAX_VALUE 4095.0f
#define LIGHT_ADC_MAX_VALUE 4095.0f

// Deklaration der Queues für ADC- und DHT-Daten
extern QueueHandle_t moistureDataQueue; // Queue für ADC-Daten
extern QueueHandle_t dhtDataQueue;      // Queue für DHT-Daten
extern QueueHandle_t lightDataQueue;    // Queue für Lichtdaten
extern QueueHandle_t led_queue;         // Queue für LED-Daten
extern QueueHandle_t pump_queue;
/**
 * @brief Initialisiert die Queues.
 *
 * Diese Funktion erstellt die Queues zur Kommunikation
 * zwischen den Tasks. Es gibt eine Queue für die ADC-Daten
 * und separate Queues für die DHT- und Lichtdaten sowie die LED-Daten.
 */
void init_queue();

/**
 * @brief ADC Sensor Task.
 *
 * Diese Funktion liest kontinuierlich ADC-Werte von einem
 * analogen Sensor (MOISTURE_CHANNEL) und schreibt diese Werte in
 * eine Queue. Es wird jede Sekunde ein neuer Wert gelesen.
 *
 * @param pvParameters Pointer zu den übergebenen Parametern (nicht verwendet).
 */
void moisture_task(void *pvParameters);

/**
 * @brief DHT Sensor Task.
 *
 * Diese Funktion liest kontinuierlich Temperatur- und Feuchtigkeitswerte
 * von einem DHT-Sensor und schreibt diese Werte in eine Queue.
 *
 * @param pvParameters Pointer zu den übergebenen Parametern (nicht verwendet).
 */
void dhtTask(void *pvParameters);

/**
 * @brief Lichtsensor Task.
 *
 * Diese Funktion liest kontinuierlich ADC-Werte von einem
 * Lichtsensor (LIGHT_CHANNEL) und schreibt diese Werte in
 * eine Queue. Es wird jede Sekunde ein neuer Wert gelesen.
 *
 * @param pvParameters Pointer zu den übergebenen Parametern (nicht verwendet).
 */
void light_sensor_task(void *pvParameters);

/**
 * @brief LED Task.
 *
 * Diese Funktion wartet auf neue RGB-Daten in der LED-Warteschlange
 * und aktualisiert die LED-Farbe entsprechend.
 *
 * @param pvParameters Pointer zu den übergebenen Parametern (nicht verwendet).
 */
void led_task(void *pvParameters);

void pump_control_task(void *pvParameter);

void fan_control_task(void *pvParameters);

void time_sync_task(void *pvParameter);
/**
 * @brief Webserver und mDNS Task.
 *
 * Diese Funktion startet einen Webserver und einen
 * mDNS-Dienst, um das Gerät im Netzwerk auffindbar zu machen.
 * Der Task bleibt in einer Endlosschleife, um die
 * Funktionalität des Servers aufrechtzuerhalten.
 *
 * @param pvParameters Pointer zu den übergebenen Parametern (nicht verwendet).
 */
void webServerTask(void *pvParameters);

#endif // TASK_COMMON_H
