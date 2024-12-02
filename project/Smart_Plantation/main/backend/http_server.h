#ifndef HTTP_SERVER_H
#define HTTP_SERVER_H

#include "esp_err.h"
#include "esp_http_server.h"
#include "freertos/queue.h"

/**
 * @brief Handler für HTML-Anfragen.
 *
 * Diese Funktion verarbeitet HTTP-Anfragen für die HTML-Seite.
 *
 * @param req Pointer auf die HTTP-Anfrage.
 * @return ESP_OK bei Erfolg.
 */
esp_err_t http_handler(httpd_req_t *req);

/**
 * @brief Handler für CSS-Anfragen.
 *
 * Diese Funktion verarbeitet HTTP-Anfragen für die CSS-Datei.
 *
 * @param req Pointer auf die HTTP-Anfrage.
 * @return ESP_OK bei Erfolg.
 */
esp_err_t css_handler(httpd_req_t *req);

/**
 * @brief Handler für JavaScript-Anfragen.
 *
 * Diese Funktion verarbeitet HTTP-Anfragen für die JavaScript-Datei.
 *
 * @param req Pointer auf die HTTP-Anfrage.
 * @return ESP_OK bei Erfolg.
 */
esp_err_t js_handler(httpd_req_t *req);

/**
 * @brief Handler für HTMX-JavaScript-Anfragen.
 *
 * Diese Funktion verarbeitet HTTP-Anfragen für die HTMX-Bibliothek.
 *
 * @param req Pointer auf die HTTP-Anfrage.
 * @return ESP_OK bei Erfolg.
 */
esp_err_t htmx_handler(httpd_req_t *req);

/**
 * @brief Handler für ADC-Wert-Anfragen (Bodenfeuchte).
 *
 * Diese Funktion verarbeitet HTTP-Anfragen und gibt den letzten
 * ADC-Wert als Text zurück.
 *
 * @param req Pointer auf die HTTP-Anfrage.
 * @return ESP_OK bei Erfolg.
 */
esp_err_t moisture_value_handler(httpd_req_t *req);

/**
 * @brief Handler für Temperatur- und Feuchtigkeitsanfragen.
 *
 * Diese Funktion verarbeitet HTTP-Anfragen und gibt die Temperatur
 * und Feuchtigkeit als Text zurück.
 *
 * @param req Pointer auf die HTTP-Anfrage.
 * @return ESP_OK bei Erfolg.
 */
esp_err_t temperature_value_handler(httpd_req_t *req);

/**
 * @brief Handler für Lichtsensorwert-Anfragen.
 *
 * Diese Funktion verarbeitet HTTP-Anfragen und gibt den Lichtwert
 * als Text zurück.
 *
 * @param req Pointer auf die HTTP-Anfrage.
 * @return ESP_OK bei Erfolg.
 */
esp_err_t light_sensor_value_handler(httpd_req_t *req);

/**
 * @brief Startet den Webserver.
 *
 * Diese Funktion initialisiert und startet den HTTP-Server
 * und registriert die URI-Handler für HTML, CSS, JS und Favicon.
 *
 * @return httpd_handle_t Handle des gestarteten HTTP-Servers oder NULL bei Fehler.
 */
httpd_handle_t start_webserver(void);

#endif // HTTP_SERVER_H
