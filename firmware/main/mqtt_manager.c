#include <string.h>
#include "esp_log.h"
#include "mqtt_client.h"
#include "mqtt_manager.h"

static const char *TAG = "mqtt_mgr";

static esp_mqtt_client_handle_t s_client = NULL;
static bool s_connected = false;

static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
    esp_mqtt_event_handle_t event = (esp_mqtt_event_handle_t)event_data;

    switch ((esp_mqtt_event_id_t)event_id) {
        case MQTT_EVENT_CONNECTED:
            s_connected = true;
            ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED");
            break;

        case MQTT_EVENT_DISCONNECTED:
            s_connected = false;
            ESP_LOGW(TAG, "MQTT_EVENT_DISCONNECTED");
            break;

        case MQTT_EVENT_ERROR:
            ESP_LOGE(TAG, "MQTT_EVENT_ERROR");
            break;

        default:
            break;
    }
}

esp_err_t mqtt_manager_start(const mqtt_manager_cfg_t *cfg)
{
    if (!cfg || !cfg->broker_uri || strlen(cfg->broker_uri) == 0) return ESP_ERR_INVALID_ARG;

    esp_mqtt_client_config_t mqtt_cfg = {
        .broker.address.uri = cfg->broker_uri,
        .credentials.client_id = cfg->client_id,
    };

    s_client = esp_mqtt_client_init(&mqtt_cfg);
    if (!s_client) return ESP_ERR_NO_MEM;

    ESP_ERROR_CHECK(esp_mqtt_client_register_event(s_client, ESP_EVENT_ANY_ID, mqtt_event_handler, NULL));
    ESP_ERROR_CHECK(esp_mqtt_client_start(s_client));

    ESP_LOGI(TAG, "mqtt_manager_start() uri=%s", cfg->broker_uri);
    return ESP_OK;
}

bool mqtt_manager_is_connected(void)
{
    return s_connected;
}

esp_err_t mqtt_manager_publish(const char *topic, const char *payload, int qos, int retain)
{
    if (!s_client) return ESP_ERR_INVALID_STATE;
    if (!topic || !payload) return ESP_ERR_INVALID_ARG;

    int msg_id = esp_mqtt_client_publish(s_client, topic, payload, 0, qos, retain);
    if (msg_id < 0) {
        ESP_LOGE(TAG, "Publish failed");
        return ESP_FAIL;
    }

    ESP_LOGI(TAG, "Published msg_id=%d topic=%s payload=%s", msg_id, topic, payload);
    return ESP_OK;
}
