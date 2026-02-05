idf_component_register(SRCS "firmware.c" "wifi_manager.c" "mqtt_manager.c" "telemetry.c" "telemetry_queue.c"
                       INCLUDE_DIRS "."
                       REQUIRES nvs_flash esp_wifi esp_netif esp_event mqtt)
