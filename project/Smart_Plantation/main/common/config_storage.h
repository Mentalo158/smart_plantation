#ifndef CONFIG_STORAGE_H
#define CONFIG_STORAGE_H

/**
 * @file config_storage.h
 * @brief Header file for saving and loading configuration data in non-volatile storage (NVS).
 */

#define MAX_TIMES 7 ///< Maximum number of time entries.

/**
 * @brief Configuration structure for storing system settings.
 */
typedef struct
{
    uint8_t moisture_threshold; ///< Threshold for soil moisture percentage (0-100%).
    uint8_t moisture_enabled;   ///< Flag to enable or disable soil moisture monitoring (0 = disabled, 1 = enabled).

    uint8_t temp_threshold; ///< Threshold for temperature in degrees Celsius.
    uint8_t temp_enabled;   ///< Flag to enable or disable temperature monitoring (0 = disabled, 1 = enabled).

    uint32_t luminance;                       ///< Target luminance level in lux.
    uint8_t light_intensity;                  ///< Intensity of light in percentage (0-100%).
    uint8_t use_luminance_or_light_intensity; ///< Flag to switch between luminance or light intensity control (0 = intensity, 1 = luminance).
    uint8_t use_dynamic_lightning;            ///< Flag to enable or disable dynamic lighting (0 = disabled, 1 = enabled).

    uint8_t hours[MAX_TIMES];   ///< Array of hours for scheduled actions (0-23).
    uint8_t minutes[MAX_TIMES]; ///< Array of minutes for scheduled actions (0-59).
    uint8_t days;               ///< Bitmask representing the selected days (e.g., 0x01 = Monday, 0x02 = Tuesday, etc.).

    uint8_t red;   ///< Red component of the RGB LED color (0-255).
    uint8_t green; ///< Green component of the RGB LED color (0-255).
    uint8_t blue;  ///< Blue component of the RGB LED color (0-255).
} config_t;

/**
 * @brief Saves the configuration data to non-volatile storage (NVS).
 *
 * @param config Pointer to the configuration structure to be saved.
 */
void save_config(config_t *config);

/**
 * @brief Loads the configuration data from non-volatile storage (NVS).
 *
 * If no stored configuration is found, default values are set in the provided structure.
 *
 * @param config Pointer to the configuration structure to be loaded.
 */
void load_config(config_t *config);

#endif // CONFIG_STORAGE_H
