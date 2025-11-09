#include "wifi.h"
#include "esp_wifi.h"
#include "esp_log.h"
#include "esp_event.h"
#include <string.h>

#define WIFI_SSID "AvaloN"
#define WIFI_PASS "@Teiubesc77@"

static const char *TAG = "WIFI_MODULE";

/**
 * @brief Event handler for Wifi events.
 *
 * @param arg User argument.
 * @param eventBase The type of event (WIFI_EVENT).
 * @param eventId The specific event id we're looking for (WIFI_EVENT_STA_START).
 * @param eventData Data associated with the event.
 */
static void WifiEventHandler(void* arg, esp_event_base_t eventBase, int32_t eventId,
                             void* eventData){
    if(eventBase == WIFI_EVENT && eventId == WIFI_EVENT_STA_START){
        esp_wifi_connect();
        ESP_LOGI(TAG, "Connecting to WiFi.");
    } else if (eventBase == WIFI_EVENT && eventId == WIFI_EVENT_STA_DISCONNECTED){
        ESP_LOGW(TAG, "Disconnected from Wifi, attempting to Reconnect.");
        esp_wifi_connect();
    }
}
/**
 * @brief Initializes the WiFi module in Station (STA) mode.Configures the SSID and password.
 *
 */
void WifiInit(void){
    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &WifiEventHandler, NULL));

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    wifi_config_t wifi_config = {0}; // Zero-initialize
    strcpy((char*)wifi_config.sta.ssid, WIFI_SSID);
    strcpy((char*)wifi_config.sta.password, WIFI_PASS);
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());
}
