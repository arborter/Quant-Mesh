#include <WiFi.h>
#include <PubSubClient.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <esp_system.h> // utilize hardware for randomsignal generation

// esp32 pico
// this is the agent

const char* ssid = "----";
const char* password = "----";
const char* mqttServer = "00.00.00.00"; // IP of the Raspberry Pi
const int mqttPort = 1900;
const char* mqttTopic = "market/raw/stock";

/*  STOCK MARKET SIMULATOR CODE BEGIN */

int usable_number;

char SYMBOL[16];

// struct who represents a stock
typedef struct {
  char symbol[10];
  double price;
  int volume;
  // timestamp time
  double volatility;
} Stock;

// function who creates a stock
Stock create_stock(const char *symbol, double (*funct_price)(void), int (*funct_vol)(void), double (*funct)(void)){
  Stock s;
  snprintf(s.symbol, sizeof(s.symbol), "%s", symbol);  // safe copy, always null-terminated
  s.price = funct_price();
  s.volume = funct_vol();
  // Below are additions to the dynamic properties of the stock.
  //s.timestamp = timestamp;
  // volatility is market update (changes dictated by new cycle, geopolitics, natural disasters, changes in industry, etc.)
  s.volatility = funct(); // in this case market volatility is dictated by temperature sensor.
  return s;
}

// Parameters for Geometric Brownian Motion
double S = 100.0;       // Initial price
const double mu = 0.05;       // Drift (annualized expected return)
const double sigma = 0.2;     // Volatility (annualized)
const double dt = 1.0 / 252;  // Time step (1 trading day in years)

// Generate a normally distributed random number using Box-Muller transform
double randNormal(void) {
  double u1 = (double)random(1, 10000) / 10000.0;
  double u2 = (double)random(1, 10000) / 10000.0;
  return sqrt(-2.0 * log(u1)) * cos(2.0 * M_PI * u2);
}

// Simulate one price tick
double nextPrice(void) {
  double Z = randNormal();
  double dS = S * (mu * dt + sigma * sqrt(dt) * Z);
  S = S + dS;
  return S;
}

// This function injects a randomn variable to
//inject into the stock
double inject_volatility(void){
  double volatile_var = (double)rand() / (double)RAND_MAX;
  return volatile_var;
}

//this function returns volume of a stock
int create_volume(void){
  return (rand() % 1000) + 1;
}

/*  STOCK MARKET SIMULATOR CODE END */


char payload[50];

 // create_stock(const char *symbol, double (*funct_price)(void), int (*funct_vol)(void), double (*funct)(void))
 Stock test = create_stock("STOCK", nextPrice, create_volume, inject_volatility);

/*  MQTT CODE BEGIN */

WiFiClient espClient;
PubSubClient client(espClient);

const int rgb_Pin = 2; // RGB LED
const int red_pin = 13; // RED LED
const int buttonPin = 38;  // input-only pin BUTTON

bool lightOn = false; // stores the state of the LED
const int threshold = 100; // brightness threshold

void setup() {
  Serial.begin(115200);
  pinMode(rgb_Pin, OUTPUT);
  pinMode(red_pin, OUTPUT);
  pinMode(buttonPin, INPUT);

  // Connect to WiFi
  WiFi.mode(WIFI_STA);delay(500);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    digitalWrite(red_pin, HIGH);
    delay(500); Serial.println(".");
    digitalWrite(red_pin, LOW);Serial.println(usable_number);

    Serial.print("WiFi status: ");
    Serial.println(WiFi.status());
  }
  Serial.println("\nConnected to WiFi");

  // Connect to Mosquitto Broker
  client.setServer(mqttServer, mqttPort);
  while (!client.connected()) {
    Serial.print("Connecting to MQTT...");
    if (client.connect("ESP32Publisher")) {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc= ");
      Serial.println(client.state());
      delay(2000);
    }
  }

  // Random Number Generation using Random-Number-Generator Data Register:
  while(true){
    srand(esp_random());
    Serial.print("ESP  Random Number = ");
    uint32_t r = rand();
    usable_number = r % 1001;   // 0–999
    Serial.println(usable_number);
    break;
  }

  // create_stock(const char *symbol, double (*funct_price)(void), int (*funct_vol)(void), double (*funct)(void))
  //Stock test = create_stock("STOCK", nextPrice, create_volume, inject_volatility);
  
  Serial.println();
  Serial.print("Symbol = "); Serial.println(test.symbol);
  Serial.print("Price = "); Serial.println(test.price);
  Serial.print("Volume = "); Serial.println(test.volume);
  Serial.print("Volatility = "); Serial.println(test.volatility);
  snprintf(payload, sizeof(payload),"%s %f %u %f", test.symbol, test.price, test.volume, test.volatility);
  Serial.println();
 
}

/*  MQTT CODE END */


/*  MQTT + STOCK AGENT BEGIN */

void loop() {
  int lightLevel = analogRead(rgb_Pin); // read the current brightness
  int reading_red = digitalRead(red_pin);
  test.price = nextPrice();
  snprintf(payload, sizeof(payload),"%s %f %u %f", test.symbol, test.price, test.volume, test.volatility);

  if (digitalRead(buttonPin) == LOW) {
    digitalWrite(red_pin, HIGH);
    //const char* msg = "ON";
//    snprintf(payload, sizeof(payload),"%s %f %f %u", test.symbol, test.price, test.volume, test.volatility);
    client.publish(mqttTopic, payload); // publish message to topic
    lightOn = true;
    Serial.print("Published: "); Serial.println(payload);
    delay(1000);
  } else {
    digitalWrite(red_pin, LOW);
    const char* msg = "OFF";
    client.publish(mqttTopic, msg); // publish message to topic
    lightOn = false;
    Serial.print("Published: "); Serial.println(msg);
    delay(1000);
  }
}

/*  MQTT + STOCK AGENT END */
