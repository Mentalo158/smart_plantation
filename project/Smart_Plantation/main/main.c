#include <stdio.h>
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_err.h"
#include "wifi-server.h"
#include "adc_sensor.c"
#include "mdns.h"

// FH-Kiel-IoT-NAT
// / !FH-NAT-001!
#define ADC_CH_4 ADC1_CHANNEL_4       // GPIO 32 auf dem ESP
#define ADC_CH_5 ADC1_CHANNEL_5       // GPIO 33 auf dem ESP
#define ADC_BITWIDTH ADC_WIDTH_BIT_12 // Je höher desto genauer, hat seine Vor- und Nachteile.

// MDNS Config
#define MDNS_NAME "Plantation"
#define INSTANCE_NAME "Smart Plantation"
#define SERVICE_TYPE "_http._tcp"
#define PROTOCOL "tcp"
#define PORT 80

/**
 * @brief Initialisiert den mDNS-Dienst für das Gerät.
 *
 * Diese Funktion richtet den mDNS-Dienst ein, damit das Gerät im lokalen Netzwerk
 * als Dienst angezeigt wird. Es verwendet den angegebenen Hostnamen, Dienstnamen
 * und Netzwerkprotokoll.
 *
 * @param hostname Der Hostname, der für mDNS verwendet werden soll.
 * @param instance_name Der Name des Dienstes (z.B. "Smart Plantation").
 * @param service_type Der Typ des Dienstes (z.B. "_http._tcp").
 * @param protocol Das Netzwerkprotokoll (z.B. "tcp").
 * @param port Die Portnummer, auf der der Dienst erreichbar ist.
 *
 * @return void
 */
void start_mdns(const char *hostname, const char *instance_name, const char *service_type, const char *protocol, uint16_t port)
{
    mdns_init();
    mdns_hostname_set(hostname);

    mdns_txt_item_t *txt = NULL;
    size_t num_items = 0;

    esp_err_t err = mdns_service_add(instance_name, service_type, protocol, port, txt, num_items);
    if (err)
    {
        ESP_LOGE("mDNS", "Failed to add service: %s", esp_err_to_name(err));
    }
    else
    {
        ESP_LOGI("mDNS", "mDNS service added successfully.");
    }
}

/**
 * @brief Die Hauptanwendungsfunktion, die beim Start des Geräts aufgerufen wird.
 *
 * Diese Funktion initialisiert den Flash-Speicher, stellt die WLAN-Verbindung her,
 * startet den mDNS-Dienst und einen Webserver. Zusätzlich kann sie den ADC
 * konfigurieren und Sensorwerte lesen (momentan auskommentiert).
 *
 * @return void
 */
void app_main(void)
{
    // Initialisiere den NVS-Flash-Speicher
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    // WLAN-Verbindung herstellen
    wifi_connection();

    // mDNS-Dienst starten
    start_mdns(MDNS_NAME, INSTANCE_NAME, SERVICE_TYPE, PROTOCOL, PORT);

    // HTTP-Server starten
    start_webserver();

    // Auskommentierter Code für ADC-Konfiguration und Sensorwert-Lesen
    // adc_init(ADC_CH_4, ADC_BITWIDTH); // ADC-Konfiguration
    // float adcPercentage = adc_read_sensor(ADC_CH_4); // Sensor einlesen
    // adc_cleanup(); // ADC deaktivieren, wenn nicht mehr benötigt
}
