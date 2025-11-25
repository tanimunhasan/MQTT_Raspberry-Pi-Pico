#ifndef MQTT_H
#define MQTT_H

#include "lwip/apps/mqtt.h"
#include "lwip/ip_addr.h"
#include "pico/stdlib.h"

// Initializes MQTT client and connects to broker
void mqtt_init_connection(void);

// Publishes a single float reading to a topic
void mqtt_publish_reading(float reading);
void mqtt_service(void);           // call each main-loop iteration

#endif
