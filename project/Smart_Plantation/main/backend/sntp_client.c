#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "esp_log.h"
#include "esp_sntp.h"
#include "nvs_flash.h"
#include "sntp_client.h"

static const char *TAG = "SNTP_CLIENT";
static time_t current_time;


void sntp_inits()
{
    ESP_LOGI(TAG, "Starting SNTP");
    esp_sntp_setoperatingmode(SNTP_OPMODE_POLL);
    esp_sntp_setservername(0, "pool.ntp.org"); 
    esp_sntp_init();
}


void sntp_update_time()
{
    struct tm timeinfo;

    time(&current_time);
    localtime_r(&current_time, &timeinfo);
    ESP_LOGI(TAG, "Updated time: %s", asctime(&timeinfo));
}


time_t get_current_time(void)
{
    return current_time; 
}

void set_time_zone()
{
    setenv("TZ", "CET-1CEST,M3.5.0/2,M10.5.0/3", 1); 
    tzset();                                         
}

int get_current_day_of_week()
{
    time_t now = get_current_time();
    struct tm timeinfo;
    localtime_r(&now, &timeinfo);
    return timeinfo.tm_wday;
}