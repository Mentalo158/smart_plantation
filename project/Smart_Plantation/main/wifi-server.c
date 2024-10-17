#include "wifi-server.h"
#include "esp_log.h"
#include "esp_wifi.h"
#include "nvs_flash.h"
#include "sdkconfig.h"
#include "temperature_sensor.h"

const char *ssid = CONFIG_WIFI_SSID;
const char *pass = CONFIG_WIFI_PASSWORD;
int retry_num = 0;

/**
 * @brief Eingebettete binäre Daten für verschiedene Ressourcen.
 *
 * Diese externen Variablen referenzieren die Start- und Endpunkte
 * von eingebetteten binären Daten, die in der Anwendung verwendet
 * werden. Die Daten umfassen HTML, CSS, JavaScript und Favicon-Ressourcen.
 *
 * Jede Datei benötigt sowohl einen Start- als auch einen Endzeiger,
 * um den vollständigen Bereich der eingebetteten Daten im Speicher
 * korrekt zu definieren.
 * - Der Startzeiger (@c _start) zeigt auf den Beginn der Datei im
 *   Speicher, wodurch der Zugriff auf die Daten ermöglicht wird.
 * - Der Endzeiger (@c _end) definiert das Ende der Datei, was wichtig
 *   ist, um die Größe der Daten zu bestimmen und sicherzustellen,
 *   dass beim Zugriff auf die Daten keine unzulässigen Speicherbereiche
 *   überschritten werden.
 *
 * @note Jede Variable wird mit einer speziellen Assemblierungsanweisung
 * (@c asm) versehen, um sicherzustellen, dass die richtigen
 * Symbole im Binary verfügbar sind.
 */
extern const uint8_t index_html_start[] asm("_binary_index_html_start");
extern const uint8_t index_html_end[] asm("_binary_index_html_end");
extern const uint8_t app_css_start[] asm("_binary_app_css_start");
extern const uint8_t app_css_end[] asm("_binary_app_css_end");
extern const uint8_t app_js_start[] asm("_binary_app_js_start");
extern const uint8_t app_js_end[] asm("_binary_app_js_end");
extern const uint8_t favicon_ico_start[] asm("_binary_favicon_ico_start");
extern const uint8_t favicon_ico_end[] asm("_binary_favicon_ico_end");
extern const uint8_t htmx_js_start[] asm("_binary_htmx_min_js_start");
extern const uint8_t htmx_js_end[] asm("_binary_htmx_min_js_end");

extern QueueHandle_t moistureDataQueue;
extern QueueHandle_t dhtDataQueue;
extern QueueHandle_t lightDataQueue;

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
void wifi_event_handler(void *event_handler_arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
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

// Handler für HTMX-Bibliothek
esp_err_t htmx_handler(httpd_req_t *req)
{
    httpd_resp_set_type(req, "application/javascript");
    httpd_resp_send(req, (const char *)htmx_js_start, htmx_js_end - htmx_js_start);
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
    float moisturePercentage = 0.0f;

    if (xQueuePeek(moistureDataQueue, &moisturePercentage, 0) == pdTRUE)
    {
        char response[128];
        // Gebe den Feuchtigkeitswert zurück
        snprintf(response, sizeof(response), "Bodenfeuchte: %.2f%%", moisturePercentage);
        httpd_resp_set_type(req, "text/plain");
        httpd_resp_send(req, response, strlen(response));
    }
    else
    {
        httpd_resp_send_404(req); // Keine Daten vorhanden
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
    dht_data_t dhtData;

    if (xQueuePeek(dhtDataQueue, &dhtData, 0) == pdTRUE)
    {
        char response[128]; // Passen wir die Größe an, da keine <div> um den Text nötig ist
        snprintf(response, sizeof(response),
                 "Temperatur: %.2f °C\n"
                 "Luftfeuchtigkeit: %.2f %%",
                 dhtData.temperature, dhtData.humidity);

        httpd_resp_set_type(req, "text/plain"); // Es wird nur Text zurückgegeben
        httpd_resp_send(req, response, strlen(response));
    }
    else
    {
        httpd_resp_send_404(req); // Keine Daten vorhanden
    }
    return ESP_OK;
}

esp_err_t light_sensor_value_handler(httpd_req_t *req)
{
    float lightPercentage = 0.0f;

    // Überprüfe, ob ein Lichtwert in der Queue verfügbar ist
    if (xQueuePeek(lightDataQueue, &lightPercentage, 0) == pdTRUE)
    {
        // Antwort mit Lichtwert (in Prozent)
        char response[64];
        snprintf(response, sizeof(response), "%.2f", lightPercentage); // Nur der Prozentsatz wird zurückgegeben

        httpd_resp_set_type(req, "text/plain");           // Rückgabe als einfacher Text
        httpd_resp_send(req, response, strlen(response)); // Sende den Lichtwert zurück
    }
    else
    {
        httpd_resp_send_404(req); // Falls keine Daten verfügbar sind, sende 404
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

// URI-Definition für HTMX JS-Datei
httpd_uri_t htmx_uri = {
    .uri = "/htmx.min.js",
    .method = HTTP_GET,
    .handler = htmx_handler,
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

httpd_uri_t light_uri = {
    .uri = "/light",
    .method = HTTP_GET,
    .handler = light_sensor_value_handler,
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
        httpd_register_uri_handler(http_server, &light_uri);

        return http_server; // Gibt das Handle des gestarteten HTTP-Servers zurück
    }

    ESP_LOGI("WebServer", "Error starting http_server!"); // Fehler beim Starten des Servers
    return NULL;                                          // Gibt NULL zurück, wenn der Server nicht gestartet werden konnte
}