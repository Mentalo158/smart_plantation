#include "wifi-server.h"
#include "esp_log.h"
#include "esp_wifi.h"
#include "nvs_flash.h"

// WiFi-Verbindungsdaten
const char *ssid = "FH-Kiel-IoT-NAT";
const char *pass = "!FH-NAT-001";
int retry_num = 0;

httpd_handle_t start_webserver(void);
void stop_webserver(httpd_handle_t server);


static void wifi_event_handler(void *event_handler_arg, esp_event_base_t event_base, int32_t event_id, void *event_data) {
    if (event_id == WIFI_EVENT_STA_START) {
        printf("WIFI CONNECTING....\n");
    } else if (event_id == WIFI_EVENT_STA_CONNECTED) {
        printf("WiFi CONNECTED\n");
    } else if (event_id == WIFI_EVENT_STA_DISCONNECTED) {
        printf("WiFi lost connection\n");
        if (retry_num < 5) {
            esp_wifi_connect();
            retry_num++;
            printf("Retrying to Connect...\n");
        }
    } else if (event_id == IP_EVENT_STA_GOT_IP) {
        printf("Wifi got IP...\n\n");
    }
}

// HTML CODE
esp_err_t custom_handler(httpd_req_t *req) {
    const char *html_head = "<html><head><style>"
                            "body {font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif; text-align: center; background-color: #2c3e50; color: #ecf0f1;}"
                            "h1 {color: #3498db;}"
                            "button {padding: 10px 20px; font-size: 16px; margin: 10px; border: none; border-radius: 5px; cursor: pointer;}"
                            "button#startButton {background-color: #2ecc71; color: white;}"
                            "button#stopButton {background-color: #e74c3c; color: white;}"
                            "#moistureBar {width: 80%; background-color: #34495e; border-radius: 5px; margin: 20px auto;}"
                            "#moistureProgress {height: 30px; border-radius: 5px; background-color: #2ecc71; text-align: center; line-height: 30px; color: white;}"
                            "</style></head><body>"
                            "<h1>Smart Plantation</h1>"

                            "<div id=\"moistureBar\">"
                            "  <div id=\"moistureProgress\" style=\"width: %d%%;\"></div>"
                            "</div>";

    const char *html_end = "</body></html>";

    // Dynamische Size
    size_t content_size = strlen(html_head) + strlen(html_end) + 20;
    char *content = (char *)malloc(content_size);
    if (content == NULL) {
        ESP_LOGE("main", "Failed to allocate memory for HTML content");
        httpd_resp_send_500(req);
        return ESP_FAIL;
    }
    sprintf(content, html_head, 50);
    httpd_resp_send(req, content, HTTPD_RESP_USE_STRLEN);
    free(content);

    return ESP_OK;
}

// URI-Definitionen und handler
httpd_uri_t custom_uri = {
        .uri = "/",
        .method = HTTP_GET,
        .handler = custom_handler,
        .user_ctx = NULL
};

// Funktion die Verbindung mit dem WIFI herstellt
void wifi_connection() {
    esp_netif_init();
    esp_event_loop_create_default();
    esp_netif_create_default_wifi_sta();
    wifi_init_config_t wifi_initiation = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&wifi_initiation);
    esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, wifi_event_handler, NULL);
    esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, wifi_event_handler, NULL);

    wifi_config_t wifi_configuration = {
            .sta = {
                    .ssid = "FH-Kiel-IoT-NAT",
                    .password = "!FH-NAT-001",
            }
    };

    strcpy((char *)wifi_configuration.sta.ssid, ssid);
    strcpy((char *)wifi_configuration.sta.password, pass);
    esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_configuration);
    esp_wifi_start();
    esp_wifi_set_mode(WIFI_MODE_STA);
    esp_wifi_connect();
    printf("wifi_init_softap finished. SSID:%s  password:%s", ssid, pass);
}

// Funktion zum starten des Servers
httpd_handle_t start_webserver(void) {
    httpd_handle_t server = NULL;
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();

    if (httpd_start(&server, &config) == ESP_OK) {
        httpd_register_uri_handler(server, &custom_uri);
        return server;
    }

    ESP_LOGI("WebServer", "Error starting server!");
    return NULL;
}