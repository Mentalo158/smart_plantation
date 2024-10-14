#include <stdio.h>
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_err.h"
#include "wifi-server.h"
#include "adc_sensor.c"
#include "mdns.h" 

// FH-Kiel-IoT-NAT
// / !FH-NAT-001!
#define ADC_CH_4 ADC1_CHANNEL_4 // GPIO 32 auf dem ESP
#define ADC_CH_5 ADC1_CHANNEL_5 // GPIO 33 auf dem ESP

#define ADC_BITWIDTH ADC_WIDTH_BIT_12 // Je höher desto genauer hat seine vor- und nachteile zu lang zum erklären.
#define MDNS_NAME "Plantation"

// Funktion zur Initialisierung von mDNS
void start_mdns(const char *hostname)
{
    mdns_init();
    mdns_hostname_set(hostname);

    // Hier die korrekten Argumente angeben
    const char *instance_name = "Smart Plantation"; // Name des Dienstes
    const char *service_type = "_http._tcp";        // Dienstetyp
    const char *proto = "tcp";                      // Protokoll
    uint16_t port = 80;                             // Portnummer

    // Leeres txt array und die Anzahl der txt Einträge auf 0 setzen
    mdns_txt_item_t *txt = NULL;
    size_t num_items = 0;

    esp_err_t err = mdns_service_add(instance_name, service_type, proto, port, txt, num_items);
    if (err)
    {
        ESP_LOGE("mDNS", "Failed to add service: %s", esp_err_to_name(err));
    }
    else
    {
        ESP_LOGI("mDNS", "mDNS service added successfully.");
    }
}

void app_main(void)
{
    // Keine Ahnung, ChatGPT sagt das ist wichtig und cool
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    wifi_connection();      // Verbindung zum WLAN herstellen
    start_mdns(MDNS_NAME); // mDNS mit dem Hostnamen "mydevice" initialisieren
    start_webserver();      // HTTP-Server starten

    // adc_init(ADC_CH_4, ADC_BITWIDTH); // ADC-Konfiguration
    // float adcPercentage = adc_read_sensor(ADC_CH_4); // Sensor einlesen
    // adc_cleanup(); // ADC deaktivieren, wenn nicht mehr benötigt
}
