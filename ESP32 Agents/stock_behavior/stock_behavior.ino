/*
 * Project Name: Stock Simulation
 * Creator: Raphael
 * Date: 12 / 05 / 2026
 * Status: Work in Progress
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





/*
 * To-do:
 * 1. Integrate ISR for temperature change to relate to stock in real time
 * 2. Integrate into the struct Stock the update of temperature change as a variable
 * 3. Create Market-Update variable in struct Stock to update as a means to represent changes in news, geopolitics, natural disasters, changes in industry, etc.  
 * 4. Integrate GBM into a variable of the struct stock.
 * 5. Add timestamps as variable in struct of stock.
 * 6. These all integrated, we have a stock who operates dynamically in real time to events and standard market fluctuations.

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
  // timestamp time
  double volatility;
} Stock;

// a struct who creates a stock
Stock create_stock(const char *symbol, double price, int volume){
  Stock s;
  snprintf(s.symbol, sizeof(s.symbol), "%s", symbol);  // safe copy, always null-terminated
  s.price = price;
  s.volume = volume;
  // Below are additions to the dynamic properties of the stock.
  //s.timestamp = timestamp;
  // volatility is market update (changes dictated by new cycle, geopolitics, natural disasters, changes in industry, etc.)
  //s.volatility = volatility; // in this case market volatility is dictated by temperature sensor.
}

volatile float celsius, fahrenheit; // Variables to influence market that also will update the Market Update

// Parameters for Geometric Brownian Motion
double S = 100.0;       // Initial price
const double mu = 0.05;       // Drift (annualized expected return)
const double sigma = 0.2;     // Volatility (annualized)
const double dt = 1.0 / 252;  // Time step (1 trading day in years)

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

void setup() {
  Serial.begin(115200);
}

void loop() {
  double price = nextPrice();
  Serial.print("Price: ");Serial.print(price);
  Serial.print(" ");
  delay(150);
  getTemp();
  Serial.println();
}

/*
How would the temp change affect the price?
*/

// set the changes to the stock from change in temperature through an ISR where changes to stock are only valid from a temp. change
void getTemp(){
  //returns the temperature from one DS18S20
   byte i;
  byte present = 0;
  byte type_s;
  byte data[9];
  byte addr[8];
  float celsius, fahrenheit;
  
  if ( !ds.search(addr)) {
//    Serial.println("No more addresses.");
//    Serial.println();
    ds.reset_search();
    delay(250);
  }
  
//  Serial.print("ROM =");
  for( i = 0; i < 8; i++) {
//    Serial.write(' ');
//    Serial.print(addr[i], HEX);
  }

  if (OneWire::crc8(addr, 7) != addr[7]) {
//      Serial.println("CRC is not valid!");
  }
//  Serial.println();
 
  // the first ROM byte indicates which chip
  switch (addr[0]) {
    case 0x10:
//      Serial.println("  Chip = DS18S20");  // or old DS1820
      type_s = 1;
      break;
    case 0x28:
//      Serial.println("  Chip = DS18B20");
      type_s = 0;
      break;
    case 0x22:
//      Serial.println("  Chip = DS1822");
      type_s = 0;
      break;
    default:
//      Serial.println("Device is not a DS18x20 family device.");
      break;
  } 

  ds.reset();
  ds.select(addr);
  ds.write(0x44, 1);        // start conversion, with parasite power on at the end
  
  delay(1000);     // maybe 750ms is enough, maybe not
  // we might do a ds.depower() here, but the reset will take care of it.
  
  present = ds.reset();
  ds.select(addr);    
  ds.write(0xBE);         // Read Scratchpad

//  Serial.print("  Data = ");
//  Serial.print(present, HEX);
//  Serial.print(" ");
  for ( i = 0; i < 9; i++) {           // we need 9 bytes
    data[i] = ds.read();
//    Serial.print(data[i], HEX);
//    Serial.print(" ");
  }
//  Serial.print(" CRC=");
//  Serial.print(OneWire::crc8(data, 8), HEX);
//  Serial.println();

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
//  Serial.print("  Temperature = ");
//  Serial.print(celsius);
//  Serial.print(" Celsius, ");
  
  Serial.print(" Fahrenheit: ");
  Serial.print(fahrenheit);
}
