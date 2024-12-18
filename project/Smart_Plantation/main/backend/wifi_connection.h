#ifndef WIFI_CONNECTION_H
#define WIFI_CONNECTION_H

#include <stdio.h>
#include <string.h>
#include "esp_event.h"
#include "esp_wifi.h"
#include "esp_netif.h"
#include "esp_sntp.h"
#include "sdkconfig.h"
#include "sntp_client.h"

/**
 * @file wifi_connection.h
 * @brief Header file for Wi-Fi connection setup and event handling.
 */

/**
 * @brief Event handler for Wi-Fi and IP events.
 *
 * Handles various Wi-Fi and IP events, including:
 * - Starting the Wi-Fi connection
 * - Successfully connecting to a Wi-Fi network
 * - Handling disconnections and retry logic
 * - Obtaining an IP address and initializing SNTP for time synchronization
 *
 * @param event_handler_arg User-defined argument passed to the event handler.
 * @param event_base Event base for Wi-Fi or IP events.
 * @param event_id Specific event ID.
 * @param event_data Additional data related to the event.
 */
void wifi_event_handler(void *event_handler_arg, esp_event_base_t event_base, int32_t event_id, void *event_data);

/**
 * @brief Initializes and establishes a Wi-Fi connection.
 *
 * Configures the Wi-Fi station mode, sets up the credentials from the configuration,
 * and starts the connection process. This function also registers event handlers
 * for Wi-Fi and IP-related events.
 */
void wifi_connection();

#endif // WIFI_CONNECTION_H
