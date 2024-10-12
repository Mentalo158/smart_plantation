#include <stdio.h>
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_err.h"
#include "wifi-server.h"
#include "adc_sensor.c"

#define ADC_CH_4 ADC1_CHANNEL_4 // GPIO 32 auf dem ESP
#define ADC_CH_5 ADC1_CHANNEL_5 // GPIO 33 auf dem ESP

#define ADC_BITWIDTH ADC_WIDTH_BIT_12 //Je höher desto genauer hat seine vor- und nachteile zu lang zum erklären.
// Wenn die Bit anzahl verändert wird muss auch die adcToPercentage funktion angepasst werden adc durch max Bitbreite adc/2^12 - 1 bspw.

void app_main(void)
{
    // Keine Ahnung, ChatGPT sagt das ist wichtig und cool
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    wifi_connection();
    start_webserver();

    // adc_init(ADC_CH_4, ADC_BITWIDTH); // Muss vorher einmal aktiviert werden zum konfigurieren des jeweiligen channels
    // float adcPercentage = adc_read_sensor(ADC_CH_4); // ein Sensor einlesen, einfach channel übergeben implementier das in ner while schleife
    // adc_cleanup();      // Um die ADC's auszuschalten am Ende wenn wir das benötigen(Nicht in while nur eimal ausführen)
}
