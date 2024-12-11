#ifndef MDNS_SERVER_H
#define MDNS_SERVER_H

#include "mdns.h"

#define MDNS_NAME "Plantation"
#define INSTANCE_NAME "Smart Plantation"
#define SERVICE_TYPE "_http._tcp"
#define PROTOCOL "tcp"
#define PORT 80

void start_mdns(const char *hostname, const char *instance_name, const char *service_type, const char *protocol, uint16_t port);

#endif // MDNS_SERVER_H
