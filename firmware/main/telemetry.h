#pragma once
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

typedef struct {
    const char *device_id;
} telemetry_cfg_t;

// Inicializa el módulo (por ahora solo guarda config).
void telemetry_init(const telemetry_cfg_t *cfg);

// Genera JSON de telemetría en `out`.
// Devuelve true si entró en el buffer.
bool telemetry_build_json(char *out, size_t out_len);
