import os

MQTT_HOST = os.getenv("MQTT_HOST", "192.168.1.11")
MQTT_PORT = int(os.getenv("MQTT_PORT", "1883"))
MQTT_TOPIC = os.getenv("MQTT_TOPIC", "agro/smart-motor/+/telemetry")
