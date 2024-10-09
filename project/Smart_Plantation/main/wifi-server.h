#ifndef WIFI_WEB_H
#define WIFI_WEB_H

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"
#include "esp_event.h"
#include "esp_http_server.h"
#include "soil_sensor.h"

void wifi_connection();
void web_server_task(void *pvParameter);

#endif  // WIFI_WEB_H