#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

char SYMBOL[16];

// struct who represents a stock
typedef struct {
  char symbol[10];
  double price;
  int volume;
  // timestamp time
  double volatility;
} Stock;

// a struct who creates a stock
Stock create_stock(const char *symbol, double price, int volume)
{
    Stock s;
    snprintf(s.symbol, sizeof(s.symbol), "%s", symbol);
    s.price = price;
    s.volume = volume;
    //s.volatility = 0.0;
      // Below are additions to the dynamic properties of the stock.
  //s.timestamp = timestamp;
  // volatility is market update (changes dictated by new cycle, geopolitics, natural disasters, changes in industry, etc.)
  //s.volatility = volatility; // in this case market volatility is dictated by temperature sensor.
    return s;
}


// Parameters for Geometric Brownian Motion
double S = 100.0;       // Initial price
const double mu = 0.05;       // Drift (annualized expected return)
const double sigma = 0.2;     // Volatility (annualized)
const double dt = 1.0 / 252;  // Time step (1 trading day in years)

// Generate a normally distributed random number using Box-Muller transform
//double randNormal() {
//  double u1 = (double)random(1, 10000) / 10000.0;
//  double u2 = (double)random(1, 10000) / 10000.0;
// return sqrt(-2.0 * log(u1)) * cos(2.0 * M_PI * u2);
//}

double randNormal(void)
{
    double u1 = ((double)rand() + 1.0) / ((double)RAND_MAX + 2.0);
    double u2 = ((double)rand() + 1.0) / ((double)RAND_MAX + 2.0);
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
	double volatile_var = (double)rand() / (double)RAND_MAX;
	return volatile_var;
}

int main(void)
{
    srand((unsigned int)time(NULL));

    double price = nextPrice();
    printf("Price: %f\n", price);

    scanf("%15s", SYMBOL);
    Stock s = create_stock(SYMBOL, 100.00, 100);
    printf("STOCK: %s \n", s.symbol);

    //return 0;
}
