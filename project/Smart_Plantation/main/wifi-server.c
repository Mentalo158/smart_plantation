#include "wifi-server.h"
#include "esp_log.h"
#include "esp_wifi.h"
#include "nvs_flash.h"
#include "sdkconfig.h"
#include "temperature_sensor.h"

// Globale Variablen für WLAN-Konfiguration
const char *ssid = CONFIG_WIFI_SSID;     ///< SSID des WLANs
const char *pass = CONFIG_WIFI_PASSWORD; ///< Passwort für das WLAN
int retry_num = 0;                       ///< Zähler für Wiederholungsversuche bei der WLAN-Verbindung

httpd_handle_t start_webserver(void);            ///< Prototyp für die Funktion zum Starten des Webservers
void stop_webserver(httpd_handle_t http_server); ///< Prototyp für die Funktion zum Stoppen des Webservers

// Eingebettete Dateien (HTML, CSS, JS, Favicon)
extern const uint8_t index_html_start[] asm("_binary_index_html_start");   ///< Start der HTML-Datei
extern const uint8_t index_html_end[] asm("_binary_index_html_end");       ///< Ende der HTML-Datei
extern const uint8_t app_css_start[] asm("_binary_app_css_start");         ///< Start der CSS-Datei
extern const uint8_t app_css_end[] asm("_binary_app_css_end");             ///< Ende der CSS-Datei
extern const uint8_t app_js_start[] asm("_binary_app_js_start");           ///< Start der JS-Datei
extern const uint8_t app_js_end[] asm("_binary_app_js_end");               ///< Ende der JS-Datei
extern const uint8_t favicon_ico_start[] asm("_binary_favicon_ico_start"); ///< Start des Favicon
extern const uint8_t favicon_ico_end[] asm("_binary_favicon_ico_end");     ///< Ende des Favicon

extern QueueHandle_t adcDataQueue;
extern QueueHandle_t dhtDataQueue;

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
static void wifi_event_handler(void *event_handler_arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
    if (event_id == WIFI_EVENT_STA_START)
    {
        printf("WIFI CONNECTING....\n");
    }
    else if (event_id == WIFI_EVENT_STA_CONNECTED)
    {
        printf("WiFi CONNECTED\n");
    }
    else if (event_id == WIFI_EVENT_STA_DISCONNECTED)
    {
        printf("WiFi lost connection\n");
        if (retry_num < 5)
        {
            esp_wifi_connect();
            retry_num++;
            printf("Retrying to Connect...\n");
        }
    }
    else if (event_id == IP_EVENT_STA_GOT_IP)
    {
        ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;
        printf("Wifi got IP...\n");
        printf("IP is: " IPSTR "\n", IP2STR(&event->ip_info.ip));
    }
}

/**
 * @brief Handler für HTML-Anfragen.
 *
 * Diese Funktion verarbeitet HTTP-Anfragen für die HTML-Seite.
 *
 * @param req Pointer auf die HTTP-Anfrage.
 * @return ESP_OK bei Erfolg.
 */
esp_err_t http_handler(httpd_req_t *req)
{
    httpd_resp_set_type(req, "text/html");
    httpd_resp_send(req, (const char *)index_html_start, index_html_end - index_html_start);
    return ESP_OK;
}

/**
 * @brief Handler für CSS-Anfragen.
 *
 * Diese Funktion verarbeitet HTTP-Anfragen für die CSS-Datei.
 *
 * @param req Pointer auf die HTTP-Anfrage.
 * @return ESP_OK bei Erfolg.
 */
esp_err_t css_handler(httpd_req_t *req)
{
    httpd_resp_set_type(req, "text/css");
    httpd_resp_send(req, (const char *)app_css_start, app_css_end - app_css_start);
    return ESP_OK;
}

/**
 * @brief Handler für JavaScript-Anfragen.
 *
 * Diese Funktion verarbeitet HTTP-Anfragen für die JavaScript-Datei.
 *
 * @param req Pointer auf die HTTP-Anfrage.
 * @return ESP_OK bei Erfolg.
 */
esp_err_t js_handler(httpd_req_t *req)
{
    httpd_resp_set_type(req, "application/javascript");
    httpd_resp_send(req, (const char *)app_js_start, app_js_end - app_js_start);
    return ESP_OK;
}

/**
 * @brief Handler für Favicon-Anfragen.
 *
 * Diese Funktion verarbeitet HTTP-Anfragen für das Favicon.
 *
 * @param req Pointer auf die HTTP-Anfrage.
 * @return ESP_OK bei Erfolg.
 */
esp_err_t favicon_handler(httpd_req_t *req)
{
    httpd_resp_set_type(req, "image/x-icon");
    httpd_resp_send(req, (const char *)favicon_ico_start, favicon_ico_end - favicon_ico_start);
    return ESP_OK;
}

/**
 * @brief Handler für ADC-Wert-Anfragen.
 *
 * Diese Funktion verarbeitet HTTP-Anfragen und gibt den letzten
 * ADC-Wert als JSON zurück.
 *
 * @param req Pointer auf die HTTP-Anfrage.
 * @return ESP_OK bei Erfolg.
 */
esp_err_t moisture_value_handler(httpd_req_t *req)
{
    float adcValue = 0.0;

    // Hole den letzten ADC-Wert ohne zu blocken
    if (xQueuePeek(adcDataQueue, &adcValue, 0) == pdTRUE)
    {
        char response[64];
        snprintf(response, sizeof(response), "{\"adc_value\": %.2f}", adcValue);
        httpd_resp_set_type(req, "application/json");
        httpd_resp_send(req, response, strlen(response));
    }
    else
    {
        httpd_resp_send_404(req); // No Content
    }
    return ESP_OK;
}

/**
 * @brief Handler für Temperatur- und Feuchtigkeitsanfragen.
 *
 * Diese Funktion verarbeitet HTTP-Anfragen und gibt die Temperatur
 * und Feuchtigkeit als JSON zurück.
 *
 * @param req Pointer auf die HTTP-Anfrage.
 * @return ESP_OK bei Erfolg.
 */
esp_err_t temperature_value_handler(httpd_req_t *req)
{
    dht_data_t dhtData; // Struktur zum Speichern der Sensorwerte

    // Hole die DHT-Daten aus der Queue ohne zu blocken
    if (xQueuePeek(dhtDataQueue, &dhtData, 0) == pdTRUE)
    {
        char response[128];
        snprintf(response, sizeof(response), "{\"temperature\": %.2f, \"humidity\": %.2f}",
                 dhtData.temperature, dhtData.humidity);
        httpd_resp_set_type(req, "application/json");
        httpd_resp_send(req, response, strlen(response));
    }
    else
    {
        // Wenn die Queue leer ist, sende einen 404-Fehler
        httpd_resp_send_404(req); // No Content
    }

    return ESP_OK;
}

// URI-Definitionen und Handler für HTML, CSS, JS und Favicon
httpd_uri_t http_uri = {
    .uri = "/",
    .method = HTTP_GET,
    .handler = http_handler,
    .user_ctx = NULL};

httpd_uri_t css_uri = {
    .uri = "/app.css",
    .method = HTTP_GET,
    .handler = css_handler,
    .user_ctx = NULL};

httpd_uri_t js_uri = {
    .uri = "/app.js", // Hier wird der URI für die JS-Datei definiert
    .method = HTTP_GET,
    .handler = js_handler,
    .user_ctx = NULL};

httpd_uri_t favicon_uri = {
    .uri = "/favicon.ico",
    .method = HTTP_GET,
    .handler = favicon_handler,
    .user_ctx = NULL};

httpd_uri_t adc_uri = {
    .uri = "/adc",
    .method = HTTP_GET,
    .handler = moisture_value_handler,
    .user_ctx = NULL};

// URI-Definition für Temperatur- und Feuchtigkeitsanfragen
httpd_uri_t temperature_uri = {
    .uri = "/temperature",
    .method = HTTP_GET,
    .handler = temperature_value_handler,
    .user_ctx = NULL};

/**
 * @brief Stellt die Verbindung zum WLAN her.
 *
 * Diese Funktion initialisiert das WLAN, registriert die Ereignis-Handler
 * und stellt die Verbindung zum definierten WLAN her.
 */
void wifi_connection()
{
    esp_netif_init();                    // Initialisiere das Netzwerkinterface
    esp_event_loop_create_default();     // Erstelle die Standard-Ereignisschleife
    esp_netif_create_default_wifi_sta(); // Erstelle ein Standard-WLAN-Station-Interface

    // WLAN-Initialisierung
    wifi_init_config_t wifi_initiation = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&wifi_initiation);

    // Registriere die Ereignis-Handler
    esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, wifi_event_handler, NULL);
    esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, wifi_event_handler, NULL);

    // WLAN-Konfiguration
    wifi_config_t wifi_configuration = {
        .sta = {
            .ssid = CONFIG_WIFI_SSID,
            .password = CONFIG_WIFI_PASSWORD,
        }};

    // SSID und Passwort setzen
    strcpy((char *)wifi_configuration.sta.ssid, ssid);
    strcpy((char *)wifi_configuration.sta.password, pass);

    // Konfiguration anwenden
    esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_configuration);
    esp_wifi_start();                 // WLAN starten
    esp_wifi_set_mode(WIFI_MODE_STA); // Setze den WLAN-Modus auf Station
    esp_wifi_connect();               // Verbinde mit dem WLAN
    printf("wifi_init_softap finished. SSID:%s  password:%s", ssid, pass);
}

/**
 * @brief Startet den Webserver.
 *
 * Diese Funktion initialisiert und startet den HTTP-Server
 * und registriert die URI-Handler für HTML, CSS, JS und Favicon.
 *
 * @return httpd_handle_t Handle des gestarteten HTTP-Servers oder NULL bei Fehler.
 */
httpd_handle_t start_webserver(void)
{
    httpd_handle_t http_server = NULL;              // Handle für den HTTP-Server
    httpd_config_t config = HTTPD_DEFAULT_CONFIG(); // Standardkonfiguration für den HTTP-Server

    // Startet den HTTP-Server
    if (httpd_start(&http_server, &config) == ESP_OK)
    {
        // Registriere die Handler für HTML, CSS, JS und Favicon
        httpd_register_uri_handler(http_server, &http_uri);
        httpd_register_uri_handler(http_server, &css_uri);
        httpd_register_uri_handler(http_server, &js_uri);
        httpd_register_uri_handler(http_server, &favicon_uri);
        httpd_register_uri_handler(http_server, &adc_uri);
        httpd_register_uri_handler(http_server, &temperature_uri);
        return http_server; // Gibt das Handle des gestarteten HTTP-Servers zurück
    }

    ESP_LOGI("WebServer", "Error starting http_server!"); // Fehler beim Starten des Servers
    return NULL;                                          // Gibt NULL zurück, wenn der Server nicht gestartet werden konnte
}