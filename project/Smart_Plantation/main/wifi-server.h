#ifndef WIFI_WEB_H
#define WIFI_WEB_H

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"
#include "esp_event.h"
#include "esp_http_server.h"

void wifi_connection(void);
httpd_handle_t start_webserver(void);

#endif  // WIFI_WEB_H
