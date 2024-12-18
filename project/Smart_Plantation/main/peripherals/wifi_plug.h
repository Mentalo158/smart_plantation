#ifndef WIFI_PLUG_H
#define WIFI_PLUG_H

#include <esp_err.h>

#define TASMOTA_PORT 80 ///< Port number for the Tasmota device

/**
 * @brief Structure to hold the pump schedule.
 *
 * This structure stores the times for the pump to operate, including the hours and minutes
 * for each of the 7 days in a week, along with a field to represent the days that are enabled.
 */
typedef struct
{
    uint8_t hours[7];   ///< Array to store the hours for each day (0-23)
    uint8_t minutes[7]; ///< Array to store the minutes for each day (0-59)
    uint8_t days;       ///< Bitmask to represent days of the week (0 - Sunday, 6 - Saturday)
} pump_times;

/**
 * @brief Toggles the power of a Tasmota-controlled device (e.g., a smart plug).
 *
 * This function sends HTTP requests to turn the Tasmota device on for a specified duration
 * and then off. The duration for which the device is on is provided in milliseconds.
 *
 * @param duration_ms The duration in milliseconds for which the device should remain on.
 * @return
 *    - ESP_OK: On and off operations completed successfully.
 *    - Other: Error occurred during communication with the Tasmota device.
 */
esp_err_t tasmota_toggle_power(uint32_t duration_ms);

#endif // WIFI_PLUG_H
