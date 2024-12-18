#ifndef SNTP_CLIENT_H
#define SNTP_CLIENT_H

#include <time.h>
#include "esp_err.h"

/**
 * @file sntp_client.h
 * @brief Header file for SNTP client functionalities, including time synchronization and time zone configuration.
 */

/**
 * @brief Initializes the SNTP service.
 *
 * This function starts the SNTP client and sets the NTP server to "pool.ntp.org".
 * If the SNTP service is already running, it stops the service before reinitializing.
 */
void sntp_inits();

/**
 * @brief Updates the current time using the SNTP service.
 *
 * Retrieves the updated time from the SNTP server and logs the updated time.
 */
void sntp_update_time();

/**
 * @brief Retrieves the current time as a UNIX timestamp.
 *
 * @return The current time in seconds since the Epoch (January 1, 1970).
 */
time_t get_current_time(void);

/**
 * @brief Sets the time zone to Central European Time (CET).
 *
 * Configures the time zone environment variable for CET and adjusts for daylight saving time (CEST).
 */
void set_time_zone();

/**
 * @brief Gets the current day of the week.
 *
 * This function calculates the current day of the week (0 = Sunday, 1 = Monday, ..., 6 = Saturday)
 * based on the locally synchronized time.
 *
 * @return The current day of the week as an integer (0-6).
 */
int get_current_day_of_week();

#endif // SNTP_CLIENT_H
