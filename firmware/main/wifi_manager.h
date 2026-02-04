#pragma once
#include "esp_err.h"

// Inicializa Wi-Fi STA, reintenta y bloquea hasta obtener IP o fallar.
esp_err_t wifi_manager_init_sta(const char *ssid, const char *pass);
