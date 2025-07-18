#include "http_server.h"
#include "esp_log.h"
#include "esp_wifi.h"
#include "nvs_flash.h"
#include "sdkconfig.h"
#include "sensors/temperature_sensor.h"
#include "peripherals/led_rgb_control.h"
#include <sys/param.h>
#include "sntp_client.h"
#include "backend/sntp_client.h"
#include "common/config_storage.h"
#include "peripherals/wifi_plug.h"
#include "esp_timer.h"
#include "esp_http_server.h"
#include "string.h"
#include "stdlib.h"
#include "cJSON.h"
#include "sensors/light_sensor.h"

int64_t last_plug_activation = 0;
const int64_t PLUG_COOLDOWN = 6000000;

extern const uint8_t index_html_start[] asm("_binary_index_html_start");
extern const uint8_t index_html_end[] asm("_binary_index_html_end");
extern const uint8_t app_css_start[] asm("_binary_app_css_start");
extern const uint8_t app_css_end[] asm("_binary_app_css_end");
extern const uint8_t app_js_start[] asm("_binary_app_js_start");
extern const uint8_t app_js_end[] asm("_binary_app_js_end");
extern const uint8_t logo_png_start[] asm("_binary_logo_png_start");
extern const uint8_t logo_png_end[] asm("_binary_logo_png_end");

extern QueueHandle_t moistureDataQueue;
extern QueueHandle_t dhtDataQueue;
extern QueueHandle_t lightDataQueue;
extern QueueHandle_t led_queue;
extern QueueHandle_t pump_queue;
extern QueueHandle_t fan_queue;
extern QueueHandle_t soil_queue;
extern QueueHandle_t moisture_enabled_queue;
extern QueueHandle_t tempThresholdQueue;
extern QueueHandle_t dynamicLightQueue;
extern QueueHandle_t luxOrIntensityQueue;
extern QueueHandle_t lightIntensityThresholdQueue;
extern QueueHandle_t lightLuxThresholdQueue;

esp_err_t http_handler(httpd_req_t *req)
{
    httpd_resp_set_type(req, "text/html");
    httpd_resp_send(req, (const char *)index_html_start, index_html_end - index_html_start);
    return ESP_OK;
}

esp_err_t css_handler(httpd_req_t *req)
{
    httpd_resp_set_type(req, "text/css");
    httpd_resp_send(req, (const char *)app_css_start, app_css_end - app_css_start);
    return ESP_OK;
}

esp_err_t js_handler(httpd_req_t *req)
{
    httpd_resp_set_type(req, "application/javascript");
    httpd_resp_send(req, (const char *)app_js_start, app_js_end - app_js_start);
    return ESP_OK;
}

esp_err_t logo_handler(httpd_req_t *req)
{
    httpd_resp_set_type(req, "image/png");
    httpd_resp_send(req, (const char *)logo_png_start, logo_png_end - logo_png_start);
    return ESP_OK;
}

esp_err_t moisture_value_handler(httpd_req_t *req)
{
    float moisturePercentage = 0.0f;

    if (xQueuePeek(moistureDataQueue, &moisturePercentage, 0) == pdTRUE)
    {
        char response[128];
        
        snprintf(response, sizeof(response), "Bodenfeuchte: %.2f%%", moisturePercentage);
        httpd_resp_set_type(req, "text/plain");
        httpd_resp_send(req, response, strlen(response));
    }
    else
    {
        httpd_resp_send_404(req);
    }
    return ESP_OK;
}

esp_err_t temperature_value_handler(httpd_req_t *req)
{
    dht_data_t dhtData;

    if (xQueuePeek(dhtDataQueue, &dhtData, 0) == pdTRUE)
    {
        char response[128];
        snprintf(response, sizeof(response),
                 "Temperatur: %.2f °C\n"
                 "Luftfeuchtigkeit: %.2f %%",
                 dhtData.temperature, dhtData.humidity);

        httpd_resp_set_type(req, "text/plain");
        httpd_resp_send(req, response, strlen(response));
    }
    else
    {
        httpd_resp_send_404(req);
    }
    return ESP_OK;
}

esp_err_t light_sensor_value_handler(httpd_req_t *req)
{
    LightState lightState;

    
    if (xQueuePeek(lightDataQueue, &lightState, 0) == pdTRUE)
    {
        
        char response[128];
        snprintf(response, sizeof(response),
                 "Lichtintensität: %.2f%%\nLux-Wert: %u",
                 lightState.light_intensity, lightState.lux_value); 

        
        httpd_resp_set_type(req, "text/plain");
        httpd_resp_send(req, response, strlen(response));
    }
    else
    {
        
        httpd_resp_send_404(req);
    }

    return ESP_OK;
}

esp_err_t config_get_handler(httpd_req_t *req)
{
    config_t config;
    load_config(&config); 

    
    cJSON *root = cJSON_CreateObject();
    if (root == NULL)
    {
        ESP_LOGE("CONFIG_HANDLER", "Fehler beim Erstellen des JSON-Objekts.");
        httpd_resp_send_500(req);
        return ESP_FAIL;
    }

    cJSON_AddNumberToObject(root, "temp_threshold", config.temp_threshold);
    cJSON_AddNumberToObject(root, "moisture_threshold", config.moisture_threshold);
    cJSON_AddBoolToObject(root, "moisture_enabled", config.moisture_enabled);
    cJSON_AddBoolToObject(root, "temp_enabled", config.temp_enabled);
    cJSON_AddNumberToObject(root, "red", config.red);
    cJSON_AddNumberToObject(root, "green", config.green);
    cJSON_AddNumberToObject(root, "blue", config.blue);
    cJSON_AddNumberToObject(root, "days", config.days);
    cJSON_AddBoolToObject(root, "use_luminance_or_light_intensity", config.use_luminance_or_light_intensity);
    cJSON_AddBoolToObject(root, "use_dynamic_lightning", config.use_dynamic_lightning);

    
    cJSON_AddNumberToObject(root, "luminance", config.luminance);             
    cJSON_AddNumberToObject(root, "light_intensity", config.light_intensity); 

    
    cJSON *times = cJSON_CreateArray();
    if (times == NULL)
    {
        ESP_LOGE("CONFIG_HANDLER", "Fehler beim Erstellen des Times-Arrays.");
        cJSON_Delete(root);
        httpd_resp_send_500(req);
        return ESP_FAIL;
    }

    for (int i = 0; i < MAX_TIMES; i++)
    {
        cJSON *time_obj = cJSON_CreateObject();
        cJSON_AddNumberToObject(time_obj, "hour", config.hours[i]);
        cJSON_AddNumberToObject(time_obj, "minute", config.minutes[i]);
        cJSON_AddItemToArray(times, time_obj);
    }
    cJSON_AddItemToObject(root, "times", times);

    
    char *json_response = cJSON_PrintUnformatted(root);
    if (json_response == NULL)
    {
        ESP_LOGE("CONFIG_HANDLER", "Fehler beim Erstellen der JSON-Antwort.");
        cJSON_Delete(root);
        httpd_resp_send_500(req);
        return ESP_FAIL;
    }
    httpd_resp_send(req, json_response, strlen(json_response));

    
    cJSON_Delete(root);
    free(json_response);

    return ESP_OK;
}

esp_err_t config_set_handler(httpd_req_t *req)
{
    char content[512];
    size_t recv_size = MIN(req->content_len, sizeof(content) - 1);
    int ret = httpd_req_recv(req, content, recv_size);
    if (ret <= 0)
    {
        if (ret == HTTPD_SOCK_ERR_TIMEOUT)
        {
            httpd_resp_send_408(req);
        }
        return ESP_FAIL;
    }

    content[recv_size] = '\0';

    
    ESP_LOGI("CONFIG_HANDLER", "Empfangene POST-Daten: %s", content);

    
    cJSON *json = cJSON_Parse(content);
    if (json == NULL)
    {
        ESP_LOGE("CONFIG_HANDLER", "Fehler beim Parsen des JSON.");
        httpd_resp_send_500(req); 
        return ESP_FAIL;
    }

    config_t new_config = {0};

    
    cJSON *temp_threshold = cJSON_GetObjectItemCaseSensitive(json, "temp_threshold");
    cJSON *moisture_threshold = cJSON_GetObjectItemCaseSensitive(json, "moisture_threshold");
    cJSON *moisture_enabled = cJSON_GetObjectItemCaseSensitive(json, "moisture_enabled");
    cJSON *temp_enabled = cJSON_GetObjectItemCaseSensitive(json, "temp_enabled");
    cJSON *red = cJSON_GetObjectItemCaseSensitive(json, "red");
    cJSON *green = cJSON_GetObjectItemCaseSensitive(json, "green");
    cJSON *blue = cJSON_GetObjectItemCaseSensitive(json, "blue");
    cJSON *days = cJSON_GetObjectItemCaseSensitive(json, "days");
    cJSON *hours = cJSON_GetObjectItemCaseSensitive(json, "hours");
    cJSON *minutes = cJSON_GetObjectItemCaseSensitive(json, "minutes");
    cJSON *use_luminance_or_light_intensity = cJSON_GetObjectItemCaseSensitive(json, "use_luminance_or_light_intensity");
    cJSON *use_dynamic_lightning = cJSON_GetObjectItemCaseSensitive(json, "use_dynamic_lightning");

    
    cJSON *luminance = cJSON_GetObjectItemCaseSensitive(json, "luminance");
    cJSON *light_intensity = cJSON_GetObjectItemCaseSensitive(json, "light_intensity");

    
    if (cJSON_IsNumber(temp_threshold))
    {
        new_config.temp_threshold = temp_threshold->valueint;
    }
    if (cJSON_IsNumber(moisture_threshold))
    {
        new_config.moisture_threshold = moisture_threshold->valueint;
    }
    if (cJSON_IsBool(moisture_enabled))
    {
        new_config.moisture_enabled = cJSON_IsTrue(moisture_enabled);
    }
    if (cJSON_IsBool(temp_enabled))
    {
        new_config.temp_enabled = cJSON_IsTrue(temp_enabled);
    }
    if (cJSON_IsNumber(red))
    {
        new_config.red = red->valueint;
    }
    if (cJSON_IsNumber(green))
    {
        new_config.green = green->valueint;
    }
    if (cJSON_IsNumber(blue))
    {
        new_config.blue = blue->valueint;
    }
    if (cJSON_IsNumber(days))
    {
        new_config.days = days->valueint;
    }
    if (cJSON_IsBool(use_luminance_or_light_intensity))
    {
        new_config.use_luminance_or_light_intensity = cJSON_IsTrue(use_luminance_or_light_intensity);
    }
    if (cJSON_IsBool(use_dynamic_lightning))
    {
        new_config.use_dynamic_lightning = cJSON_IsTrue(use_dynamic_lightning);
    }

    
    if (cJSON_IsNumber(luminance))
    {
        new_config.luminance = luminance->valueint;
    }
    if (cJSON_IsNumber(light_intensity))
    {
        new_config.light_intensity = light_intensity->valueint;
    }

    
    if (cJSON_IsArray(hours) && cJSON_IsArray(minutes))
    {
        int i = 0;
        cJSON *hour_item = NULL;
        cJSON *minute_item = NULL;
        cJSON_ArrayForEach(hour_item, hours)
        {
            if (i < MAX_TIMES && cJSON_IsNumber(hour_item))
            {
                new_config.hours[i] = hour_item->valueint;
            }
            i++;
        }

        i = 0;
        cJSON_ArrayForEach(minute_item, minutes)
        {
            if (i < MAX_TIMES && cJSON_IsNumber(minute_item))
            {
                new_config.minutes[i] = minute_item->valueint;
            }
            i++;
        }
    }

    
    pump_times pump_config_data;

    
    for (int i = 0; i < 7; i++)
    {
        pump_config_data.hours[i] = new_config.hours[i];     
        pump_config_data.minutes[i] = new_config.minutes[i]; 
    }

    
    pump_config_data.days = new_config.days;

    
    if (xQueueOverwrite(pump_queue, &pump_config_data) != pdTRUE)
    {
        ESP_LOGE("CONFIG_HANDLER", "Fehler beim Überschreiben in config_queue");
        httpd_resp_send_500(req);
        return ESP_FAIL;
    }

    uint8_t temp_enabled_status = (new_config.temp_enabled) ? 1 : 0;
    if (xQueueSend(fan_queue, &temp_enabled_status, pdMS_TO_TICKS(10)) != pdTRUE)
    {
        ESP_LOGE("CONFIG_HANDLER", "Fehler beim Senden in die fan_queue");
        httpd_resp_send_500(req);
        return ESP_FAIL;
    }

    uint8_t moisture_enabled_status = (new_config.moisture_enabled) ? 1 : 0;
    if (xQueueSend(moisture_enabled_queue, &moisture_enabled_status, pdMS_TO_TICKS(10)) != pdTRUE)
    {
        ESP_LOGE("CONFIG_HANDLER", "Fehler beim Senden in die moisture_enabled_queue");
        httpd_resp_send_500(req);
        return ESP_FAIL;
    }

    if (new_config.use_dynamic_lightning != 0)
    {
        led_color_t led_data;
        led_data.red = new_config.red;
        led_data.green = new_config.green;
        led_data.blue = new_config.blue;

        if (xQueueSend(led_queue, &led_data, pdMS_TO_TICKS(10)) != pdTRUE)
        {
            ESP_LOGE("CONFIG_HANDLER", "Fehler beim Senden in die led_queue");
            httpd_resp_send_500(req);
            return ESP_FAIL;
        }
    }

    uint8_t soil_value = new_config.moisture_threshold;
    if (xQueueOverwrite(soil_queue, &soil_value) != pdPASS)
    {
        ESP_LOGE("CONFIG_HANDLER", "Fehler beim Überschreiben der soil_queue");
        httpd_resp_send_500(req);
        return ESP_FAIL;
    }

    uint8_t dynamicLightFlag = new_config.use_dynamic_lightning;
    if (xQueueOverwrite(dynamicLightQueue, &dynamicLightFlag) != pdPASS)
    {
        ESP_LOGE("CONFIG_HANDLER", "Fehler beim Überschreiben der dynamicLightQueue");
        httpd_resp_send_500(req);
        return ESP_FAIL;
    }

    uint8_t isLuxOrIntensity = new_config.use_luminance_or_light_intensity;
    if (xQueueOverwrite(luxOrIntensityQueue, &isLuxOrIntensity) != pdPASS)
    {
        ESP_LOGE("CONFIG_HANDLER", "Fehler beim Überschreiben der luxOrIntensityQueue");
        httpd_resp_send_500(req);
        return ESP_FAIL;
    }

    uint8_t lightIntensity = new_config.light_intensity;
    if (xQueueOverwrite(lightIntensityThresholdQueue, &lightIntensity) != pdPASS)
    {
        ESP_LOGE("CONFIG_HANDLER", "Fehler beim Überschreiben der lightIntensityThresholdQueue");
        httpd_resp_send_500(req);
        return ESP_FAIL;
    }

    uint32_t lightLuxThreshold = new_config.luminance;
    if (xQueueOverwrite(lightLuxThresholdQueue, &lightLuxThreshold) != pdPASS)
    {
    ESP_LOGE("CONFIG_HANDLER", "Fehler beim Überschreiben der lightLuxThresholdQueue");
    }

    uint8_t tmp_thres = new_config.temp_threshold;
    if (xQueueOverwrite(tempThresholdQueue, &tmp_thres) != pdPASS)
    {
        ESP_LOGE("CONFIG_HANDLER", "Fehler beim Überschreiben der tempThresholdQueue");
        httpd_resp_send_500(req);
        return ESP_FAIL;
    }

    save_config(&new_config);

    httpd_resp_send(req, "Konfiguration erfolgreich aktualisiert", HTTPD_RESP_USE_STRLEN);

    
    cJSON_Delete(json);

    return ESP_OK;

}

esp_err_t wifi_plug_switch_handler(httpd_req_t *req)
{
    int64_t current_time = esp_timer_get_time();

    
    if (current_time - last_plug_activation >= PLUG_COOLDOWN)
    {
        
        if (tasmota_toggle_power(4000) == ESP_OK) 
        {
            last_plug_activation = current_time; 
            httpd_resp_sendstr(req, "Steckdose für 2 Sekunden eingeschaltet.");
            return ESP_OK;
        }
        else
        {
            httpd_resp_send_500(req);
            return ESP_FAIL;
        }
    }
    else
    {
        
        httpd_resp_sendstr(req, "Bitte warten, bevor Sie erneut klicken.");
        return ESP_OK;
    }
}

httpd_uri_t http_uri = {
    .uri = "/",
    .method = HTTP_GET,
    .handler = http_handler,
    .user_ctx = NULL};

httpd_uri_t css_uri = {
    .uri = "/app.css",
    .method = HTTP_GET,
    .handler = css_handler,
    .user_ctx = NULL};

httpd_uri_t js_uri = {
    .uri = "/app.js",
    .method = HTTP_GET,
    .handler = js_handler,
    .user_ctx = NULL};

httpd_uri_t logo_uri = {
    .uri = "/logo.png",
    .method = HTTP_GET,
    .handler = logo_handler,
    .user_ctx = NULL};

httpd_uri_t adc_uri = {
    .uri = "/adc",
    .method = HTTP_GET,
    .handler = moisture_value_handler,
    .user_ctx = NULL};

httpd_uri_t temperature_uri = {
    .uri = "/temperature",
    .method = HTTP_GET,
    .handler = temperature_value_handler,
    .user_ctx = NULL};

httpd_uri_t light_uri = {
    .uri = "/light",
    .method = HTTP_GET,
    .handler = light_sensor_value_handler,
    .user_ctx = NULL};

httpd_uri_t config_get_uri = {
    .uri = "/config",
    .method = HTTP_GET,
    .handler = config_get_handler,
    .user_ctx = NULL};

httpd_uri_t config_set_uri = {
    .uri = "/config_set",
    .method = HTTP_POST,
    .handler = config_set_handler,
    .user_ctx = NULL};


httpd_uri_t wifi_plug_switch_uri = {
    .uri = "/plug_switch",
    .method = HTTP_GET,
    .handler = wifi_plug_switch_handler,
    .user_ctx = NULL};

httpd_handle_t start_webserver()
{
    httpd_handle_t http_server = NULL;              
    httpd_config_t config = HTTPD_DEFAULT_CONFIG(); 
    config.max_uri_handlers = 20;
    
    if (httpd_start(&http_server, &config) == ESP_OK)
    {
        
        httpd_register_uri_handler(http_server, &http_uri);
        httpd_register_uri_handler(http_server, &css_uri);
        httpd_register_uri_handler(http_server, &js_uri);
        httpd_register_uri_handler(http_server, &adc_uri);
        httpd_register_uri_handler(http_server, &temperature_uri);
        httpd_register_uri_handler(http_server, &light_uri);
        httpd_register_uri_handler(http_server, &logo_uri);
        httpd_register_uri_handler(http_server, &config_get_uri);
        httpd_register_uri_handler(http_server, &config_set_uri);
        httpd_register_uri_handler(http_server, &wifi_plug_switch_uri);

        return http_server; 
    }

    ESP_LOGI("WebServer", "Error starting http_server!"); 
    return NULL;                                          
}