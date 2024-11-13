#include "wifi_plug.h"
#include <esp_http_client.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <stdio.h>
#include <string.h>
#include "esp_log.h"

const char *TAG = "wifi_plug"; // Definiert den TAG für dieses Modul
const char *wifiPlugIP = CONFIG_WIFI_PLUG_IP;

static esp_err_t _http_event_handler(esp_http_client_event_t *evt)
{
    switch (evt->event_id)
    {
    case HTTP_EVENT_ERROR:
        ESP_LOGI(TAG, "HTTP_EVENT_ERROR");
        break;
    case HTTP_EVENT_ON_CONNECTED:
        ESP_LOGI(TAG, "HTTP_EVENT_ON_CONNECTED");
        break;
    case HTTP_EVENT_HEADER_SENT:
        ESP_LOGI(TAG, "HTTP_EVENT_HEADER_SENT");
        break;
    case HTTP_EVENT_ON_HEADER:
        ESP_LOGI(TAG, "HTTP_EVENT_ON_HEADER, key=%s, value=%s", evt->header_key, evt->header_value);
        break;
    case HTTP_EVENT_ON_DATA:
        ESP_LOGI(TAG, "HTTP_EVENT_ON_DATA, len=%d", evt->data_len);
        if (!esp_http_client_is_chunked_response(evt->client))
        {
            // Wenn Daten direkt empfangen werden
            printf("%.*s", evt->data_len, (char *)evt->data);
        }
        break;
    case HTTP_EVENT_ON_FINISH:
        ESP_LOGI(TAG, "HTTP_EVENT_ON_FINISH");
        break;
    case HTTP_EVENT_DISCONNECTED:
        ESP_LOGI(TAG, "HTTP_EVENT_DISCONNECTED");
        break;
    case HTTP_EVENT_REDIRECT: // Neuer Fall für das Redirect-Event
        ESP_LOGI(TAG, "HTTP_EVENT_REDIRECT");
        break;
    default:
        ESP_LOGW(TAG, "Unknown HTTP event: %d", evt->event_id);
        break;
    }
    return ESP_OK;
}

// Funktion zum Ein- und Ausschalten der Tasmota-Steckdose
esp_err_t tasmota_toggle_power(uint32_t duration_ms)
{
    // Steckdose einschalten
    char url_on[64];
    snprintf(url_on, sizeof(url_on), "http://%s:%d/cm?cmnd=Power%%20On", wifiPlugIP, TASMOTA_PORT);

    esp_http_client_config_t config_on = {
        .url = url_on,
        .event_handler = _http_event_handler,
    };

    esp_http_client_handle_t client_on = esp_http_client_init(&config_on);
    esp_err_t err = esp_http_client_perform(client_on);
    esp_http_client_cleanup(client_on);

    if (err == ESP_OK)
    {
        printf("Steckdose eingeschaltet.\n");
    }
    else
    {
        printf("Fehler beim Einschalten der Steckdose: %s\n", esp_err_to_name(err));
        return err;
    }

    // Warten für die angegebene Dauer
    vTaskDelay(duration_ms / portTICK_PERIOD_MS);

    // Steckdose ausschalten
    char url_off[64];
    snprintf(url_off, sizeof(url_off), "http://%s:%d/cm?cmnd=Power%%20Off", wifiPlugIP, TASMOTA_PORT);

    esp_http_client_config_t config_off = {
        .url = url_off,
        .event_handler = _http_event_handler,
    };

    esp_http_client_handle_t client_off = esp_http_client_init(&config_off);
    err = esp_http_client_perform(client_off);
    esp_http_client_cleanup(client_off);

    if (err == ESP_OK)
    {
        printf("Steckdose ausgeschaltet.\n");
    }
    else
    {
        printf("Fehler beim Ausschalten der Steckdose: %s\n", esp_err_to_name(err));
    }

    return err;
}
