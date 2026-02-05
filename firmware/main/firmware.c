#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "nvs_flash.h"

#include "sdkconfig.h"
#include "wifi_manager.h"
#include "mqtt_manager.h"
#include "telemetry.h"

#include "telemetry_queue.h"


static const char *TAG = "main";


static void build_telemetry_topic(char *out, size_t out_len, const char *base, const char *device_id)
{
    snprintf(out, out_len, "%s/%s/telemetry", base, device_id);
}

static void telemetry_task(void *arg)
{
    (void)arg;
    while (1) {
        char payload[160];
        if (telemetry_build_json(payload, sizeof(payload))) {
            (void)telemetry_queue_push(payload); // si se llena, se pierde (por ahora)
        }
        vTaskDelay(pdMS_TO_TICKS(5000));
    }
}

static void mqtt_pub_task(void *arg)
{
    (void)arg;

    char topic[128];
    build_telemetry_topic(topic, sizeof(topic), CONFIG_APP_MQTT_TOPIC_BASE, CONFIG_APP_MQTT_CLIENT_ID);

    while (1) {
        char payload[200];

        // Espera hasta 2000ms por un mensaje
        if (telemetry_queue_pop(payload, sizeof(payload), 2000)) {
            if (mqtt_manager_is_connected()) {
                mqtt_manager_publish(topic, payload, 0, 0);
            }
        }
    }
}



void app_main(void)
{
    ESP_LOGI(TAG, "Etapa 2.4 - Telemetria via queue + tasks");

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
        .device_id = CONFIG_APP_MQTT_CLIENT_ID,
    };
    telemetry_init(&tel_cfg);

    telemetry_queue_init();

    xTaskCreate(telemetry_task, "telemetry_task", 4096, NULL, 5, NULL);
    xTaskCreate(mqtt_pub_task,  "mqtt_pub_task",  4096, NULL, 5, NULL);


}

