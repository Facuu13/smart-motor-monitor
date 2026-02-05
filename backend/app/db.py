import os
import psycopg2
from psycopg2.extras import Json

DB_HOST = os.getenv("DB_HOST", "127.0.0.1")
DB_PORT = int(os.getenv("DB_PORT", "5432"))
DB_NAME = os.getenv("DB_NAME", "smartmotor")
DB_USER = os.getenv("DB_USER", "smartmotor")
DB_PASS = os.getenv("DB_PASS", "smartmotor")


def get_conn():
    return psycopg2.connect(
        host=DB_HOST,
        port=DB_PORT,
        dbname=DB_NAME,
        user=DB_USER,
        password=DB_PASS,
    )


def init_db():
    with get_conn() as conn, conn.cursor() as cur:
        cur.execute("""
        CREATE TABLE IF NOT EXISTS telemetry (
            id BIGSERIAL PRIMARY KEY,
            ts_ingest TIMESTAMPTZ NOT NULL DEFAULT NOW(),
            device_id TEXT NOT NULL,
            topic TEXT NOT NULL,
            payload JSONB NOT NULL
        );
        """)
        conn.commit()


def insert_telemetry(device_id: str, topic: str, payload: dict):
    with get_conn() as conn, conn.cursor() as cur:
        cur.execute(
            "INSERT INTO telemetry (device_id, topic, payload) VALUES (%s, %s, %s)",
            (device_id, topic, Json(payload)),
        )
        conn.commit()
