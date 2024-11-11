#include "wifi-server.h"
#include "esp_log.h"
#include "esp_wifi.h"
#include "nvs_flash.h"
#include "sdkconfig.h"
#include "sensors/temperature_sensor.h"
#include "peripherals/led_rgb_control.h"
#include <sys/param.h>
#include "sntp_client.h"
#include "backend/sntp_client.h"
#include "common/config_storage.h"
#include "peripherals/wifi_plug.h"
#include "esp_timer.h"

const char *ssid = CONFIG_WIFI_SSID;
const char *pass = CONFIG_WIFI_PASSWORD;
int retry_num = 0;

int64_t last_plug_activation = 0;      // Speichert den letzten Aktivierungszeitpunkt in Mikrosekunden
const int64_t PLUG_COOLDOWN = 6000000; // 3 Sekunden in Mikrosekunden (Cooldown-Zeitraum)

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
extern const uint8_t logo_png_start[] asm("_binary_logo_png_start");
extern const uint8_t logo_png_end[] asm("_binary_logo_png_end");

extern QueueHandle_t moistureDataQueue;
extern QueueHandle_t dhtDataQueue;
extern QueueHandle_t lightDataQueue;
extern QueueHandle_t led_queue;
extern QueueHandle_t config_queue;

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

        sntp_inits();
        set_time_zone();
        // sntp_update_time();
    }
}

esp_err_t http_handler(httpd_req_t *req)
{
    httpd_resp_set_type(req, "text/html");
    httpd_resp_send(req, (const char *)index_html_start, index_html_end - index_html_start);
    return ESP_OK;
}

esp_err_t css_handler(httpd_req_t *req)
{
    httpd_resp_set_type(req, "text/css");
    httpd_resp_send(req, (const char *)app_css_start, app_css_end - app_css_start);
    return ESP_OK;
}

esp_err_t js_handler(httpd_req_t *req)
{
    httpd_resp_set_type(req, "application/javascript");
    httpd_resp_send(req, (const char *)app_js_start, app_js_end - app_js_start);
    return ESP_OK;
}

esp_err_t favicon_handler(httpd_req_t *req)
{
    httpd_resp_set_type(req, "image/x-icon");
    httpd_resp_send(req, (const char *)favicon_ico_start, favicon_ico_end - favicon_ico_start);
    return ESP_OK;
}

esp_err_t htmx_handler(httpd_req_t *req)
{
    httpd_resp_set_type(req, "application/javascript");
    httpd_resp_send(req, (const char *)htmx_js_start, htmx_js_end - htmx_js_start);
    return ESP_OK;
}

esp_err_t logo_handler(httpd_req_t *req)
{
    httpd_resp_set_type(req, "image/png");
    httpd_resp_send(req, (const char *)logo_png_start, logo_png_end - logo_png_start);
    return ESP_OK;
}

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
        httpd_resp_send_404(req);
    }
    return ESP_OK;
}

esp_err_t temperature_value_handler(httpd_req_t *req)
{
    dht_data_t dhtData;

    if (xQueuePeek(dhtDataQueue, &dhtData, 0) == pdTRUE)
    {
        char response[128];
        snprintf(response, sizeof(response),
                 "Temperatur: %.2f °C\n"
                 "Luftfeuchtigkeit: %.2f %%",
                 dhtData.temperature, dhtData.humidity);

        httpd_resp_set_type(req, "text/plain");
        httpd_resp_send(req, response, strlen(response));
    }
    else
    {
        httpd_resp_send_404(req);
    }
    return ESP_OK;
}

esp_err_t light_sensor_value_handler(httpd_req_t *req)
{
    float lightPercentage = 0.0f;

    if (xQueuePeek(lightDataQueue, &lightPercentage, 0) == pdTRUE)
    {
        char response[64];
        snprintf(response, sizeof(response), "Lichtintensität: %.2f", lightPercentage); // Prozentsatz übergeben

        httpd_resp_set_type(req, "text/plain");
        httpd_resp_send(req, response, strlen(response));
    }
    else
    {
        httpd_resp_send_404(req);
    }

    return ESP_OK;
}

esp_err_t time_value_handler(httpd_req_t *req)
{
    time_t now = get_current_time();
    struct tm timeinfo;
    localtime_r(&now, &timeinfo);

    char response[128];
    snprintf(response, sizeof(response), "Aktuelle Zeit: %02d:%02d:%02d, Datum: %02d.%02d.%04d",
             timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec,
             timeinfo.tm_mday, timeinfo.tm_mon + 1, timeinfo.tm_year + 1900);

    httpd_resp_set_type(req, "text/plain");
    httpd_resp_send(req, response, strlen(response));
    return ESP_OK;
}

esp_err_t config_get_handler(httpd_req_t *req)
{
    config_t config;
    load_config(&config);

    char response[256];
    snprintf(response, sizeof(response),
             "{\"temp_threshold\": %ld, \"red\": %u, \"green\": %u, \"blue\": %u, \"hour\": %u, \"minute\": %u, \"days\": %u}",
             config.temp_threshold, config.red, config.green, config.blue, config.hour, config.minute, config.days);

    httpd_resp_set_type(req, "application/json");
    httpd_resp_send(req, response, strlen(response));
    return ESP_OK;
}

esp_err_t config_set_handler(httpd_req_t *req)
{
    char content[128];
    size_t recv_size = MIN(req->content_len, sizeof(content) - 1);
    int ret = httpd_req_recv(req, content, recv_size);
    if (ret <= 0)
    {
        if (ret == HTTPD_SOCK_ERR_TIMEOUT)
        {
            httpd_resp_send_408(req);
        }
        return ESP_FAIL;
    }

    content[recv_size] = '\0';

    // Neue Konfiguration aus der HTTP-Anfrage lesen
    config_t new_config;
    if (sscanf(content, "temp_threshold=%ld&red=%hhu&green=%hhu&blue=%hhu&hour=%hhu&minute=%hhu&days=%hhu",
               &new_config.temp_threshold, &new_config.red, &new_config.green,
               &new_config.blue, &new_config.hour, &new_config.minute, &new_config.days) == 7)
    {
        // Konfiguration in NVS speichern
        save_config(&new_config);

        // Neue Konfigurationsdaten für die Queue vorbereiten
        config_data_t config_data;
        config_data.hour = new_config.hour;
        config_data.minute = new_config.minute;
        config_data.days = new_config.days;

        // Konfigurationsdaten in die Queue schreiben (falls sie voll ist, wird sie überschrieben)
        if (xQueueOverwrite(config_queue, &config_data) != pdTRUE)
        {
            httpd_resp_send_500(req);
            return ESP_FAIL;
        }

        // Rückmeldung an den Webserver über erfolgreiche Aktualisierung
        httpd_resp_send(req, "Konfiguration erfolgreich aktualisiert", HTTPD_RESP_USE_STRLEN);
        return ESP_OK;
    }
    else
    {
        // Fehlerhafte Parameter
        httpd_resp_send_500(req);
        return ESP_FAIL;
    }
}

esp_err_t wifi_plug_switch_handler(httpd_req_t *req)
{
    int64_t current_time = esp_timer_get_time();

    // Prüfen, ob 3 Sekunden vergangen sind
    if (current_time - last_plug_activation >= PLUG_COOLDOWN)
    {
        // Wenn ja, dann die Steckdose umschalten
        if (tasmota_toggle_power(2000) == ESP_OK) // 2000 ms = 2 Sekunden
        {
            last_plug_activation = current_time; // Update den letzten Aktivierungszeitpunkt
            httpd_resp_sendstr(req, "Steckdose für 2 Sekunden eingeschaltet.");
            return ESP_OK;
        }
        else
        {
            httpd_resp_send_500(req);
            return ESP_FAIL;
        }
    }
    else
    {
        // Wenn weniger als 3 Sekunden vergangen sind, gebe eine Fehlermeldung zurück
        httpd_resp_sendstr(req, "Bitte warten, bevor Sie erneut klicken.");
        return ESP_OK;
    }
}

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
    .uri = "/app.js",
    .method = HTTP_GET,
    .handler = js_handler,
    .user_ctx = NULL};

httpd_uri_t favicon_uri = {
    .uri = "/favicon.ico",
    .method = HTTP_GET,
    .handler = favicon_handler,
    .user_ctx = NULL};

httpd_uri_t htmx_uri = {
    .uri = "/htmx.min.js",
    .method = HTTP_GET,
    .handler = htmx_handler,
    .user_ctx = NULL};

httpd_uri_t logo_uri = {
    .uri = "/logo.png",
    .method = HTTP_GET,
    .handler = logo_handler,
    .user_ctx = NULL};

httpd_uri_t adc_uri = {
    .uri = "/adc",
    .method = HTTP_GET,
    .handler = moisture_value_handler,
    .user_ctx = NULL};

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

httpd_uri_t time_uri = {
    .uri = "/time",
    .method = HTTP_GET,
    .handler = time_value_handler,
    .user_ctx = NULL};

httpd_uri_t config_get_uri = {
    .uri = "/config",
    .method = HTTP_GET,
    .handler = config_get_handler,
    .user_ctx = NULL};

httpd_uri_t config_set_uri = {
    .uri = "/config_set",
    .method = HTTP_POST,
    .handler = config_set_handler,
    .user_ctx = NULL};

// URI-Handler für die Steckdose
httpd_uri_t wifi_plug_switch_uri = {
    .uri = "/plug_switch",
    .method = HTTP_GET,
    .handler = wifi_plug_switch_handler,
    .user_ctx = NULL};

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

httpd_handle_t start_webserver(void)
{
    httpd_handle_t http_server = NULL;              // Handle für den HTTP-Server
    httpd_config_t config = HTTPD_DEFAULT_CONFIG(); // Standardkonfiguration für den HTTP-Server
    config.max_uri_handlers = 20;
    // Startet den HTTP-Server
    if (httpd_start(&http_server, &config) == ESP_OK)
    {
        // Registriere die Handlers
        httpd_register_uri_handler(http_server, &http_uri);
        httpd_register_uri_handler(http_server, &css_uri);
        httpd_register_uri_handler(http_server, &js_uri);
        httpd_register_uri_handler(http_server, &favicon_uri);
        httpd_register_uri_handler(http_server, &adc_uri);
        httpd_register_uri_handler(http_server, &temperature_uri);
        httpd_register_uri_handler(http_server, &light_uri);
        httpd_register_uri_handler(http_server, &logo_uri);
        httpd_register_uri_handler(http_server, &config_get_uri);
        httpd_register_uri_handler(http_server, &config_set_uri);
        httpd_register_uri_handler(http_server, &wifi_plug_switch_uri);
        if (httpd_register_uri_handler(http_server, &time_uri) != ESP_OK)
        {
            ESP_LOGE("WebServer", "Failed to register time URI handler");
        }

        return http_server; // Gibt das Handle des gestarteten HTTP-Servers zurück
    }

    ESP_LOGI("WebServer", "Error starting http_server!"); // Fehler beim Starten des Servers
    return NULL;                                          // Gibt NULL zurück, wenn der Server nicht gestartet werden konnte
}