#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "nvs_flash.h"

#include "sdkconfig.h"
#include "wifi_manager.h"
#include "mqtt_manager.h"

static const char *TAG = "main";

#define TOPIC_TELEMETRY "agro/smart-motor/telemetry"

void app_main(void)
{
    ESP_LOGI(TAG, "Etapa 2.0 - MQTT minimo");

    // NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ESP_ERROR_CHECK(nvs_flash_init());
    }

    // Wi-Fi (bloquea hasta IP)
    ESP_ERROR_CHECK(wifi_manager_init_sta(CONFIG_APP_WIFI_SSID, CONFIG_APP_WIFI_PASS));

    // MQTT
    mqtt_manager_cfg_t cfg = {
        .broker_uri = "mqtt://192.168.1.11",
        .client_id  = "esp32c3-agro-001",
    };
    ESP_ERROR_CHECK(mqtt_manager_start(&cfg));

    // Publicar cada 5s (hello)
    int counter = 0;
    while (1) {
        if (mqtt_manager_is_connected()) {
            char payload[64];
            snprintf(payload, sizeof(payload), "{\"hello\":%d}", counter++);
            mqtt_manager_publish(TOPIC_TELEMETRY, payload, 0, 0);
        } else {
            ESP_LOGW(TAG, "MQTT not connected yet...");
        }

        vTaskDelay(pdMS_TO_TICKS(5000));
    }
}
