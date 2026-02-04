#pragma once
#include <stdbool.h>
#include "esp_err.h"

typedef struct {
    const char *broker_uri;   // ej: "mqtt://192.168.1.11"
    const char *client_id;    // ej: "esp32c3-001"
} mqtt_manager_cfg_t;

esp_err_t mqtt_manager_start(const mqtt_manager_cfg_t *cfg);
bool mqtt_manager_is_connected(void);

esp_err_t mqtt_manager_publish(const char *topic, const char *payload, int qos, int retain);
