#pragma once
#include <stdbool.h>
#include <stddef.h>

void telemetry_queue_init(void);

// Encola un payload JSON (copia interna). Devuelve true si pudo.
bool telemetry_queue_push(const char *payload);

// Intenta sacar un payload (copia en out). Bloquea hasta timeout_ms.
// timeout_ms = 0 => no bloquea
bool telemetry_queue_pop(char *out, size_t out_len, int timeout_ms);
