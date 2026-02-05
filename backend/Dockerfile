FROM python:3.12-slim

ENV PYTHONDONTWRITEBYTECODE=1 \
    PYTHONUNBUFFERED=1

WORKDIR /app

# deps de build mínimas (psycopg2-binary suele andar sin gcc, pero esto es seguro y liviano)
RUN apt-get update && apt-get install -y --no-install-recommends \
    ca-certificates \
  && rm -rf /var/lib/apt/lists/*

COPY requirements.txt /app/requirements.txt
RUN pip install --no-cache-dir -r /app/requirements.txt

COPY app /app/app
COPY run.py /app/run.py
COPY run_api.py /app/run_api.py

# Por defecto no expone nada; la API la exponemos desde compose
CMD ["python", "run_api.py"]