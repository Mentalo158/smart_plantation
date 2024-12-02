#include "wifi_connection.h"

const char *ssid = CONFIG_WIFI_SSID;
const char *pass = CONFIG_WIFI_PASSWORD;
int retry_num = 0;

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
    }
}

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