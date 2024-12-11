#ifndef HTTP_SERVER_H
#define HTTP_SERVER_H

#include "esp_err.h"
#include "esp_http_server.h"
#include "freertos/queue.h"


esp_err_t http_handler(httpd_req_t *req);

esp_err_t css_handler(httpd_req_t *req);

esp_err_t js_handler(httpd_req_t *req);

esp_err_t logo_handler(httpd_req_t *req);

esp_err_t moisture_value_handler(httpd_req_t *req);

esp_err_t temperature_value_handler(httpd_req_t *req);

esp_err_t light_sensor_value_handler(httpd_req_t *req);

esp_err_t config_get_handler(httpd_req_t *req);

esp_err_t config_set_handler(httpd_req_t *req);

esp_err_t wifi_plug_switch_handler(httpd_req_t *req);

httpd_handle_t start_webserver(void);

#endif 
