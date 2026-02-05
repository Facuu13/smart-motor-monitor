from fastapi import FastAPI, HTTPException, Query
from .db import init_db
from .queries import list_devices, latest_telemetry, latest_all

app = FastAPI(title="Smart Motor Monitor API", version="0.1.0")


@app.on_event("startup")
def _startup():
    # asegura tabla creada
    init_db()


@app.get("/health")
def health():
    return {"status": "ok"}


@app.get("/devices")
def devices(limit: int = Query(100, ge=1, le=1000)):
    return {"devices": list_devices(limit=limit)}


@app.get("/telemetry/latest")
def telemetry_latest(device_id: str = Query(..., min_length=1)):
    data = latest_telemetry(device_id=device_id)
    if not data:
        raise HTTPException(status_code=404, detail="device_id not found")
    return data


@app.get("/telemetry/recent")
def telemetry_recent(limit: int = Query(50, ge=1, le=500)):
    return {"items": latest_all(limit=limit)}
