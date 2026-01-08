#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

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


// This function injects a randomn variable to
//inject into the stock
double inject_volatility(void){
	// seeding for a number
	srand(time(NULL));
	double volatile_var = (double)rand() / (double)RAND_MAX;
	return volatile_var;
}

int main(void) {
  double price = nextPrice();
  printf("Price: "); printf(price);
  Serial.print(" ");
  delay(150);
  getTemp();
  Serial.println();
}
