#ifndef MDNS_SERVER_H
#define MDNS_SERVER_H

#include "mdns.h"

/**
 * @file mdns_server.h
 * @brief Header file for the mDNS server configuration and initialization.
 */

/**
 * @brief Default mDNS hostname for the device.
 */
#define MDNS_NAME "Plantation"

/**
 * @brief Default mDNS instance name to be displayed.
 */
#define INSTANCE_NAME "Smart Plantation"

/**
 * @brief Default mDNS service type, indicating the type of service (e.g., HTTP, FTP).
 */
#define SERVICE_TYPE "_http._tcp"

/**
 * @brief Default protocol used by the mDNS service (e.g., TCP or UDP).
 */
#define PROTOCOL "tcp"

/**
 * @brief Default port for the mDNS service.
 */
#define PORT 80

/**
 * @brief Initializes and starts the mDNS service with the specified parameters.
 *
 * This function sets up the mDNS service for the device with a given hostname,
 * instance name, service type, protocol, and port. It also logs errors if the
 * service cannot be added successfully.
 *
 * @param hostname       The hostname to be assigned to the device.
 * @param instance_name  The instance name to be displayed in the mDNS query response.
 * @param service_type   The service type (e.g., "_http._tcp") that the mDNS service advertises.
 * @param protocol       The protocol used by the service (e.g., "tcp").
 * @param port           The port number on which the service is running.
 */
void start_mdns(const char *hostname, const char *instance_name, const char *service_type, const char *protocol, uint16_t port);

#endif // MDNS_SERVER_H
