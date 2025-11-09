#ifndef MQTT_H
#define MQTT_H

void MqttAppStart(void);
void MqttPublishData(const char *topic, const char *data);

#endif