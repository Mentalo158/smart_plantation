#ifndef HTTP_SERVER_H
#define HTTP_SERVER_H

#include "esp_err.h"
#include "esp_http_server.h"
#include "freertos/queue.h"

/**
 * @brief Handles HTTP GET requests for the root endpoint ("/").
 *
 * Responds with the HTML content of the main index page.
 *
 * @param req Pointer to the HTTP request object.
 * @return ESP_OK on success, or an appropriate ESP error code.
 */
esp_err_t http_handler(httpd_req_t *req);

/**
 * @brief Handles HTTP GET requests for the CSS file ("/app.css").
 *
 * Responds with the CSS content.
 *
 * @param req Pointer to the HTTP request object.
 * @return ESP_OK on success, or an appropriate ESP error code.
 */
esp_err_t css_handler(httpd_req_t *req);

/**
 * @brief Handles HTTP GET requests for the JavaScript file ("/app.js").
 *
 * Responds with the JavaScript content.
 *
 * @param req Pointer to the HTTP request object.
 * @return ESP_OK on success, or an appropriate ESP error code.
 */
esp_err_t js_handler(httpd_req_t *req);

/**
 * @brief Handles HTTP GET requests for the logo image ("/logo.png").
 *
 * Responds with the PNG image content.
 *
 * @param req Pointer to the HTTP request object.
 * @return ESP_OK on success, or an appropriate ESP error code.
 */
esp_err_t logo_handler(httpd_req_t *req);

/**
 * @brief Handles HTTP GET requests for the soil moisture value ("/adc").
 *
 * Retrieves the latest soil moisture percentage from the queue and responds with the value.
 * If no data is available, responds with a 404 error.
 *
 * @param req Pointer to the HTTP request object.
 * @return ESP_OK on success, or an appropriate ESP error code.
 */
esp_err_t moisture_value_handler(httpd_req_t *req);

/**
 * @brief Handles HTTP GET requests for temperature and humidity values ("/temperature").
 *
 * Retrieves the latest temperature and humidity data from the queue and responds with the values.
 * If no data is available, responds with a 404 error.
 *
 * @param req Pointer to the HTTP request object.
 * @return ESP_OK on success, or an appropriate ESP error code.
 */
esp_err_t temperature_value_handler(httpd_req_t *req);

/**
 * @brief Handles HTTP GET requests for light sensor data ("/light").
 *
 * Retrieves the latest light intensity and lux values from the queue and responds with the values.
 * If no data is available, responds with a 404 error.
 *
 * @param req Pointer to the HTTP request object.
 * @return ESP_OK on success, or an appropriate ESP error code.
 */
esp_err_t light_sensor_value_handler(httpd_req_t *req);

/**
 * @brief Handles HTTP GET requests for configuration data ("/config").
 *
 * Retrieves the current configuration settings and responds with a JSON object.
 *
 * @param req Pointer to the HTTP request object.
 * @return ESP_OK on success, or an appropriate ESP error code.
 */
esp_err_t config_get_handler(httpd_req_t *req);

/**
 * @brief Handles HTTP POST requests to update configuration data ("/config_set").
 *
 * Parses the incoming JSON data and updates the configuration settings.
 * Responds with a success message or an appropriate error code.
 *
 * @param req Pointer to the HTTP request object.
 * @return ESP_OK on success, or an appropriate ESP error code.
 */
esp_err_t config_set_handler(httpd_req_t *req);

/**
 * @brief Handles HTTP GET requests to toggle the WiFi plug power ("/plug_switch").
 *
 * Toggles the power of the WiFi-connected plug if the cooldown period has passed.
 * Responds with a success or cooldown message.
 *
 * @param req Pointer to the HTTP request object.
 * @return ESP_OK on success, or an appropriate ESP error code.
 */
esp_err_t wifi_plug_switch_handler(httpd_req_t *req);

/**
 * @brief Starts the HTTP web server.
 *
 * Registers all URI handlers and initializes the HTTP server.
 *
 * @return A handle to the HTTP server instance, or NULL if the server failed to start.
 */
httpd_handle_t start_webserver();

#endif // HTTP_SERVER_H
