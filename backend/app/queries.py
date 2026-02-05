from typing import Any, Dict, List, Optional
from .db import get_conn


def list_devices(limit: int = 100) -> List[str]:
    with get_conn() as conn, conn.cursor() as cur:
        cur.execute(
            """
            SELECT DISTINCT device_id
            FROM telemetry
            ORDER BY device_id
            LIMIT %s
            """,
            (limit,),
        )
        rows = cur.fetchall()
    return [r[0] for r in rows]


def latest_telemetry(device_id: str) -> Optional[Dict[str, Any]]:
    with get_conn() as conn, conn.cursor() as cur:
        cur.execute(
            """
            SELECT id, ts_ingest, device_id, topic, payload
            FROM telemetry
            WHERE device_id = %s
            ORDER BY id DESC
            LIMIT 1
            """,
            (device_id,),
        )
        row = cur.fetchone()

    if not row:
        return None

    _id, ts_ingest, dev, topic, payload = row
    return {
        "id": _id,
        "ts_ingest": ts_ingest.isoformat(),
        "device_id": dev,
        "topic": topic,
        "payload": payload,
    }


def latest_all(limit: int = 50) -> List[Dict[str, Any]]:
    with get_conn() as conn, conn.cursor() as cur:
        cur.execute(
            """
            SELECT id, ts_ingest, device_id, topic, payload
            FROM telemetry
            ORDER BY id DESC
            LIMIT %s
            """,
            (limit,),
        )
        rows = cur.fetchall()

    out: List[Dict[str, Any]] = []
    for _id, ts_ingest, dev, topic, payload in rows:
        out.append(
            {
                "id": _id,
                "ts_ingest": ts_ingest.isoformat(),
                "device_id": dev,
                "topic": topic,
                "payload": payload,
            }
        )
    return out


def latest_per_device(limit_devices: int = 200) -> List[Dict[str, Any]]:
    """
    Devuelve el último registro por device_id (Postgres).
    """
    with get_conn() as conn, conn.cursor() as cur:
        cur.execute(
            """
            SELECT DISTINCT ON (device_id)
                id, ts_ingest, device_id, topic, payload
            FROM telemetry
            ORDER BY device_id, id DESC
            LIMIT %s
            """,
            (limit_devices,),
        )
        rows = cur.fetchall()

    out: List[Dict[str, Any]] = []
    for _id, ts_ingest, dev, topic, payload in rows:
        out.append(
            {
                "id": _id,
                "ts_ingest": ts_ingest.isoformat(),
                "device_id": dev,
                "topic": topic,
                "payload": payload,
            }
        )
    return out

