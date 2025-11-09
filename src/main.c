#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "nvs_flash.h"
#include "esp_netif.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_wifi_default.h"


#include "wifi.h"
#include "mqtt.h"
#include "sensor.h"

static const char *TAG = "MAIN_APP";

EventGroupHandle_t app_event_group;
const int MQTT_CONNECTED_BIT = BIT0;
/**
 * @brief Event handler for IP-related events. After IP_EVENT_STA_GOT_IP is set, which signals that
 * the connection to the wifi is succesful, and the esp has recieved an ip adress, it starts the mqtt conenction process.
 *
 * @param arg User argument
 * @param eventBase The type of event (IP_EVENT)
 * @param eventId The specific event ID we're looking for (IP_EVENT_STA_GOT_IP).
 * @param eventData Data associated with our event.
 */
static void IpEventHandler(void* arg, esp_event_base_t eventBase,
                             int32_t eventId, void* eventData)
{
    if (eventBase == IP_EVENT && eventId == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t* event = (ip_event_got_ip_t*) eventData;
        ESP_LOGI(TAG, "Got IP address: " IPSTR, IP2STR(&event->ip_info.ip));
        MqttAppStart();
    }
}

void app_main(void)
{
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
      ESP_ERROR_CHECK(nvs_flash_erase());
      ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    // This line creates the network interface
    esp_netif_create_default_wifi_sta();

    app_event_group = xEventGroupCreate();

    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &IpEventHandler, NULL));

    SensorInit();
    WifiInit();
    SensorStartTask();

    ESP_LOGI(TAG, "startup completed succesfully.");
}