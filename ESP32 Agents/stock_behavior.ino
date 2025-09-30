/*
 * Project Name: Stock Simulation
 * Creator: Raphael
 * Date: 09 / 29 / 2025
 * 
 * Objective:
 *  This is a simulation of a stock through synthetic means.
 * 
 * Anatomy of the stock:
 *  The simulation includes static qualities of a stock such as:
 *    - price
 *    - volume
 *    - symbol
 * 
 *  The simulation also includes dynamic properties:
 *    - volatility in price as per geopolitocal, news, and fluctuations inindustry
 *    - changes in volume as per purchase and sale of stock
 *    - changes in price as per purchse and sale of stock
 * 
 * Synthetic Means to Generate Behavior:
 *  The ESP32 is connected to sensors as a means to simulate the effects of news, 
 *  natural disasters, geopolitics, and other phenomena known to impact the stocks.
 * 
 * 
 * 
 * Libraries necessary:
 *  - OneWire (for the DS18x20 series of temperature sensor)
*/

#include <stdio.h>
#include <string.h>
#include <math.h>
#include <OneWire.h> // Temperature sensor library

const int DS18S20_Pin = 26; //DS18S20 Signal pin on digital pin 26

OneWire  ds(DS18S20_Pin);  // on pin 26 (a 4.7K resistor is necessary)

// struct who represents a stock
typedef struct {
  char symbol[10];
  double price;
  int volume;
} Stock;

// a struct who represents a stock
Stock create_stock(const char *symbol, double price, int volume){
  Stock s;
  snprintf(s.symbol, sizeof(s.symbol), "%s", symbol);  // safe copy, always null-terminated
  s.price = price;
  s.volume = volume;
  
  // Below are additions to the dynamic properties of the stock.
  // include ability to add time stamp
  // s.timestamp = timestamp
  // include market update (changes dictated by new cycle, geopolitics, natural disasters, changes in industry, etc.)
  // s.volatility = volatility
}

volatile float celsius, fahrenheit; // Variables to influence market that also will update the Market Update

// Parameters for Geometric Brownian Motion
double S = 100.0;       // Initial price
double mu = 0.05;       // Drift (annualized expected return)
double sigma = 0.2;     // Volatility (annualized)
double dt = 1.0 / 252;  // Time step (1 trading day in years)

// Generate a normally distributed random number using Box-Muller transform
double randNormal() {
  double u1 = (double)random(1, 10000) / 10000.0;
  double u2 = (double)random(1, 10000) / 10000.0;
  return sqrt(-2.0 * log(u1)) * cos(2.0 * M_PI * u2);
}

// Simulate one price tick
double nextPrice() {
  double Z = randNormal();
  double dS = S * (mu * dt + sigma * sqrt(dt) * Z);
  S = S + dS;
  return S;
}

// function to update the price of a stock
void update_price(Stock *s, double new_price){
  s->price = new_price;
}


// this function is for the hedge fund to sell shares
void sell_shares(Stock *s, int shares_to_sell){
  if (s->volume >= shares_to_sell){
    s->volume -= shares_to_sell;
  } else {
    printf("Insufficient volume to make sale \n");
  }
}

// this function is for the hedge fund to purchase shares
void buy_shares(Stock *s, int amount_to_buy){
  if (s->volume >= amount_to_buy){
    sell_shares(s, amount_to_buy);
  } else {
    printf("Insufficient shares of stock available to make purchase \n");
  }
}


/*
 * To-do:
 * 1. Include sensor[x]
 * 2. make variable for sensor that will update price of stock
 * 3. make a function that will generate stock tick
 * 4. 

*/

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

}

void loop() {
//  getTemp();

}


// set the changes to the stock from change in temperature through an ISR where changes to stock are only valid from a temp. change
void getTemp(){
  //returns the temperature from one DS18S20
   byte i;
  byte present = 0;
  byte type_s;
  byte data[9];
  byte addr[8];
//  float celsius, fahrenheit;
  
  if ( !ds.search(addr)) {
    Serial.println("No more addresses.");
    Serial.println();
    ds.reset_search();
    delay(250);
    return;
  }
  
  Serial.print("ROM =");
  for( i = 0; i < 8; i++) {
    Serial.write(' ');
    Serial.print(addr[i], HEX);
  }

  if (OneWire::crc8(addr, 7) != addr[7]) {
      Serial.println("CRC is not valid!");
      return;
  }
  Serial.println();
 
  // the first ROM byte indicates which chip
  switch (addr[0]) {
    case 0x10:
      Serial.println("  Chip = DS18S20");  // or old DS1820
      type_s = 1;
      break;
    case 0x28:
      Serial.println("  Chip = DS18B20");
      type_s = 0;
      break;
    case 0x22:
      Serial.println("  Chip = DS1822");
      type_s = 0;
      break;
    default:
      Serial.println("Device is not a DS18x20 family device.");
      return;
  } 

  ds.reset();
  ds.select(addr);
  ds.write(0x44, 1);        // start conversion, with parasite power on at the end
  
  delay(1000);     // maybe 750ms is enough, maybe not
  // we might do a ds.depower() here, but the reset will take care of it.
  
  present = ds.reset();
  ds.select(addr);    
  ds.write(0xBE);         // Read Scratchpad

  Serial.print("  Data = ");
  Serial.print(present, HEX);
  Serial.print(" ");
  for ( i = 0; i < 9; i++) {           // we need 9 bytes
    data[i] = ds.read();
    Serial.print(data[i], HEX);
    Serial.print(" ");
  }
  Serial.print(" CRC=");
  Serial.print(OneWire::crc8(data, 8), HEX);
  Serial.println();

  // Convert the data to actual temperature
  // because the result is a 16 bit signed integer, it should
  // be stored to an "int16_t" type, which is always 16 bits
  // even when compiled on a 32 bit processor.
  int16_t raw = (data[1] << 8) | data[0];
  if (type_s) {
    raw = raw << 3; // 9 bit resolution default
    if (data[7] == 0x10) {
      // "count remain" gives full 12 bit resolution
      raw = (raw & 0xFFF0) + 12 - data[6];
    }
  } else {
    byte cfg = (data[4] & 0x60);
    // at lower res, the low bits are undefined, so let's zero them
    if (cfg == 0x00) raw = raw & ~7;  // 9 bit resolution, 93.75 ms
    else if (cfg == 0x20) raw = raw & ~3; // 10 bit res, 187.5 ms
    else if (cfg == 0x40) raw = raw & ~1; // 11 bit res, 375 ms
    //// default is 12 bit resolution, 750 ms conversion time
  }
  celsius = (float)raw / 16.0;
  fahrenheit = celsius * 1.8 + 32.0;
  Serial.print("  Temperature = ");
  Serial.print(celsius);
  Serial.print(" Celsius, ");
  Serial.print(fahrenheit);
  Serial.println(" Fahrenheit");
}
