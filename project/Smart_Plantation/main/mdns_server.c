#include "mdns_server.h"
#include "esp_log.h"

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
    // Initialisiere mDNS
    mdns_init();
    mdns_hostname_set(hostname);
    mdns_instance_name_set(instance_name);

    // Leere TXT-Einträge, falls benötigt, können hier gesetzt werden
    mdns_txt_item_t *txt = NULL;
    size_t num_items = 0;

    // Füge den mDNS-Dienst hinzu
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
