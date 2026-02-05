#include "telemetry_queue.h"
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

#define TELEMETRY_Q_LEN  10
#define TELEMETRY_MAX_PAYLOAD 200

typedef struct {
    char payload[TELEMETRY_MAX_PAYLOAD];
} telemetry_msg_t;

static QueueHandle_t s_q = NULL;

void telemetry_queue_init(void)
{
    if (!s_q) {
        s_q = xQueueCreate(TELEMETRY_Q_LEN, sizeof(telemetry_msg_t));
    }
}

bool telemetry_queue_push(const char *payload)
{
    if (!s_q || !payload) return false;

    telemetry_msg_t msg = {0};
    strncpy(msg.payload, payload, sizeof(msg.payload) - 1);

    return xQueueSend(s_q, &msg, 0) == pdTRUE;
}

bool telemetry_queue_pop(char *out, size_t out_len, int timeout_ms)
{
    if (!s_q || !out || out_len == 0) return false;

    telemetry_msg_t msg;
    TickType_t ticks = (timeout_ms <= 0) ? 0 : pdMS_TO_TICKS(timeout_ms);

    if (xQueueReceive(s_q, &msg, ticks) == pdTRUE) {
        strncpy(out, msg.payload, out_len - 1);
        out[out_len - 1] = '\0';
        return true;
    }
    return false;
}
