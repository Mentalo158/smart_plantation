#ifndef MDNS_SERVER_H
#define MDNS_SERVER_H

#include "mdns.h"

// MDNS Config Definitions
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
void start_mdns(const char *hostname, const char *instance_name, const char *service_type, const char *protocol, uint16_t port);

#endif // MDNS_SERVER_H
