import json
from typing import Any, Dict, Optional, Tuple

import paho.mqtt.client as mqtt

from .config import MQTT_HOST, MQTT_PORT, MQTT_TOPIC

from .db import init_db, insert_telemetry



def parse_topic(topic: str) -> Tuple[Optional[str], str]:
    # agro/smart-motor/<device_id>/telemetry
    parts = topic.split("/")
    if len(parts) >= 4 and parts[0] == "agro" and parts[1] == "smart-motor":
        return parts[2], parts[3]
    return None, parts[-1]


def on_connect(client: mqtt.Client, userdata, flags, rc):
    if rc == 0:
        print(f"[MQTT] Connected to {MQTT_HOST}:{MQTT_PORT}")
        client.subscribe(MQTT_TOPIC)
        print(f"[MQTT] Subscribed to {MQTT_TOPIC}")
    else:
        print(f"[MQTT] Connect failed rc={rc}")


def on_message(client: mqtt.Client, userdata, msg: mqtt.MQTTMessage):
    topic = msg.topic
    payload_raw = msg.payload.decode("utf-8", errors="replace")

    device_id_from_topic, kind = parse_topic(topic)

    try:
        data: Dict[str, Any] = json.loads(payload_raw)
    except json.JSONDecodeError:
        print(f"[MQTT] INVALID JSON topic={topic} payload={payload_raw}")
        return

    device_id = data.get("device_id") or device_id_from_topic or "unknown"

    insert_telemetry(device_id=device_id, topic=topic, payload=data)

    print(f"[MQTT] SAVED device={device_id} kind={kind} topic={topic} data={data}")


def run_forever():
    init_db()

    client = mqtt.Client(client_id="backend-smart-motor-monitor")
    client.on_connect = on_connect
    client.on_message = on_message

    client.connect(MQTT_HOST, MQTT_PORT, keepalive=30)
    client.loop_forever()
