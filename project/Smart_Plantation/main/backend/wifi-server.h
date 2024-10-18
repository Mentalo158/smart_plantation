#ifndef WIFI_SERVER_H
#define WIFI_SERVER_H

#include "esp_err.h"
#include "esp_http_server.h"
#include "freertos/queue.h"

/**
 * @brief WLAN-Ereignishandler.
 *
 * Diese Funktion behandelt verschiedene WLAN-Ereignisse wie
 * Verbindungsstatus und IP-Adressenvergabe.
 *
 * @param event_handler_arg Argument für den Ereignishandler (nicht verwendet).
 * @param event_base Basis des Ereignisses.
 * @param event_id ID des Ereignisses.
 * @param event_data Daten des Ereignisses.
 */
void wifi_event_handler(void *event_handler_arg, esp_event_base_t event_base, int32_t event_id, void *event_data);

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
 * @brief Handler für Favicon-Anfragen.
 *
 * Diese Funktion verarbeitet HTTP-Anfragen für das Favicon.
 *
 * @param req Pointer auf die HTTP-Anfrage.
 * @return ESP_OK bei Erfolg.
 */
esp_err_t favicon_handler(httpd_req_t *req);

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
 * @brief Stellt die Verbindung zum WLAN her.
 *
 * Diese Funktion initialisiert das WLAN, registriert die Ereignis-Handler
 * und stellt die Verbindung zum definierten WLAN her.
 */
void wifi_connection(void);

/**
 * @brief Startet den Webserver.
 *
 * Diese Funktion initialisiert und startet den HTTP-Server
 * und registriert die URI-Handler für HTML, CSS, JS und Favicon.
 *
 * @return httpd_handle_t Handle des gestarteten HTTP-Servers oder NULL bei Fehler.
 */
httpd_handle_t start_webserver(void);

#endif // WIFI_SERVER_H
