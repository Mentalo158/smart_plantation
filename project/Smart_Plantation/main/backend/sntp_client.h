#ifndef SNTP_CLIENT_H
#define SNTP_CLIENT_H

#include <time.h>
#include "esp_err.h"

// Funktion zur Initialisierung des SNTP-Clients
void sntp_inits();

// Funktion zur Aktualisierung der Zeit
void sntp_update_time();

// Funktion zur Rückgabe der aktuellen Zeit
time_t get_current_time(void);

// Funktion zur Einstellung der Zeitzone
void set_time_zone();

#endif // SNTP_CLIENT_H
