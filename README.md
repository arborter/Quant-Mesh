# QuantMesh: A Distributed Microcontroller-Based Market & Trading System
 
This project simulates a miniature financial market using a network of ESP32 microcontrollers and a Raspberry Pi.

Each ESP32 acts as an **agent** that generates a synthetic stock price using stochastic processes like **Geometric Browninan Motion (GBM)**, **random walks**, and random number distribution.
A central ESP32 acts as a **hedge fund** who analyzes the market, calculates risk, asseses whether to buy, sell, short, etc., then makes trade decisions in real time.

## Operation

The Pi has two operations:
- It behaves as an exchange (further refereed to as **PiSE**).
- It behaves as a prime-broker (further referred to as **Broker**).

1. Agents send their data to a synthetic exchange on the PiSE over **MQTT**.
2. The PiSE organizes the data from each agent into the following feeds:
    - Timestamps 
    - Trade prices 
    - Market status
3. The Broker subscribes to different streams at the request of the hedge fund.
4. The Broker can aggregate data from streams, clean the data, and provide data in structure useful for the hedge fund.
5. The hedge fund actively monitors feeds to assess and simulates trading strategies.

## Injecting Volatility

The ESP32s are connected to sensors as a means to simulate the effects of news, natural disasters, geopolitics, and other phenomena that impact the stock market.

The following system is in place:
1. Each agent and the hedge fund is connected to a central temperature sensor.
2. Some agents behave as **correlated pairs** such that they are connected by an additional sensor that is either a touch sensor, humidity sensor, accelorometer, or the ultra-sonic sensor.
3. Some agents have a sensor that is exclusive to them to simulate changes in the industry of the asset they represent.

## Features

- **Stochastic Stock Simulation**
  - Uniform random numbers
  - Coin flips and dice rolls
  - Box–Muller transform for normal distribution samples
  - Random walk processes
  - Geometric Brownian Motion for stock prices

- **Sensor-Based Correlations**
  - Agents can share sensor inputs (e.g., light sensor → correlated movement).
  - Global sensors (e.g., temperature or accelerometer) inject volatility shocks into all stocks.

- **Communication Layer**
  - Uses **MQTT** over WiFi.
  - Agents **publish** their stock data to topics.
  - The hedge fund ESP32 **subscribes** to these topics to gather and trade on the data.

- **Scalable Design**
  - Each ESP32 represents multiple stocks (e.g., 5 per agent).
  - Central hedge fund can monitor dozens of streams.

---

## Architecture

- **Agents**: Simulate stock prices and publish data.  
- **MQTT Broker**: Routes messages between devices (e.g., Mosquitto running on a Raspberry Pi).  
- **Hedge Fund ESP32**: Subscribes to topics, processes data, and simulates trading strategies.

---

## Requirements

- ESP32 development boards (ESP32, ESP32-S2, ESP32-PICO Mini, etc.)
- MQTT broker (e.g., Mosquitto on Raspberry Pi or local PC)
- Arduino IDE or ESP-IDF
- WiFi connection
- Optional: sensors (light, temperature, accelerometer) for correlated inputs

---

## Example Simulation Functions

```c
// Uniform random between 0 and 1
double uniform() {
    return (double) rand() / RAND_MAX;
}

// Normal distribution sample (Box–Muller transform)
double normal_sample() {
    double u1 = uniform();
    double u2 = uniform();
    return sqrt(-2.0 * log(u1)) * cos(2 * M_PI * u2);
}

// Geometric Brownian Motion for asset prices
void gbm(double S0, double mu, double sigma, double T, int steps) {
    double dt = T / steps;
    double logS = log(S0);
    for (int i = 0; i < steps; i++) {
        double z = normal_sample();
        logS += (mu - 0.5 * sigma * sigma) * dt + sigma * sqrt(dt) * z;
        double price = exp(logS);
        printf("%f\n", price);
    }
}
```