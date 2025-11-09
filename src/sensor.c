#include "sensor.h"
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_log.h"
#include "esp_adc/adc_oneshot.h"

#include "mqtt.h"

extern EventGroupHandle_t app_event_group;
extern const int MQTT_CONNECTED_BIT;

#define ADC_PIN ADC_CHANNEL_6
#define MQTT_TOPIC "home/kitchen/gas/state"
#define WARMUP_TIME 20000
#define DELAY 1000

static const char *TAG = "SENSOR_MODULE";
static adc_oneshot_unit_handle_t adcHandle;

/**
 * @brief Configure the ADC for 0-.3.3v reading.
 *
 */
static void ConfigureAdc(void){
    adc_oneshot_unit_init_cfg_t initConfig = {
        .unit_id = ADC_UNIT_1
    };
    ESP_ERROR_CHECK(adc_oneshot_new_unit(&initConfig, &adcHandle));
    adc_oneshot_chan_cfg_t config = {
        .bitwidth = ADC_BITWIDTH_12,
        .atten = ADC_ATTEN_DB_12
    };
    ESP_ERROR_CHECK(adc_oneshot_config_channel(adcHandle, ADC_PIN, &config));
    ESP_LOGI(TAG, "Configured ADC on GPIO34");
}

/**
 * @brief Warms up the sensors and collects data infinetly with a 1 second delay inbetween.
 *
 * @param arg Task argument.
 */
static void SensorReadPublishTask(void *arg){
    ESP_LOGI(TAG, "Sensor powered on. Waiting to connect to MQTT.");
    xEventGroupWaitBits(app_event_group,
                        MQTT_CONNECTED_BIT,
                        pdFALSE,
                        pdFALSE,
                        portMAX_DELAY);

    ESP_LOGI(TAG, "MQTT conected, warming up the sensor(20 seconds).");
    vTaskDelay(pdMS_TO_TICKS(WARMUP_TIME));
    ESP_LOGI(TAG, "Sensor's ready, proceeding with data collection.");

    while (1) {
        int gasValue = 0;
        ESP_ERROR_CHECK(adc_oneshot_read(adcHandle, ADC_PIN, &gasValue));
        ESP_LOGI(TAG, "A0 value: %d", gasValue);

        char msg[8];
        sprintf(msg, "%d", gasValue);
        MqttPublishData(MQTT_TOPIC, msg);
        vTaskDelay(pdMS_TO_TICKS(DELAY));
    }
}
/**
 * @brief Initializes the sensor hardware.
 * 
 */
void SensorInit(void){
    ConfigureAdc();
}

/**
 * @brief Creates and starts the sensor's main task of collecting data.
 * 
 */
void SensorStartTask(void){
    xTaskCreate(SensorReadPublishTask, "sensor_task", 4096, NULL, 5, NULL);
}