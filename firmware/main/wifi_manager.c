#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"

#include "esp_log.h"
#include "esp_netif.h"
#include "esp_event.h"
#include "esp_wifi.h"

#include "wifi_manager.h"

static const char *TAG = "wifi_mgr";

#define WIFI_CONNECTED_BIT BIT0
#define WIFI_FAIL_BIT      BIT1

#define WIFI_MAX_RETRY     10

static EventGroupHandle_t s_wifi_event_group = NULL;
static int s_retry_count = 0;

static void wifi_event_handler(void *arg,
                               esp_event_base_t event_base,
                               int32_t event_id,
                               void *event_data)
{
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        ESP_LOGI(TAG, "STA_START -> connect");
        esp_wifi_connect();
        return;
    }

    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        wifi_event_sta_disconnected_t *disc = (wifi_event_sta_disconnected_t *)event_data;
        ESP_LOGW(TAG, "STA_DISCONNECTED reason=%d", disc ? disc->reason : -1);

        if (s_retry_count < WIFI_MAX_RETRY) {
            s_retry_count++;
            ESP_LOGI(TAG, "Retrying Wi-Fi... (%d/%d)", s_retry_count, WIFI_MAX_RETRY);
            esp_wifi_connect();
        } else {
            ESP_LOGE(TAG, "Wi-Fi failed after %d retries", WIFI_MAX_RETRY);
            xEventGroupSetBits(s_wifi_event_group, WIFI_FAIL_BIT);
        }
        return;
    }
}

static void ip_event_handler(void *arg,
                             esp_event_base_t event_base,
                             int32_t event_id,
                             void *event_data)
{
    if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;
        ESP_LOGI(TAG, "GOT_IP: " IPSTR, IP2STR(&event->ip_info.ip));

        s_retry_count = 0;
        xEventGroupSetBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
        return;
    }
}

esp_err_t wifi_manager_init_sta(const char *ssid, const char *pass)
{
    if (!ssid || !pass) return ESP_ERR_INVALID_ARG;
    if (strlen(ssid) == 0) return ESP_ERR_INVALID_ARG; // SSID vacío no tiene sentido

    // Crear event group (si ya existe, no recrear)
    if (s_wifi_event_group == NULL) {
        s_wifi_event_group = xEventGroupCreate();
        if (!s_wifi_event_group) return ESP_ERR_NO_MEM;
    } else {
        xEventGroupClearBits(s_wifi_event_group, WIFI_CONNECTED_BIT | WIFI_FAIL_BIT);
    }

    // netif + event loop (si ya existen, estas calls pueden fallar; lo manejamos simple)
    // Para tu proyecto actual se llama 1 sola vez, así que OK.
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_event_handler, NULL));
    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &ip_event_handler, NULL));

    wifi_config_t wifi_config = {0};
    strncpy((char *)wifi_config.sta.ssid, ssid, sizeof(wifi_config.sta.ssid));
    strncpy((char *)wifi_config.sta.password, pass, sizeof(wifi_config.sta.password));

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));

    s_retry_count = 0;
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_LOGI(TAG, "Waiting for connection result...");

    EventBits_t bits = xEventGroupWaitBits(
        s_wifi_event_group,
        WIFI_CONNECTED_BIT | WIFI_FAIL_BIT,
        pdFALSE,
        pdFALSE,
        portMAX_DELAY
    );

    if (bits & WIFI_CONNECTED_BIT) {
        ESP_LOGI(TAG, "Wi-Fi connected + IP acquired ✅");
        return ESP_OK;
    }
    if (bits & WIFI_FAIL_BIT) {
        ESP_LOGE(TAG, "Wi-Fi connection failed ❌");
        return ESP_FAIL;
    }

    return ESP_FAIL;
}
