#include "telemetry.h"

#include <stdio.h>
#include <string.h>

#include "esp_log.h"
#include "esp_timer.h"
#include "esp_random.h"
#include "esp_wifi.h"

static const char *TAG = "telemetry";
static const char *s_device_id = "unknown";

void telemetry_init(const telemetry_cfg_t *cfg)
{
    if (cfg && cfg->device_id && strlen(cfg->device_id) > 0) {
        s_device_id = cfg->device_id;
    }
    ESP_LOGI(TAG, "telemetry_init device_id=%s", s_device_id);
}

// RSSI real si estás conectado a un AP; si no, -127
static int get_rssi_dbm(void)
{
    wifi_ap_record_t ap_info;
    if (esp_wifi_sta_get_ap_info(&ap_info) == ESP_OK) {
        return ap_info.rssi;
    }
    return -127;
}

// Uptime en segundos (por ahora). Más adelante metemos SNTP para epoch real.
static uint32_t uptime_s(void)
{
    return (uint32_t)(esp_timer_get_time() / 1000000ULL);
}

static float sim_temp_c(void)
{
    // Simulación simple: 20.0 .. 35.0
    uint32_t r = esp_random() % 1501; // 0..1500
    return 20.0f + (r / 100.0f);
}

static float sim_vib_rms(void)
{
    // Simulación simple: 0.00 .. 1.00
    uint32_t r = esp_random() % 101; // 0..100
    return r / 100.0f;
}

bool telemetry_build_json(char *out, size_t out_len)
{
    if (!out || out_len == 0) return false;

    int rssi = get_rssi_dbm();
    uint32_t ts = uptime_s();
    float temp = sim_temp_c();
    float vib  = sim_vib_rms();

    // JSON “contrato” básico, fácil de parsear
    int n = snprintf(out, out_len,
                     "{\"device_id\":\"%s\",\"ts\":%u,\"rssi\":%d,"
                     "\"temp_c\":%.2f,\"vib_rms\":%.2f}",
                     s_device_id, (unsigned)ts, rssi, temp, vib);

    return (n > 0 && (size_t)n < out_len);
}
