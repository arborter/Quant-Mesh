#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

// --- WiFi credentials ---
const char* ssid = "----";
const char* password = "----";

// --- MQTT broker ---
const char* mqttServer = "00.00.00.00"; // IP of the Raspberry Pi
const int mqtt_port = 1883;

// --- Stock symbol to follow ---
const char* stockSymbol = "STOCK";

// --- WiFi and MQTT clients ---
WiFiClient espClient;
PubSubClient client(espClient);

// --- Strategy threshold ---
float priceThreshold = 90.0;  // Example: take action if price drops below this

// --- Connect to WiFi ---
void setup_wifi() {
  Serial.print("Connecting to WiFi");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected to WiFi!");
}

// --- Callback when message is received ---
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");

  // Copy payload into a null-terminated string
  char msg[length + 1];
  memcpy(msg, payload, length);
  msg[length] = '\0';
  Serial.println(msg);

  // --- Parse JSON ---
  StaticJsonDocument<256> doc;
  DeserializationError error = deserializeJson(doc, msg);
  if (error) {
    Serial.print("JSON parse failed: ");
    Serial.println(error.c_str());
    return;
  }

  const char* symbol = doc["symbol"];
  float price = doc["price"];
  int volume = doc["volume"];
  float volatility = doc["volatility"];
  const char* ts = doc["ts"];

  Serial.print("Symbol: "); Serial.println(symbol);
  Serial.print("Price: "); Serial.println(price);
  Serial.print("Volume: "); Serial.println(volume);
  Serial.print("Volatility: "); Serial.println(volatility);
  Serial.print("Timestamp: "); Serial.println(ts);

  // --- Simple strategy: print alert if price below threshold ---
  if (price < priceThreshold) {
    Serial.println(">>> ALERT: Price below threshold! Consider buying!");
  }
  else {
    Serial.println(">>> ALERT: Consider selling!");
  }
}

// --- Reconnect to MQTT broker if disconnected ---
void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect("ESP32-Trader")) {
      Serial.println("connected");
      // Subscribe to cleaned stock feed
      String topic = String("market/clean/stock/") + stockSymbol;
      client.subscribe(topic.c_str());
      Serial.print("Subscribed to: ");
      Serial.println(topic);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 2 seconds");
      delay(2000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqttServer, mqtt_port);
  client.setCallback(callback);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
}
