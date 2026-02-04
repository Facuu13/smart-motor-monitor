#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "nvs_flash.h"

#include "sdkconfig.h"
#include "wifi_manager.h"
#include "mqtt_manager.h"

static const char *TAG = "main";

void app_main(void)
{
    ESP_LOGI(TAG, "Etapa 2.1 - MQTT via menuconfig");

    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ESP_ERROR_CHECK(nvs_flash_init());
    }

    ESP_ERROR_CHECK(wifi_manager_init_sta(CONFIG_APP_WIFI_SSID, CONFIG_APP_WIFI_PASS));

    mqtt_manager_cfg_t cfg = {
        .broker_uri = CONFIG_APP_MQTT_BROKER_URI,
        .client_id  = CONFIG_APP_MQTT_CLIENT_ID,
    };
    ESP_ERROR_CHECK(mqtt_manager_start(&cfg));

    int counter = 0;
    while (1) {
        if (mqtt_manager_is_connected()) {
            char payload[64];
            snprintf(payload, sizeof(payload), "{\"hello\":%d}", counter++);
            mqtt_manager_publish(CONFIG_APP_MQTT_TOPIC_TELEMETRY, payload, 0, 0);
        } else {
            ESP_LOGW(TAG, "MQTT not connected yet...");
        }
        vTaskDelay(pdMS_TO_TICKS(5000));
    }
}
