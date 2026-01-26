import json
import time
import paho.mqtt.client as mqtt
from datetime import datetime, timezone
# timestamp = datetime.now(timezone.utc).isoformat()
# e.g., '2026-01-25T13:45:12.123456+00:00'

BROKER = "localhost"
PORT = 1883

# In-memory market state (authoritative)
market_state = {}

# File to store all ticks
LOG_FILE = "market_log.json"

# ------------------ MQTT callbacks ------------------

def on_connect(client, userdata, flags, reason_code, properties):
    client.subscribe([
        ("market/raw/stock", 0),
        ("market/request/stock", 0)
    ])
    print("Broker online, rc =", reason_code)

def on_message(client, userdata, msg):
    global market_state

    payload = msg.payload.decode().strip()

    # ---- Ingest raw market data ----
    if msg.topic == "market/raw/stock":
        parts = payload.split()

        if len(parts) != 4:
            print("Rejected malformed payload:", payload)
            return

        symbol, price_s, volume_s, vol_s = parts

        try:
            price = float(price_s)
            volume = int(volume_s)
            volatility = float(vol_s)
        except ValueError:
            print("Rejected non-numeric values:", payload)
            return

        # Create cleaned tick
        clean = {
            "symbol": symbol,
            "price": round(price, 2),
            "volume": volume,
            "volatility": max(0.0, min(volatility, 1.0)),
            "ts": datetime.now(timezone.utc).isoformat()  # UTC timestamp
        }

        # Update in-memory market state
        market_state[symbol] = clean

        # Publish cleaned tick
        client.publish(
            f"market/clean/stock/{symbol}",
            json.dumps(clean),
            retain=True
        )

        # Append to JSON log file (one JSON object per line)
        with open(LOG_FILE, "a") as f:
            f.write(json.dumps(clean) + "\n")

        print("Cleaned market tick:", clean)

    # ---- Serve on request (pull model) ----
    elif msg.topic == "market/request/stock":
        symbol = payload
        if symbol in market_state:
            client.publish(
                f"market/clean/stock/{symbol}",
                json.dumps(market_state[symbol]),
                retain=True
            )
            print("Served request for", symbol)

# ------------------ MQTT client setup ------------------

client = mqtt.Client(
    client_id="pi-market-broker",
    protocol=mqtt.MQTTv311,
    callback_api_version=mqtt.CallbackAPIVersion.VERSION2
)

client.on_connect = on_connect
client.on_message = on_message

client.connect(BROKER, PORT, 60)
client.loop_forever()
