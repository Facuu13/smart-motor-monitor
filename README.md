# Smart Motor Monitor — Agro IoT

End-to-end IoT system for monitoring agricultural motors and pumps using **ESP32**, **MQTT**, **PostgreSQL**, and **Docker**.

This project simulates a real-world **predictive maintenance** solution, covering the full stack:
embedded firmware → message broker → backend ingestion → database → API → dashboard.

---

## 🚜 Use Case

Agricultural environments rely heavily on electric motors (irrigation pumps, ventilation systems, etc.).
Unexpected motor failures can cause:

- Water supply interruptions
- Crop damage
- Costly emergency maintenance

This system provides **continuous telemetry monitoring** to enable early detection of abnormal behavior.

---

## 🧱 System Architecture

```

ESP32 (ESP-IDF)
│
│  MQTT
▼
Mosquitto Broker
│
▼
Backend Ingest (Python)
│
▼
PostgreSQL
│
▼
FastAPI REST API
│
▼
Minimal Web Dashboard

```

---

## 🔧 Firmware (ESP32-C3, ESP-IDF)

**Key features:**
- ESP-IDF + FreeRTOS
- Modular architecture
- Wi-Fi manager with:
  - retry logic
  - event groups
  - blocking until IP
- MQTT client module
- Telemetry pipeline using:
  - dedicated tasks
  - FreeRTOS queue
- Configurable via `menuconfig`

**Published topic structure:**
```

agro/smart-motor/<device_id>/telemetry

````

**Telemetry payload (example):**
```json
{
  "device_id": "esp32c3-agro-001",
  "ts": 123,
  "rssi": -47,
  "temp_c": 27.3,
  "vib_rms": 0.42
}
````

> Sensors are simulated, but the architecture is designed for real sensors (temperature, vibration, current).

---

## 🖥 Backend

### MQTT Ingest Service

* Subscribes to:

  ```
  agro/smart-motor/+/telemetry
  ```
* Parses JSON payloads
* Persists telemetry to PostgreSQL

### Database (PostgreSQL)

* JSONB payload storage
* Ingestion timestamp
* Device-based indexing

### REST API (FastAPI)

Available endpoints:

* `GET /health`
* `GET /devices`
* `GET /telemetry/latest`
* `GET /telemetry/latest_all`

API documentation available at:

```
/docs
```

---

## 🐳 Dockerized Infrastructure

All infrastructure runs via **Docker Compose**:

* Mosquitto (MQTT broker)
* PostgreSQL
* Backend ingest service
* Backend API service

Includes:

* Health checks
* Proper service dependencies
* Restart policies
* Internal Docker networking

Start everything with:

```bash
docker compose up -d --build
```

---

## 📊 Dashboard

A minimal HTML + JavaScript dashboard served by FastAPI:

* Lists all devices
* Displays latest telemetry per device
* Computes status:

  * 🟢 OK
  * 🟡 Warning
  * 🔴 Critical

Purposefully simple — focused on validating the end-to-end data flow.

---

