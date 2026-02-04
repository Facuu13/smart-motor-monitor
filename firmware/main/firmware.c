#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "wifi_manager.h"

#include "sdkconfig.h"   // To use CONFIG_APP_WIFI_SSID and CONFIG_APP_WIFI_PASS

static const char *TAG = "main";

void app_main(void)
{
    ESP_LOGI(TAG, "Etapa 1.2 - Wi-Fi credentials via menuconfig");

    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ESP_ERROR_CHECK(nvs_flash_init());
    }

    ESP_ERROR_CHECK(wifi_manager_init_sta(CONFIG_APP_WIFI_SSID, CONFIG_APP_WIFI_PASS));

    while (1) {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
