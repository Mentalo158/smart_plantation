#ifndef TASK_COMMON_H
#define TASK_COMMON_H

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#define I2C_SCL_PIN CONFIG_LIGHT_SENSOR_SCL ///< I2C clock pin for the light sensor
#define I2C_SDA_PIN CONFIG_LIGHT_SENSOR_SDA ///< I2C data pin for the light sensor
#define MOISTURE_ATTEN ADC_ATTEN_DB_2_5     ///< ADC attenuation for moisture sensor
#define MOISTURE_CHANNEL ADC1_CHANNEL_4     ///< ADC channel for moisture sensor

#define DHT_GPIO_PIN CONFIG_TEMP_SENSOR_GPIO ///< GPIO pin for the DHT temperature sensor

#define LEDC_OUTPUT_R_PIN CONFIG_LED_RED_GPIO   ///< GPIO pin for the red LED
#define LEDC_OUTPUT_G_PIN CONFIG_LED_GREEN_GPIO ///< GPIO pin for the green LED
#define LEDC_OUTPUT_B_PIN CONFIG_LED_BLUE_GPIO  ///< GPIO pin for the blue LED

#define FAN_PWM_PIN CONFIG_FAN_PWM_GPIO         ///< PWM GPIO pin for fan speed control
#define FAN_TACH_PIN CONFIG_FAN_TACH_GPIO       ///< GPIO pin for fan tachometer
#define FAN_CONTROL_PIN CONFIG_FAN_CONTROL_GPIO ///< GPIO pin for controlling fan power

#define MOISTURE_ADC_MAX_VALUE 4095.0f ///< Maximum ADC value for the moisture sensor

extern QueueHandle_t moistureDataQueue; ///< Queue for Moisture data
extern QueueHandle_t lightDataQueue; ///< Queue for light sensor data
extern QueueHandle_t led_queue; ///< Queue for LED RGB values
extern QueueHandle_t pump_queue; ///< Queue for scheduling pump actions
extern QueueHandle_t fan_queue; ///< Queue for fan control state (enabled/disabled)
extern QueueHandle_t fanSpeedQueue; ///< Queue for fan speed data (RPM)
extern QueueHandle_t soil_queue; ///< Queue for adjusting soil moisture thresholds
extern QueueHandle_t moisture_enabled_queue; ///< Queue for enabling/disabling moisture sensing
extern QueueHandle_t tempThresholdQueue; ///< Queue for temperature threshold adjustments
extern QueueHandle_t dynamicLightQueue; ///< Queue for dynamic lighting control
extern QueueHandle_t luxOrIntensityQueue; ///< Queue for selecting between lux or light intensity
extern QueueHandle_t lightIntensityThresholdQueue; ///< Queue for light intensity threshold
extern QueueHandle_t lightLuxThresholdQueue; ///< Queue for lux threshold in dynamic lighting

/**
 * @brief Initializes all task-related queues.
 *
 * This function creates and initializes the queues used for communication
 * between tasks. Each queue is configured with a length and an item size
 * based on the specific data type it will hold.
 */
void init_queue();

/**
 * @brief Task for reading moisture levels from the sensor and sending to queue.
 *
 * This task reads the moisture level from the ADC channel and pushes the
 * value to the moistureDataQueue for other tasks to process.
 *
 * @param pvParameters Parameters passed to the task (not used here).
 */
void moisture_task(void *pvParameters);

/**
 * @brief Task to control pump operation based on soil moisture levels.
 *
 * This task checks the moisture data and activates the pump if the
 * moisture level falls below a predefined threshold.
 *
 * @param pvParameters Parameters passed to the task (not used here).
 */
void pump_water_based_on_soil_task(void *pvParameters);

/**
 * @brief Task to read data from the light sensor and send to queue.
 *
 * This task continuously reads data from the light sensor and sends it to
 * the lightDataQueue for further processing.
 *
 * @param pvParameters Parameters passed to the task (not used here).
 */
void light_sensor_task(void *pvParameters);

/**
 * @brief Task to read temperature and humidity from the DHT sensor.
 *
 * This task reads temperature and humidity data from the DHT sensor and
 * sends the data to the dhtDataQueue for use by other tasks.
 *
 * @param pvParameters Parameters passed to the task (not used here).
 */
void dhtTask(void *pvParameters);

/**
 * @brief Task to control the LED color based on user input.
 *
 * This task reads the color data from the led_queue and updates the LED color
 * accordingly.
 *
 * @param pvParameters Parameters passed to the task (not used here).
 */
void led_task(void *pvParameters);

/**
 * @brief Task to control pump based on a schedule.
 *
 * This task checks the configured schedule and activates the pump at the
 * specified times.
 *
 * @param pvParameter Parameters passed to the task (not used here).
 */
void pump_control_task(void *pvParameter);

/**
 * @brief Task to control the fan based on temperature readings.
 *
 * This task checks the temperature data from the DHT sensor and adjusts
 * the fan speed accordingly to maintain the desired temperature.
 *
 * @param pvParameters Parameters passed to the task (not used here).
 */
void fan_control_task(void *pvParameters);

/**
 * @brief Task for synchronizing time using SNTP.
 *
 * This task periodically updates the system time by using SNTP to ensure
 * time accuracy.
 *
 * @param pvParameter Parameters passed to the task (not used here).
 */
void time_sync_task(void *pvParameter);

/**
 * @brief Task to control dynamic lighting based on sensor readings.
 *
 * This task adjusts the LED color or state based on light intensity or lux
 * threshold values received through queues.
 *
 * @param pvParameters Parameters passed to the task (not used here).
 */
void dynamicLightTask(void *pvParameters);

/**
 * @brief Task to serve a web interface for controlling and monitoring the system.
 *
 * This task starts a web server and handles incoming HTTP requests to allow
 * the user to control and monitor various system parameters.
 *
 * @param pvParameters Parameters passed to the task (not used here).
 */
void webServerTask(void *pvParameters);

#endif // TASK_COMMON_H
