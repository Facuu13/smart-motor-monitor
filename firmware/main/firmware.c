#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "nvs_flash.h"

#include "sdkconfig.h"
#include "wifi_manager.h"
#include "mqtt_manager.h"
#include "telemetry.h"

static const char *TAG = "main";


static void build_telemetry_topic(char *out, size_t out_len, const char *base, const char *device_id)
{
    // agro/smart-motor/<device_id>/telemetry
    snprintf(out, out_len, "%s/%s/telemetry", base, device_id);
}


void app_main(void)
{
    ESP_LOGI(TAG, "Etapa 2.2 - Telemetria (modulo)");

    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ESP_ERROR_CHECK(nvs_flash_init());
    }

    ESP_ERROR_CHECK(wifi_manager_init_sta(CONFIG_APP_WIFI_SSID, CONFIG_APP_WIFI_PASS));

    mqtt_manager_cfg_t mqtt_cfg = {
        .broker_uri = CONFIG_APP_MQTT_BROKER_URI,
        .client_id  = CONFIG_APP_MQTT_CLIENT_ID,
    };
    ESP_ERROR_CHECK(mqtt_manager_start(&mqtt_cfg));

    telemetry_cfg_t tel_cfg = {
        .device_id = CONFIG_APP_MQTT_CLIENT_ID, // reutilizamos el client_id como device_id
    };
    telemetry_init(&tel_cfg);

    while (1) {
        if (mqtt_manager_is_connected()) {
            char payload[160];
            if (telemetry_build_json(payload, sizeof(payload))) {
                char topic[128];
                build_telemetry_topic(topic, sizeof(topic), CONFIG_APP_MQTT_TOPIC_BASE, CONFIG_APP_MQTT_CLIENT_ID);
                mqtt_manager_publish(topic, payload, 0, 0);

            } else {
                ESP_LOGW(TAG, "telemetry_build_json: buffer too small?");
            }
        } else {
            ESP_LOGW(TAG, "MQTT not connected yet...");
        }

        vTaskDelay(pdMS_TO_TICKS(5000));
    }
}
