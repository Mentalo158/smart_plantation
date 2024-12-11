#ifndef SNTP_CLIENT_H
#define SNTP_CLIENT_H

#include <time.h>
#include "esp_err.h"

void sntp_inits();

void sntp_update_time();

time_t get_current_time(void);

void set_time_zone();

int get_current_day_of_week();

#endif 
