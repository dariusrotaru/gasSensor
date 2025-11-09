#include "mqtt.h"
#include "mqtt_client.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"

extern EventGroupHandle_t app_event_group;
extern const int MQTT_CONNECTED_BIT;

#define MQTT_BROKER     "mqtt://192.168.50.91"
#define MQTT_USER       "proxxym"
#define MQTT_PASS       "@#Teiubesc77@#"
#define MQTT_CLIENT_ID  "gasSensor"

static const char *TAG = "MQTT_MODULE";
static esp_mqtt_client_handle_t mqttClient;
/**
 * @brief Event handler for MQTT events.
 *
 * @param arg User argument.
 * @param eventBase The type of event (MQTT_EVENT).
 * @param eventId The specific event id we're looking for.
 * @param eventData Data associated with the event.
 */
static void MqttEventHandler(void* arg, esp_event_base_t eventBase,
                             int32_t eventId, void* eventData){
    (void)eventData;
    switch ((esp_mqtt_event_id_t)eventId){
        case MQTT_EVENT_CONNECTED:
            ESP_LOGI(TAG, "Connected MQTT.");
            xEventGroupSetBits(app_event_group, MQTT_CONNECTED_BIT);
            break;
        case MQTT_EVENT_DISCONNECTED:
            ESP_LOGW(TAG, "Disconnected MQTT.");
            xEventGroupClearBits(app_event_group, MQTT_CONNECTED_BIT);
            break;
        case MQTT_EVENT_ERROR:
            ESP_LOGE(TAG, "ERROR MQTT!");
            break;
        default:
            break;
    }
}
/**
 * @brief Initialize and start the MQTT client by populating the ip, username, pass.
 * 
 */
void MqttAppStart(void){
    esp_mqtt_client_config_t mqttCfg = {
        .broker.address.uri = MQTT_BROKER,
        .credentials = {
            .username = MQTT_USER,
            .client_id = MQTT_CLIENT_ID,
            .authentication.password = MQTT_PASS,
        },
    };
    mqttClient = esp_mqtt_client_init(&mqttCfg);
    ESP_ERROR_CHECK(esp_mqtt_client_register_event(mqttClient, (esp_mqtt_event_id_t)ESP_EVENT_ANY_ID, &MqttEventHandler, NULL));
    ESP_ERROR_CHECK(esp_mqtt_client_start(mqttClient));
}

/**
 * @brief Publishes oru data payload to the specific MQTT topic (see sensor.c).
 *
 * @param topic the MQTT topic to publish to ("home/kitchen/gas/state").
 * @param data the string payload to send (0-4095).
 */
void MqttPublishData(const char *topic, const char *data){
    if(mqttClient){
        esp_mqtt_client_publish(mqttClient, topic, data, 0, 1, 1);
    }
}