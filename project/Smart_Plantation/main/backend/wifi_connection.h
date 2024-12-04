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

void wifi_event_handler(void *event_handler_arg, esp_event_base_t event_base, int32_t event_id, void *event_data);
void wifi_connection();

#endif // WIFI_CONNECTION_H
